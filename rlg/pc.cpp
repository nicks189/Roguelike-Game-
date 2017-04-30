#include <climits>

#include "pc.h"
#include "pathfinding.h"
#include "dungeon.h"
#include "projectile.h"
#include "effect.h"

pc::pc(_dungeon *dun, int16_t *loc) : character(dun), inventory() {
  name = "Player";
  description = "A brave adventurer";
  symbol = '@';
  speed = 10;
  x = loc[dim_x];
  y = loc[dim_y];
  prev[dim_x] = x;
  prev[dim_y] = y;
  initMap();
  updateMap();
}

pc::pc(_dungeon *dun) : character(dun), inventory() {
  name = "Randy";
  description = "the savage";
  hp = DEFAULT_HP;
  maxhp = hp;
  dice *dicetemp = new dice(0, 1, 4);
  damage = dicetemp;
  cash = 0;
  weight = 0;
  strength = 0;
  rangedAP = 0;
  score = 0;
  lightRadius = DEFAULT_LIGHT_RADIUS;
  speed = DEFAULT_SPEED;
  currentClass = NO_CLASS;
  fireball_is_learned = false;
  heal_is_learned = false;
  level = 1;
  symbol = '@';
  color = COLOR_YELLOW;
  place(d);
}

void pc::place(_dungeon *dun) {
  d = dun;
  x = rand_range(d->rooms[0].x + 1,
                             d->rooms[0].length - 1 + d->rooms[0].x);
  y = rand_range(d->rooms[0].y + 1,
                             d->rooms[0].height - 1 + d->rooms[0].y);
  prev[dim_x] = x;
  prev[dim_y] = y;
  next[dim_x] = x;
  next[dim_y] = y;

  initMap();
  updateMap();
}

int pc::attackPos() {
  if(char_gridpair(next) != nullptr) {
    npc *n = (npc *) char_gridpair(next);

    /* 
     * First determine hit or miss --
     *
     * Considering 70 as default hit chance, hit 
     * rating is added to this and then subtracted 
     * by targets defense rating. This is the hit 
     * chance.        
     * */
    int temphit = hit;
    temphit -= n->getDodge(); 

    if(temphit < rand_range(0, 100)) {
      d->disp_msg = "Attack missed ";
      d->disp_msg.append(n->getName()); 

      next[dim_x] = x;
      next[dim_y] = y;
      return 1;
    }

    /* Rolls for damage */
    int dmg = 0;
    dmg += strength;
    if(equipment.weapon == nullptr) 
      dmg += damage->roll();
    else 
      dmg += equipment.weapon->getDamage()->roll(); 
    if(equipment.offhand != nullptr) 
      dmg += equipment.offhand->getDamage()->roll(); 
    if(equipment.ranged != nullptr) 
      dmg += equipment.ranged->getDamage()->roll(); 
    if(equipment.armor != nullptr) 
      dmg += equipment.armor->getDamage()->roll(); 
    if(equipment.helmet != nullptr) 
      dmg += equipment.helmet->getDamage()->roll(); 
    if(equipment.cloak != nullptr) 
      dmg += equipment.cloak->getDamage()->roll(); 
    if(equipment.gloves != nullptr) 
      dmg += equipment.gloves->getDamage()->roll(); 
    if(equipment.boots != nullptr) 
      dmg += equipment.boots->getDamage()->roll(); 
    if(equipment.amulet != nullptr) 
      dmg += equipment.amulet->getDamage()->roll(); 
    if(equipment.light != nullptr) 
      dmg += equipment.light->getDamage()->roll(); 
    if(equipment.ring_one != nullptr) 
      dmg += equipment.ring_one->getDamage()->roll(); 
    if(equipment.ring_two != nullptr) 
      dmg += equipment.ring_two->getDamage()->roll(); 

    d->disp_msg = "";
    d->disp_msg.append("You hit "); 
    d->disp_msg.append(char_gridpair(next)->getName()); 
    d->disp_msg.append(" for ");
    d->disp_msg.append(std::to_string(dmg));

    n->setHp(n->getHp() - dmg);

    if(n->getHp() < 1) {
      n->setHp(0);
      d->disp_msg.append("-");
      d->disp_msg.append(n->getName());
      d->disp_msg.append(" was slain");

      score += 100 * (n->getLevel() + 1);

      n->kill(); 
      if(n->getItem() != nullptr) {
        if(d->item_grid[n->getY()][n->getX()] == nullptr) {
          d->item_grid[n->getY()][n->getX()] = n->getItem();
          n->setItem(nullptr);
        }
        else {
          delete n->getItem();
          n->setItem(nullptr);
        }
      }

      char_gridpair(next) = nullptr;
    }

    else {
      d->disp_msg.append("-");  
      d->disp_msg.append(n->getName());
      d->disp_msg.append(" has ");
      d->disp_msg.append(std::to_string(n->getHp()));
      d->disp_msg.append(" remaining");
    }

    next[dim_x] = x;
    next[dim_y] = y;
  }
  return 0;
}

