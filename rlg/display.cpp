#include <string>
#include <ncurses.h>

#include "display.h"
#include "dungeon.h"
#include "pc.h"

using std::string;

cursesDisplay::cursesDisplay(_dungeon *dun) : abstractDisplay(dun) {
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, true);
  start_color();
  init_pair(COLOR_RED, COLOR_RED,     COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN,   COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW,  COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE,    COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN,    COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE,   COLOR_BLACK);
}

cursesDisplay::~cursesDisplay() {
  endwin();
}

void cursesDisplay::displayMap() {
  clear();
  int x, y, x_l, y_l, x_h, y_h;
  int px, py;
  px = 0;
  py = 1;

  if(d->view_mode == LOOK_MODE) {
    x_l = d->lcoords[dim_x] - 39;
    x_h = d->lcoords[dim_x] + 41;
    y_l = d->lcoords[dim_y] - 9;
    y_h = d->lcoords[dim_y] + 12;
  }

  else {
    if(d->player->getX() >= 39) {
      if(d->player->getX() <= DUNGEON_X - 41) {
        x_l = d->player->getX() - 39;
        x_h = d->player->getX() + 41;
      }

      else {
        x_h = DUNGEON_X;
        x_l = 80;
      }
    }

    else {
      x_l = 0;
      x_h = 80;
    }

    if(d->player->getY() >= 9) {
      if(d->player->getY() <= DUNGEON_Y - 12) {
        y_l = d->player->getY() - 9;
        y_h = d->player->getY() + 12;
      }

      else {
        y_h = DUNGEON_Y;
        y_l = 84;
      }
    }

    else {
      y_l = 0;
      y_h = 21;
    }
  }
  
  for(y = y_l; y < y_h; y++) {
    px = 0;
    for(x = x_l; x < x_h; x++) {
      if(x == d->player->getX() && y == d->player->getY()) {
        mvaddch(py, px, '@' | COLOR_PAIR(COLOR_YELLOW));
      }

      else if(char_gridxy(x, y) != nullptr && mapxy(x, y) != ter_wall_immutable
                              && inLosRange(x, y) 
                              && !(char_gridxy(x, y)->isDead())) {

        attron(COLOR_PAIR(char_gridxy(x, y)->getColor()));
        mvprintw(py, px, "%c", char_gridxy(x, y)->getSymbol());
        attroff(COLOR_PAIR(char_gridxy(x, y)->getColor()));
      }

      else if(d->item_grid[y][x] != nullptr && (inLosRange(x, y)
                              || d->item_grid[y][x]->isKnown())) {

        attron(COLOR_PAIR(d->item_grid[y][x]->getColor()));
        mvprintw(py, px, "%c", d->item_grid[y][x]->getSymbol());
        attroff(COLOR_PAIR(d->item_grid[y][x]->getColor()));
        d->item_grid[y][x]->setKnown();
      }

      else {
        if(inLosRange(x, y)) {
          attron(A_BOLD);
        }
        switch(d->nofog ? mapxy(x, y) : d->player->pcmap[y][x]) {
          case ter_wall:
          case ter_wall_immutable:
            mvaddch(py, px, ' ' | COLOR_PAIR(COLOR_WHITE));
            break;
          case ter_floor:
          case ter_floor_room:
            mvaddch(py, px, '.' | COLOR_PAIR(COLOR_WHITE));
            break;
          case ter_floor_hall:
            mvaddch(py, px, '#' | COLOR_PAIR(COLOR_WHITE));
            break;
          case ter_stairs_down:
            mvaddch(py, px, '>' | COLOR_PAIR(COLOR_RED));
            break;
          case ter_stairs_up:
            mvaddch(py, px, '<' | COLOR_PAIR(COLOR_RED));
            break;
          case endgame_flag:
            mvaddch(py, px, 'X');
            break;
        }
        if(inLosRange(x, y)) {
          attroff(A_BOLD);
        }
      }
      px++;
    }
    py++;
    addch('\n');
  }

  int hptemp = d->player->getHp() >= 0 ? d->player->getHp() : 0;

  const char *msg = d->disp_msg.c_str();
  string hpmsg = "HP:" + std::to_string(hptemp) + "(" 
        + std::to_string(d->player->getMaxHp()) + ")";
  string cashmsg = "$:" + std::to_string(d->player->getCash());

  mvprintw(0, 0, "%s", msg);
  mvprintw(23, 0, "%s%d", "Dlvl:", d->level);
  mvprintw(23, 8, cashmsg.c_str());
  mvprintw(23, cashmsg.size() + 9, hpmsg.c_str());
  refresh();
}

