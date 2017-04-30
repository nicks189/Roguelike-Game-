#include "dungeon.h"
#include "display.h"

projectile::projectile(_dungeon *dun, int x, int y, int dir, int dam)
                    : d(dun), x(x), y(y), direction(dir), damage(dam) {

  color = COLOR_YELLOW;
  x = 0;
  y = 0;
  eff = nullptr;
  symbol = '*'; 
  travelDistance = 0;
}

void projectile::increment() {
  switch(direction) {
    case NW:
      y--;
      x--;
      break;
    case N:
      y--;
      break;
    case NE:
      x++;
      y--;
      break;
    case E:
      x++;
      break;
    case SE:
      x++;
      y++;
      break;
    case S:
      y++;
      break;
    case SW:
      x--;
      y++;
      break;
    case W:
      x--;
      break;
  }
}

int projectile::move() {
  increment();

  if(!(mapxy(x, y) == ter_floor_hall || mapxy(x, y) == ter_floor_room 
    || mapxy(x, y) == ter_stairs_down || mapxy(x, y) == ter_stairs_up)) {

    if(eff)
      eff->trigger(x, y);

    return 0;
  }
  else if(travelDistance >= MAX_TRAVEL_DISTANCE) {

    if(eff)
      eff->trigger(x, y);

    return 0;
  }
  else if(char_gridxy(x, y) != nullptr) {
    npc *c = (npc *) char_gridxy(x, y);
    c->setHp(c->getHp() - damage);

    d->disp_msg = "Hit "; 
    d->disp_msg.append(c->getName()); 
    d->disp_msg.append(" for ");
    d->disp_msg.append(std::to_string(damage));

    if(c->getHp() < 1) {
      c->setHp(0);
      d->disp_msg.append("-");
      d->disp_msg.append(c->getName());
      d->disp_msg.append(" was slain");

      d->player->setScore(d->player->getScore() + 100 * (c->getLevel() + 1));

      c->kill(); 

      if(c->getItem() != nullptr) {
        d->item_grid[c->getY()][c->getX()] = c->getItem();
        c->setItem(nullptr);
      }

     char_gridxy(x, y) = nullptr;
    }
    else {
      d->disp_msg.append("-");  
      d->disp_msg.append(c->getName());
      d->disp_msg.append(" has ");
      d->disp_msg.append(std::to_string(c->getHp()));
      d->disp_msg.append(" remaining");
    }

    if(eff) 
      eff->trigger(x, y);

    return 0;
  }
 
 travelDistance++;

 return 1;
}

//int spell::move() {
//  increment();
//
//  if(!(mapxy(x, y) == ter_floor_hall || mapxy(x, y) == ter_floor_room 
//    || mapxy(x, y) == ter_stairs_down || mapxy(x, y) == ter_stairs_up)) {
//    eff->trigger(x, y);
//    return 0;
//  }
//  else if(travelDistance >= MAX_TRAVEL_DISTANCE) {
//    eff->trigger(x, y);
//    return 0;
//  }
//  else if(char_gridxy(x, y) != nullptr) {
//    npc *c = (npc *) char_gridxy(x, y);
//    c->setHp(c->getHp() - damage);
//
//    d->disp_msg = "Hit "; 
//    d->disp_msg.append(c->getName()); 
//    d->disp_msg.append(" for ");
//    d->disp_msg.append(std::to_string(damage));
//
//    if(c->getHp() < 1) {
//      c->setHp(0);
//      d->disp_msg.append("-");
//      d->disp_msg.append(c->getName());
//      d->disp_msg.append(" was slain");
//
//      c->kill(); 
//
//      if(c->getItem() != nullptr) {
//        d->item_grid[c->getY()][c->getX()] = c->getItem();
//        c->setItem(nullptr);
//      }
//
//     char_gridxy(x, y) = nullptr;
//    }
//
//    else {
//      d->disp_msg.append("-");  
//      d->disp_msg.append(c->getName());
//      d->disp_msg.append(" has ");
//      d->disp_msg.append(std::to_string(c->getHp()));
//      d->disp_msg.append(" remaining");
//    }
//
//    eff->trigger(x, y);
//    return 0;
//  }
// 
// travelDistance++;
//
// return 1;
//}