inline int pc::command(int cmd, displayMode mode) {
  if(cmd == QUIT_GAME) {
    return end_game(d, BOTH_MODE);
  }
  else if(mode == INVENTORY_MODE) {
    return inventoryCommand(cmd);
  }
  else if(mode == EQUIPMENT_MODE) {
    return equipmentCommand(cmd);
  }
  else if(mode == WEAR_MODE) {
    return wearCommand(cmd);
  }
  else if(mode == TAKEOFF_MODE) {
    return takeOffCommand(cmd);
  }
  else if(mode == DROP_MODE) {
    return dropCommand(cmd);
  }
  else if(mode == EXPUNGE_MODE) {
    return expungeCommand(cmd);
  }
  else if(mode == INSPECT_MODE) {
    return inspectCommand(cmd);
  }
  return 0;
}

inline int pc::statsCommand(int cmd) {
  if(cmd == QUIT_GAME) {
    return end_game(d, BOTH_MODE);
  }
  else if(cmd == STATS_SCREEN || CONTROL_MODE) {
    return 0;
  }
  return 1;
}

inline int pc::inventoryCommand(int cmd) {
  if(cmd == INVENTORY_SCREEN || cmd == CONTROL_MODE) {
    return 0;
  } 

  return 1;
}

inline int pc::equipmentCommand(int cmd) {
  if(cmd == EQUIPMENT_SCREEN || cmd == CONTROL_MODE) {
    return 0;
  }
  return 1;
}

inline int pc::wearCommand(int cmd) {
  int index = INT_MAX;

  if(cmd == WEAR_SCREEN || cmd == CONTROL_MODE)
    return 0;
  else if(cmd == SLOT0)
    index = 0;
  else if(cmd == SLOT1)
    index = 1;
  else if(cmd == SLOT2)
    index = 2;
  else if(cmd == SLOT3)
    index = 3;
  else if(cmd == SLOT4)
    index = 4;
  else if(cmd == SLOT5)
    index = 5;
  else if(cmd == SLOT6)
    index = 6;
  else if(cmd == SLOT7)
    index = 7;
  else if(cmd == SLOT8)
    index = 8;
  else if(cmd == SLOT9)
    index = 9;
  else 
    return 1;

  if(inventory[index] == nullptr) {
    return 1;
  }

  /* Success */
  item *temp = equip(inventory[index]);
  inventory[index] = temp;
  d->display->displayInventory(WEAR_MODE);

  return 1; 
}