void cursesDisplay::displayInventory(displayMode mode) {
  int i;
  string to_print[12];
  clear();

  mvprintw(0, 0, "%s", d->disp_msg.c_str());

  //attron(COLOR_PAIR(COLOR_CYAN));

  if(mode == INVENTORY_MODE || mode == WEAR_MODE || mode == INSPECT_MODE 
                            || mode == DROP_MODE || mode == EXPUNGE_MODE) {

    if(mode == INVENTORY_MODE)
      mvprintw(1, 0, "--------- Inventory ---------");
    else if(mode == WEAR_MODE)
      mvprintw(1, 0, "----------- Wear -----------");
    else if(mode == INSPECT_MODE)
      mvprintw(1, 0, "---------- Inspect ----------");
    else if(mode == DROP_MODE)
      mvprintw(1, 0, "----------- Drop -----------");
    else if(mode == EXPUNGE_MODE)
      mvprintw(1, 0, "---------- EXPUNGE ----------");

    for(i = 0; i < INVENTORY_SIZE; i++) {
      if(d->player->inventory[i] != nullptr) {
        to_print[i] = d->player->inventory[i]->toString(); 
      }
      else {
        to_print[i] = "Empty";
      }
      mvprintw(i + 2, 0, "(%s) %s", std::to_string(i).c_str(), 
                   to_print[i].c_str());
    }
  }

  else if(mode == EQUIPMENT_MODE) {
    mvprintw(1, 0, "-------- Equipment --------");
    mvprintw(2, 0, "(Weapon)  %s", (d->player->equipment.weapon != nullptr 
                      ? d->player->equipment.weapon->toString().c_str() : "-----"));
    mvprintw(3, 0, "(Offhand) %s", (d->player->equipment.offhand != nullptr 
                      ? d->player->equipment.offhand->toString().c_str() : "-----"));
    mvprintw(4, 0, "(Ranged)  %s", (d->player->equipment.ranged != nullptr 
                      ? d->player->equipment.ranged->toString().c_str() : "-----"));
    mvprintw(5, 0, "(Armor)   %s", (d->player->equipment.armor != nullptr 
                      ? d->player->equipment.armor->toString().c_str() : "-----"));
    mvprintw(6, 0, "(Helmet)  %s", (d->player->equipment.helmet != nullptr 
                      ? d->player->equipment.helmet->toString().c_str() : "-----"));
    mvprintw(7, 0, "(Cloak)   %s", (d->player->equipment.cloak != nullptr 
                      ? d->player->equipment.cloak->toString().c_str() : "-----"));
    mvprintw(8, 0, "(Gloves)  %s", (d->player->equipment.gloves != nullptr 
                      ? d->player->equipment.gloves->toString().c_str() : "-----"));
    mvprintw(9, 0, "(Boots)   %s", (d->player->equipment.boots != nullptr 
                      ? d->player->equipment.boots->toString().c_str() : "-----"));
    mvprintw(10, 0, "(Amulet)  %s", (d->player->equipment.amulet != nullptr 
                      ? d->player->equipment.amulet->toString().c_str() : "-----"));
    mvprintw(11, 0, "(Light)   %s", (d->player->equipment.light != nullptr 
                      ? d->player->equipment.light->toString().c_str() : "-----"));
    mvprintw(12, 0, "(Ring)    %s", (d->player->equipment.ring_one != nullptr 
                      ? d->player->equipment.ring_one->toString().c_str() : "-----"));
    mvprintw(13, 0, "(Ring)    %s", (d->player->equipment.ring_two != nullptr 
                      ? d->player->equipment.ring_two->toString().c_str() : "-----"));
  }

  else if(mode == TAKEOFF_MODE) {
    mvprintw(1, 0, "-------- Take off --------");
    mvprintw(2, 0, "(a) %s", (d->player->equipment.weapon != nullptr 
                      ? d->player->equipment.weapon->toString().c_str() : "Empty"));
    mvprintw(3, 0, "(b) %s", (d->player->equipment.offhand != nullptr 
                      ? d->player->equipment.offhand->toString().c_str() : "Empty"));
    mvprintw(4, 0, "(c) %s", (d->player->equipment.ranged != nullptr 
                      ? d->player->equipment.ranged->toString().c_str() : "Empty"));
    mvprintw(5, 0, "(d) %s", (d->player->equipment.armor != nullptr 
                      ? d->player->equipment.armor->toString().c_str() : "Empty"));
    mvprintw(6, 0, "(e) %s", (d->player->equipment.helmet != nullptr 
                      ? d->player->equipment.helmet->toString().c_str() : "Empty"));
    mvprintw(7, 0, "(f) %s", (d->player->equipment.cloak != nullptr 
                      ? d->player->equipment.cloak->toString().c_str() : "Empty"));
    mvprintw(8, 0, "(g) %s", (d->player->equipment.gloves != nullptr 
                      ? d->player->equipment.gloves->toString().c_str() : "Empty"));
    mvprintw(9, 0, "(h) %s", (d->player->equipment.boots != nullptr 
                      ? d->player->equipment.weapon->toString().c_str() : "Empty"));
    mvprintw(10, 0, "(i) %s", (d->player->equipment.amulet != nullptr 
                      ? d->player->equipment.amulet->toString().c_str() : "Empty"));
    mvprintw(11, 0, "(j) %s", (d->player->equipment.light != nullptr 
                      ? d->player->equipment.light->toString().c_str() : "Empty"));
    mvprintw(12, 0, "(k) %s", (d->player->equipment.ring_one != nullptr 
                      ? d->player->equipment.ring_one->toString().c_str() : "Empty"));
    mvprintw(13, 0, "(l) %s", (d->player->equipment.ring_two != nullptr 
                      ? d->player->equipment.ring_two->toString().c_str() : "Empty"));
  }

  //attroff(COLOR_PAIR(COLOR_CYAN));

  refresh();
}