inline int pc::takeOffCommand(int cmd) {
  int index = INT_MAX;

  if(!getInventorySlot(index)) {
    d->disp_msg = "Inventory is full!";
    return 0;
  }

  if(cmd == TAKEOFF_SCREEN || cmd == CONTROL_MODE) {
    return 0;
  }
  else if(cmd == EQUIPA && equipment.weapon != nullptr) {
    inventory[index] = equipment.weapon;
    equipment.weapon = nullptr;
  }
  else if(cmd == EQUIPB && equipment.offhand!= nullptr) {
    inventory[index] = equipment.offhand;
    equipment.offhand = nullptr;
  }
  else if(cmd == EQUIPC && equipment.ranged != nullptr) {
    inventory[index] = equipment.ranged;
    equipment.ranged = nullptr;
  }
  else if(cmd == EQUIPD && equipment.armor != nullptr) {
    inventory[index] = equipment.armor;
    equipment.armor = nullptr;
  }
  else if(cmd == EQUIPE && equipment.helmet != nullptr) {
    inventory[index] = equipment.helmet;
    equipment.helmet = nullptr;
  }
  else if(cmd == EQUIPF && equipment.cloak != nullptr) {
    inventory[index] = equipment.cloak;
    equipment.cloak = nullptr;
  }
  else if(cmd == EQUIPG && equipment.gloves != nullptr) {
    inventory[index] = equipment.gloves;
    equipment.gloves = nullptr;
  }
  else if(cmd == EQUIPH && equipment.boots != nullptr) {
    inventory[index] = equipment.boots;
    equipment.boots = nullptr;
  }
  else if(cmd == EQUIPI && equipment.amulet != nullptr) {
    inventory[index] = equipment.amulet;
    equipment.amulet = nullptr;
  }
  else if(cmd == EQUIPJ && equipment.light != nullptr) {
    inventory[index] = equipment.light;
    lightRadius -= equipment.light->getAttribute(); 
    equipment.light = nullptr;
  }
  else if(cmd == EQUIPK && equipment.ring_one != nullptr) {
    inventory[index] = equipment.ring_one;
    equipment.ring_one = nullptr;
  }
  else if(cmd == EQUIPL && equipment.ring_two != nullptr) {
    inventory[index] = equipment.ring_two;
    equipment.ring_two = nullptr;
  }
  else if(cmd == EQUIPM && equipment.wand != nullptr) {
    inventory[index] = equipment.wand;
    mana -= equipment.wand->getAttribute();
    maxMana -= equipment.wand->getAttribute();
    intellect -= equipment.wand->getAttribute();
    equipment.wand = nullptr;
  }
  else if(cmd == EQUIPN && equipment.ammunition != nullptr) {
    inventory[index] = equipment.ammunition;
    equipment.ammunition = nullptr;
  }
  else  {
    return 1;
  }

  /* Success */
  hp -= inventory[index]->getDefense();
  maxhp -= inventory[index]->getDefense();
  hit -= inventory[index]->getHit();
  speed -= inventory[index]->getSpeed();
  dodge -= inventory[index]->getDodge();
  weight -= inventory[index]->getWeight();

  d->disp_msg = "Took off ";
  d->disp_msg.append(inventory[index]->getName());

  d->display->displayInventory(TAKEOFF_MODE);
  return 1; 
}

inline int pc::dropCommand(int cmd) {
  int index = INT_MAX;

  /* Cell is not open */
  if(d->item_grid[y][x] != nullptr) {
    d->disp_msg = "Space is occupied";
    return 0;
  }

  else if(cmd == DROP_SCREEN || cmd == CONTROL_MODE)
    return 0;
  else if(cmd == SLOT0)
    index = 0;
  else if(cmd == SLOT1)
    index = 1;
  else if(cmd == SLOT2)
    index = 2;
  else if(cmd == SLOT3)
    index = 3;
  else if(cmd == SLOT4)
    index = 4;
  else if(cmd == SLOT5)
    index = 5;
  else if(cmd == SLOT6)
    index = 6;
  else if(cmd == SLOT7)
    index = 7;
  else if(cmd == SLOT8)
    index = 8;
  else if(cmd == SLOT9)
    index = 9;
  else 
    return 1;

  if(inventory[index] == nullptr) {
    return 1;
  }

  d->item_grid[y][x] = inventory[index]; 
  inventory[index] = nullptr;

  return 0;
}

inline int pc::expungeCommand(int cmd) {
  int index = INT_MAX;

  if(cmd == EXPUNGE_SCREEN || cmd == CONTROL_MODE)
    return 0;
  else if(cmd == SLOT0)
    index = 0;
  else if(cmd == SLOT1)
    index = 1;
  else if(cmd == SLOT2)
    index = 2;
  else if(cmd == SLOT3)
    index = 3;
  else if(cmd == SLOT4)
    index = 4;
  else if(cmd == SLOT5)
    index = 5;
  else if(cmd == SLOT6)
    index = 6;
  else if(cmd == SLOT7)
    index = 7;
  else if(cmd == SLOT8)
    index = 8;
  else if(cmd == SLOT9)
    index = 9;
  else 
    return 1;

  if(inventory[index] == nullptr) {
    return 1;
  }
  
  delete inventory[index];
  inventory[index] = nullptr;

  d->display->displayInventory(EXPUNGE_MODE);

  return 1;  
}

inline int pc::inspectCommand(int cmd) {
  int index = INT_MAX;

  if(cmd == INSPECT_SCREEN || cmd == CONTROL_MODE)
    return 0;
  else if(cmd == SLOT0)
    index = 0;
  else if(cmd == SLOT1)
    index = 1;
  else if(cmd == SLOT2)
    index = 2;
  else if(cmd == SLOT3)
    index = 3;
  else if(cmd == SLOT4)
    index = 4;
  else if(cmd == SLOT5)
    index = 5;
  else if(cmd == SLOT6)
    index = 6;
  else if(cmd == SLOT7)
    index = 7;
  else if(cmd == SLOT8)
    index = 8;
  else if(cmd == SLOT9)
    index = 9;
  else 
    return 1;

  if(inventory[index] == nullptr) {
    return 1;
  }

  d->display->displayItemDescription(inventory[index]);
  getch();

  return 0;
}

int pc::rangedAttack(projectile *p, int cmd) {
  int dir = INT_MAX;

  if(cmd == QUIT_GAME)
    return end_game(d, BOTH_MODE);
  else if(cmd == RANGED_SCREEN || cmd == CONTROL_MODE) {
    d->disp_msg = d->level_msg;
    return 0;
  }
  else if(cmd == MV_UP_LEFT_1 || cmd == MV_UP_LEFT_2)
    dir = NW;
  else if(cmd == MV_UP_RIGHT_1 || cmd == MV_UP_RIGHT_2)
    dir = NE;
  else if(cmd == MV_UP_1 || cmd == MV_UP_2)
    dir = N;
  else if(cmd == MV_LEFT_1 || cmd == MV_LEFT_2)
    dir = W;
  else if(cmd == MV_RIGHT_1 || cmd == MV_RIGHT_2)
    dir = E;
  else if(cmd == MV_DWN_LEFT_1 || cmd == MV_DWN_LEFT_2)
    dir = SW;
  else if(cmd == MV_DWN_RIGHT_1 || cmd == MV_DWN_RIGHT_2)
    dir = SE;
  else if(cmd == MV_DWN_1 || cmd == MV_DWN_2)
    dir = S;
  else 
    return 1;

  p->setDirection(dir);
  d->display->displayProjectile(p);

  return 0;
}

inline int pc::rangedCommand(int cmd) {
  int dmg = 0;
  int color = 0;
  projectile *p;

  if(cmd == RANGED_SCREEN || cmd == CONTROL_MODE) {
    return 0;
  }

  /* Ranged weapon */
  else if(cmd == SLOT1) {
    if(equipment.ranged == nullptr) {
      d->disp_msg = "You don't have a ranged weapon!"; 
      return 0;
    }
    else if(equipment.ammunition == nullptr) {
      d->disp_msg = "Out of ammo"; 
      return 0;
    }
    else if(equipment.ammunition->getAttribute() < 1) {
      d->disp_msg = "Out of ammo"; 
      return 0;
    }

    dmg = equipment.ranged->getAttribute();
    equipment.ammunition->setAttribute(equipment.ammunition->getAttribute() - 1);
    color = equipment.ammunition->getColor();

    p = new projectile(d, x, y); 
    p->setDamage(dmg);
    p->setColor(color);

    d->disp_msg = "You loose an arrow";
  }

  /* Poison Ball */
  else if(cmd == SLOT2) {
    if(mana < 20) {
      d->disp_msg = "Not enough mana";
      return 0;
    }

    if(mana > 0)
      mana -= 20;
    else 
      mana = 0;

    dmg = 20;
    dmg += (intellect / 2);
    color = COLOR_GREEN;

    effect *e = new effect(d);
    e->setColor(color);
    e->setDamage(10);
    e->setName("poison gas");

    p = new projectile(d, x, y);
    p->setEffect(e);
    p->setDamage(dmg);
    p->setColor(color);
  
    d->disp_msg = "You cast a poison ball";
  }

  /* Fireball */
  else if(cmd == SLOT3 && fireball_is_learned) {
    if(mana < 20) {
      d->disp_msg = "Not enough mana";
      return 0;
    }

    if(mana > 0)
      mana -= 20;
    else 
      mana = 0;

    dmg = 10;
    dmg += (intellect / 2);
    color = COLOR_RED;

    effect *e = new effect(d);
    e->setColor(color);
    e->setDamage(20);
    e->setName("explosion");

    p = new projectile(d, x, y);
    p->setEffect(e);
    p->setDamage(dmg);
    p->setColor(color);
  
    d->disp_msg = "You cast a fireball";
  }

  /* Heal */
  else if(cmd == SLOT4 && heal_is_learned) {
    if(mana < 30) {
      d->disp_msg = "Not enough mana";
      return 0;
    } 

    if(hp == maxhp)
      return 1;

    if(mana > 0)
      mana -= 30;
    else 
      mana = 0;

    int htemp = 20 + (intellect / 2);
    hp += htemp;

    if(hp > maxhp)
      hp = maxhp;

    d->disp_msg = "Healed yourself for ";
    d->disp_msg.append(std::to_string(htemp));
    return 0;
  }
  
  else {
    return 1;
  }

  
  d->display->displayRangedAttack(RANGED_ATTACK_MODE);
  while(rangedAttack(p, getch())) {}

  return 0;
}