void cursesDisplay::displayCharacterStats() {
  clear();
  string dmg;

  if(d->player->equipment.weapon != nullptr) 
     dmg.append(d->player->equipment.weapon->getDamage()->toString().c_str());
  else
     dmg.append(d->player->getDamage()->toString().c_str());

  if(d->player->equipment.offhand != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.offhand->getDamage()->toString()); 
  }
  if(d->player->equipment.ranged != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.ranged->getDamage()->toString()); 
  }
  if(d->player->equipment.armor != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.armor->getDamage()->toString()); 
  }
  if(d->player->equipment.helmet != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.helmet->getDamage()->toString()); 
  }
  if(d->player->equipment.cloak != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.cloak->getDamage()->toString()); 
  }
  if(d->player->equipment.gloves != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.gloves->getDamage()->toString()); 
  }
  if(d->player->equipment.boots != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.boots->getDamage()->toString()); 
  }
  if(d->player->equipment.amulet != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.amulet->getDamage()->toString()); 
  }
  if(d->player->equipment.light != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.light->getDamage()->toString()); 
  }
  if(d->player->equipment.ring_one != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.ring_one->getDamage()->toString()); 
  }
  if(d->player->equipment.ring_two != nullptr) {
    dmg.append(" ");
    dmg.append(d->player->equipment.ring_two->getDamage()->toString()); 
  }

  mvprintw(0, 0, d->disp_msg.c_str());
  mvprintw(1, 0, "---------- %s ----------", d->player->getName().c_str());
  mvprintw(2, 0, "%s", d->player->getDesc().c_str());
  mvprintw(3, 0, "HP: %d(%d)", d->player->getHp(), d->player->getMaxHp());
  mvprintw(4, 0, "Damage: %s", dmg.c_str());
  mvprintw(5, 0, "Dodge: %d", d->player->getDodge());
  mvprintw(6, 0, "Hit: %d", d->player->getHit());
  mvprintw(7, 0, "Speed: %d", d->player->getSpeed());
  mvprintw(8, 0, "Weight: %d", d->player->getWeight());
  mvprintw(9, 0, "Light radius: %d", d->player->getLightRadius());

  refresh();
}

void cursesDisplay::displayItemDescription(item *i) {
  clear();

  //attron(COLOR_PAIR(COLOR_CYAN));

  mvprintw(0, 0, d->disp_msg.c_str());
  mvprintw(1, 0, "---------- %s ----------", i->getName().c_str());
  mvprintw(2, 0, "%s", i->getDesc().c_str());

  //attroff(COLOR_PAIR(COLOR_CYAN));
  
  refresh(); 
}

void cursesDisplay::endGameScreen(int mode) {
  if(mode == NPC_MODE) {
    displayMap();
    getch();
    clear();
    mvprintw(11, 31, "GAME OVER");
  }
  else if(mode == PC_MODE) {
    displayMap();
    getch();
    clear();
    mvprintw(11, 32, "YOU WIN!");
  }
  else {
    clear();
    mvprintw(11, 36, "QUIT");
  }

  getch();
  refresh();
}

inline int cursesDisplay::inLosRange(int x, int y) {
  if(d->nofog) {
    return 1;
  }
  else if(abs(x - d->player->getX()) <= d->player->getLightRadius() && 
          (abs(y - d->player->getY()) <= d->player->getLightRadius())) {
    return 1;
  }
  return 0;
}