int pc::move() {
  findRandNeighbor();
  getch();
  x = next[dim_x];
  y = next[dim_y];
  return 0;
}

int pc::move(int move) {
  /*                 * 
   *                 *
   * Display screens *
   *                 *
   *                 */

  if(move == INVENTORY_SCREEN) {
    d->display->displayInventory(INVENTORY_MODE);
    while(command(getch(), INVENTORY_MODE)) {}
    return 1;
  }

  else if(move == EQUIPMENT_SCREEN) {
    d->display->displayInventory(EQUIPMENT_MODE);
    while(command(getch(), EQUIPMENT_MODE)) {}
    return 1;
  }

  else if(move == WEAR_SCREEN) {
    d->display->displayInventory(WEAR_MODE);
    while(command(getch(), WEAR_MODE)) {}
    return 1;
  }

  else if(move == TAKEOFF_SCREEN) {
    d->display->displayInventory(TAKEOFF_MODE);
    while(command(getch(), TAKEOFF_MODE)) {}
    return 1;
  }

  else if(move == DROP_SCREEN) {
    d->display->displayInventory(DROP_MODE);
    while(command(getch(), DROP_MODE)) {}
    return 1;
  }

  else if(move == EXPUNGE_SCREEN) {
    d->display->displayInventory(EXPUNGE_MODE);
    while(command(getch(), EXPUNGE_MODE)) {}
    return 1;
  }
  
  else if(move == INSPECT_SCREEN) {
    d->display->displayInventory(INSPECT_MODE);
    while(command(getch(), INSPECT_MODE)) {}
    return 1;
  }

  else if(move == STATS_SCREEN) {
    d->display->displayCharacterStats();
    while(statsCommand(getch())) {}
    return 1;
  }

  else if(move == RANGED_SCREEN) {
    d->display->displayRangedAttack(RANGED_SELECT_MODE);
    while(rangedCommand(getch())) {}
    return 1;
  }

  /*                 * 
   *                 *
   *      Moves      *
   *                 *
   *                 */

  d->disp_msg = d->level_msg;
  if((move == MV_UP_LEFT_1 || move == MV_UP_LEFT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x - 1;
    next[dim_y] = y - 1;
  }

  else if(move == MV_UP_1 || move == MV_UP_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = x;
      next[dim_y] = y - 1;
    }

    else {
      d->lcoords[dim_y] = (d->lcoords[dim_y] >= 19 ?
          d->lcoords[dim_y] - 10 : 9);
      return 1;
    }
  }

  else if((move == MV_UP_RIGHT_1 || move == MV_UP_RIGHT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x + 1;
    next[dim_y] = y - 1;
  }

  else if(move == MV_RIGHT_1 || move == MV_RIGHT_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = x + 1;
      next[dim_y] = y;
    }

    else {
      d->lcoords[dim_x] = (d->lcoords[dim_x] <= 99 ?
          d->lcoords[dim_x] + 20: 119);
      return 1;
    }
  }

  else if((move == MV_DWN_RIGHT_1 || move == MV_DWN_RIGHT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x + 1;
    next[dim_y] = y + 1;
  }

  else if(move == MV_DWN_1 || move == MV_DWN_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = x;
      next[dim_y] = y + 1;
    }

    else {
      d->lcoords[dim_y] = (d->lcoords[dim_y] <= 83 ?
          d->lcoords[dim_y] + 10: 93);
      return 1;
    }
  }

  else if((move == MV_DWN_LEFT_1 || move == MV_DWN_LEFT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x - 1;
    next[dim_y] = y + 1;
  }

  else if(move == MV_LEFT_1 || move == MV_LEFT_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = x - 1;
      next[dim_y] = y;
    }

    else {
      d->lcoords[dim_x] = (d->lcoords[dim_x] >= 59 ?
          d->lcoords[dim_x] - 20: 39);
      return 1;
    }
  }

  else if((move == REST_1|| move == REST_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x;
    next[dim_y] = y;
    return 0;
  }

  else if(move == MV_UP_STAIRS) {
    return mv_up_stairs(d);
  }

  else if(move == MV_DWN_STAIRS) {
    return mv_dwn_stairs(d);
  }

  else if(move == LOOK_MODE) {
    if(d->view_mode != LOOK_MODE) {
      d->view_mode = LOOK_MODE;
      if(d->lcoords[dim_y] < 9) d->lcoords[dim_y] = 9;
      else if(d->lcoords[dim_y] > 93) d->lcoords[dim_y] = 93;
      if(d->lcoords[dim_x] > 119) d->lcoords[dim_x] = 119;
      else if(d->lcoords[dim_x] < 39) d->lcoords[dim_x] = 39;
    }
    return 1;
  }

  else if(move == CONTROL_MODE) {
    if(d->view_mode != CONTROL_MODE) {
      d->view_mode = CONTROL_MODE;
      d->lcoords[dim_x] = x;
      d->lcoords[dim_y] = y;
      d->display->displayMap();
    }
    return 1;
  }

  else if(move == QUIT_GAME) {
    return end_game(d, BOTH_MODE);
  }

  else {
    return 1;
  }

  if(hardnesspair(next) == 0) {
    /* attackPos() will set next to current 
     * position if it is occupied, so this works */
    attackPos();
    prev[dim_x] = x;
    prev[dim_y] = y;
    x = next[dim_x];
    y = next[dim_y];
    pickupItems();
    updateMap();
    return 0;
  }

  else {
    return 1;
  }
}

bool pc::getInventorySlot(int &s) {
  /* Sets s to first open slot 
   * Returns true if slot was found -
   * false otherwise */

  int i;
  
  for(i = 0; i < INVENTORY_SIZE; i++) {
    if(inventory[i] == nullptr) {
      s = i;
      return true;
    }
  }

  return false;
}

int pc::pickupItems() {
  int i = INT_MAX;
  item *t = d->item_grid[next[dim_y]][next[dim_x]];
  if(t != nullptr) {
    if(t->getType() == GOLD_TYPE) {
      d->disp_msg = "Picked up ";
      d->disp_msg.append(std::to_string(t->getValue()));
      d->disp_msg.append(" gold");

      d->item_grid[next[dim_y]][next[dim_x]] = nullptr;
      cash += t->getValue();
      delete t;
    }
    else if(getInventorySlot(i)) {
      inventory[i] = t; 
      d->item_grid[next[dim_y]][next[dim_x]] = nullptr;
      d->disp_msg = "Picked up ";
      d->disp_msg.append(inventory[i]->getName());
      score += 100 * (t->getLevel() + 1);
      t = nullptr;
      return 1;
    }
  }

  t = nullptr;
  return 0;
}

item *pc::equip(item *t) {
  item *ret = nullptr;

  /* Rings are special */
  if(t->getType() == RING_TYPE) {
    if(equipment.ring_one == nullptr) {
      ret = nullptr;
      equipment.ring_one = t;
    }
    else if(equipment.ring_two == nullptr) {
      ret = nullptr;
      equipment.ring_two = t;
    }
    else { 
      ret = equipment.ring_one;
      equipment.ring_one = t;
    }
  }

  /* Food and water */
  else if(t->getType() == FOOD_TYPE) {
    if(hp == maxhp)
      return t; 
    hp += t->getAttribute();

    d->disp_msg = "You gain ";
    d->disp_msg.append(std::to_string(t->getAttribute()));
    d->disp_msg.append(" hp from ");
    d->disp_msg.append(t->getName());

    if(hp > maxhp)
      hp = maxhp;

    delete t;
    t = nullptr;
    ret = nullptr;
    return ret;
  }
  else if(t->getType() == WATER_TYPE) {
    if(mana == maxMana)
      return t; 
    mana += t->getAttribute();

    d->disp_msg = "You gain ";
    d->disp_msg.append(std::to_string(t->getAttribute()));
    d->disp_msg.append(" mana from ");
    d->disp_msg.append(t->getName());


    if(mana > maxMana)
      mana = maxMana;

    delete t;
    t = nullptr;
    ret = nullptr;
    return ret;
  }

  /* Books and Scrolls */
  else if(t->getType() == BOOK_TYPE) {
    d->disp_msg = t->getName();
    d->disp_msg.append(" grants you ");
    d->disp_msg.append(std::to_string(t->getAttribute()));
    d->disp_msg.append(" intellect");

    intellect += t->getAttribute();
    mana += t->getAttribute() / 2;
    maxMana += t->getAttribute() / 2;

    delete t;
    t = nullptr;
    ret = nullptr;
    return ret;
  }
  else if(t->getType() == SCROLL_TYPE) {
    d->disp_msg = t->getName();
    d->disp_msg.append(" grants you ");
    d->disp_msg.append(std::to_string(t->getAttribute()));
    d->disp_msg.append(" intellect");

    intellect += t->getAttribute();
    mana += t->getAttribute() / 2;
    maxMana += t->getAttribute() / 2;

    if(t->getName() == "Scroll of Wisdom") {
      heal_is_learned = true;
      d->disp_msg = "Learned spell \"Heal\"";
    }
    if(t->getName() == "Scroll of Fire") {
      fireball_is_learned = true;
      d->disp_msg = "Learned spell \"Fireball\"";
    }

    delete t;
    t = nullptr;
    ret = nullptr;
    return ret;
  }

  /* Flasks */
  else if(t->getType() == FLASK_TYPE) {
    if(t->getName() == "strangely colored flask") {
      if(rand_range(1, 2) == 2) {
        hp -= t->getAttribute();
        d->disp_msg = t->getName();
        d->disp_msg.append(" did ");
        d->disp_msg.append(std::to_string(t->getAttribute()));
        d->disp_msg.append(" to you");

        if(hp < 0) {
          hp = 0;
          d->disp_msg.append("-Everything goes dark...");
          end_game(d, NPC_MODE);
        }
      } 
      else {
        hp += t->getAttribute();
        maxhp += t->getAttribute();
        d->disp_msg = ("You gained ");
        d->disp_msg.append(std::to_string(t->getAttribute()));
        d->disp_msg.append(" hp from ");
        d->disp_msg.append(t->getName());

        delete t;
        t = nullptr;
      }
    }
    else if(t->getName() == "Elixer of Accuracy") {
      d->disp_msg = ("Gained ");
      d->disp_msg.append(std::to_string(t->getAttribute()));
      d->disp_msg.append(" hit from ");
      d->disp_msg.append(t->getName());

      hit += t->getAttribute();
      delete t;
      t = nullptr;
    }
    else if(t->getName() == "Mysterious potion") {
      d->disp_msg = ("Gained ");
      d->disp_msg.append(std::to_string(t->getAttribute()));
      d->disp_msg.append(" speed from ");
      d->disp_msg.append(t->getName());

      speed += t->getAttribute();
      delete t;
      t = nullptr;
    }
    return ret;
  }

  else if(t->getType() == WEAPON_TYPE) {
    ret = equipment.weapon;
    equipment.weapon = t;
  }
  else if(t->getType() == OFFHAND_TYPE) {
    ret = equipment.offhand;
    equipment.offhand = t;
  }
  else if(t->getType() == RANGED_TYPE) {
    ret = equipment.ranged;
    equipment.ranged = t;
  }
  else if(t->getType() == ARMOR_TYPE) {
    ret = equipment.armor;
    equipment.armor = t;
  }
  else if(t->getType() == HELMET_TYPE) {
    ret = equipment.helmet;
    equipment.helmet = t;
  }
  else if(t->getType() == CLOAK_TYPE) {
    ret = equipment.cloak;
    equipment.cloak = t;
  }
  else if(t->getType() == GLOVE_TYPE) {
    ret = equipment.gloves;
    equipment.gloves = t;
  }
  else if(t->getType() == BOOT_TYPE) {
    ret = equipment.boots;
    equipment.boots = t;
  }
  else if(t->getType() == AMULET_TYPE) {
    ret = equipment.amulet;
    equipment.amulet = t;
  }
  else if(t->getType() == LIGHT_TYPE) {
    ret = equipment.light;
    if(ret) {
      lightRadius -= ret->getAttribute();
    }
    equipment.light = t;
    lightRadius = equipment.light->getAttribute() + DEFAULT_LIGHT_RADIUS;
    updateMap();
  }
  else if(t->getType() == WAND_TYPE) {
    ret = equipment.wand;
    if(ret) {
      mana -= ret->getAttribute();
      maxMana -= ret->getAttribute();
      intellect -= ret->getAttribute();
    }
    equipment.wand = t;
    mana += equipment.wand->getAttribute();
    maxMana += equipment.wand->getAttribute();
    intellect += t->getAttribute();
  }
  else if(t->getType() == AMMUNITION_TYPE) {
    ret = equipment.ammunition;
    equipment.ammunition = t;
  }
  else {
    /* Invalid type */
    return t;
  }

  /* Succeeded */
  if(t) {
    hp += t->getDefense();
    maxhp += t->getDefense();
    hit += t->getHit();
    speed += t->getSpeed();
    dodge += t->getDodge();
    weight += t->getWeight();
  }

  if(ret) {
    hp -= ret->getDefense();
    maxhp -= ret->getDefense();
    hit -= ret->getHit();
    speed -= ret->getSpeed();
    dodge -= ret->getDodge();
    weight -= ret->getWeight();
  }

  d->disp_msg = "Equiped ";
  d->disp_msg.append(t->getName());

  /* return whatever was equipped */
  return ret;
} 

void pc::clearInventories() {
  /*__TODO__*/
  int i;

  for(i = 0; i < INVENTORY_SIZE; i++) {
    if(inventory[i] != nullptr) {
      delete inventory[i];
      inventory[i] = nullptr;
    } 
  }

  if(equipment.weapon != nullptr) 
    delete equipment.weapon; 
  if(equipment.offhand != nullptr) 
    delete equipment.offhand; 
  if(equipment.ranged != nullptr) 
    delete equipment.ranged; 
  if(equipment.armor != nullptr) 
    delete equipment.armor; 
  if(equipment.helmet != nullptr) 
    delete equipment.helmet; 
  if(equipment.cloak != nullptr) 
    delete equipment.cloak; 
  if(equipment.gloves != nullptr) 
    delete equipment.gloves; 
  if(equipment.boots != nullptr) 
    delete equipment.boots; 
  if(equipment.amulet != nullptr) 
    delete equipment.amulet; 
  if(equipment.light != nullptr) 
    delete equipment.light; 
  if(equipment.ring_one != nullptr) 
    delete equipment.ring_one; 
  if(equipment.ring_two != nullptr) 
    delete equipment.ring_two; 

}

inline void pc::updateDesc() {
  if(currentClass == WARRIOR)
    description = "the warrior";
  else if(currentClass == WIZARD)
    description = "the wizard";
  else if(currentClass == ARCHER)
    description = "the archer";
  else if(currentClass == ROGUE)
    description = "the rogue";
  else 
    description = "";
}

void pc::updateMap() {
  int j, i;
  for(j = y - lightRadius; j < (y + lightRadius + 1); j++) {
    for(i = x - lightRadius; i < (x + lightRadius + 1); i++) {
      if(j < DUNGEON_Y && j >= 0 && i < DUNGEON_X && i >= 0) {
        pcmap[j][i] = mapxy(i, j); 
      }
    }
  }  
}

void pc::initMap() {
  for(int j = 0; j < DUNGEON_Y; j++) {
    for(int i = 0; i < DUNGEON_X; i++) {
      pcmap[j][i] = ter_wall; 
    }
  } 
}
