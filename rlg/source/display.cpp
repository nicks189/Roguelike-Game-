#include <ncurses.h>

#include "../include/display.h"
#include "../include/dungeon.h"
#include "../include/pc.h"

cursesDisplay::cursesDisplay(_dungeon *dun) : abstractDisplay(dun) {
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, true);
  start_color();
  init_pair(COLOR_RED,     COLOR_RED,     COLOR_BLACK);
  init_pair(COLOR_GREEN,   COLOR_GREEN,   COLOR_BLACK);
  init_pair(COLOR_YELLOW,  COLOR_YELLOW,  COLOR_BLACK);
  init_pair(COLOR_BLUE,    COLOR_BLUE,    COLOR_BLACK);
  init_pair(COLOR_CYAN,    COLOR_CYAN,    COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(COLOR_WHITE,   COLOR_WHITE,   COLOR_BLACK);
}

cursesDisplay::~cursesDisplay() {
  endwin();
}

int cursesDisplay::displayMenu() {
  clear();

  mvprintw(0, 37, "Randy");
  mvprintw(1, 36, "v2.00.02");
  mvprintw(2, 34, "(1) New Game");
  mvprintw(3, 33, "(2) Load Game");
  mvprintw(4, 35, "(3) Options");

  return getch();
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
        pcX = px;
        pcY = py;
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
          case ter_border_north:
            mvaddch(py, px, '-' | COLOR_PAIR(COLOR_WHITE));
            break;
          case ter_border_south:
            mvaddch(py, px, '-' | COLOR_PAIR(COLOR_WHITE));
            break;
          case ter_border_west:
            mvaddch(py, px, '|' | COLOR_PAIR(COLOR_WHITE));
            break;
          case ter_border_east:
            mvaddch(py, px, '|' | COLOR_PAIR(COLOR_WHITE));
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

  string msg = d->disp_msg;

  /* Currently just truncating display message, change later */
  if(msg.size() > 80) {
    msg.resize(80);
  }

  /* Setting up output */
  int hptemp = d->player->getHp() >= 0 ? d->player->getHp() : 0;
  string hpmsg = "HP:" + std::to_string(hptemp) + "(" 
        + std::to_string(d->player->getMaxHp()) + ")";
  string cashmsg = "$:" + std::to_string(d->player->getCash());
  string manamsg = "M:" + std::to_string(d->player->getMana()) + "(" 
        + std::to_string(d->player->getMaxMana()) + ")";
  string namemsg = d->player->getName() + " " + d->player->getDesc();
  string pclvlmsg = "Lv:" + std::to_string(d->player->getLevel());
  string scoremsg = "S:" + std::to_string(d->player->getScore());
  string statsmsg = "St:" + std::to_string(d->player->getStrength());
  statsmsg += " Sp:" + std::to_string(d->player->getSpeed());
  statsmsg += " In:" + std::to_string(d->player->getIntellect());
  statsmsg += " Dg:" + std::to_string(d->player->getDodge());
  statsmsg += " Ht:" + std::to_string(d->player->getHit() - 70);
  statsmsg += " Rg:" + std::to_string(d->player->getRangedAP());

  /* Format and display output */
  mvprintw(0, 0, "%s", msg.c_str());
  mvprintw(22, 0, namemsg.c_str());
  mvprintw(22, namemsg.size() + 2, pclvlmsg.c_str());
  mvprintw(22, namemsg.size() + pclvlmsg.size() + 3, statsmsg.c_str());
  mvprintw(22, 80 - (scoremsg.size() + 1), scoremsg.c_str());
  mvprintw(23, 0, "%s%d", "Dlvl:", d->level);
  mvprintw(23, 8, cashmsg.c_str());
  mvprintw(23, cashmsg.size() + 9, hpmsg.c_str());
  mvprintw(23, cashmsg.size() + 10 + hpmsg.size(), manamsg.c_str());

  refresh();
}

void cursesDisplay::displayInventory(displayMode mode) {
  int i;
  string to_print[12];
  clear();

  mvprintw(0, 0, "%s", d->disp_msg.c_str());

  if(mode == INVENTORY_MODE || mode == WEAR_MODE || mode == INSPECT_MODE 
                            || mode == DROP_MODE || mode == EXPUNGE_MODE) {

    if(mode == INVENTORY_MODE)
      mvprintw(1, 0, "--------- Inventory ---------");
    else if(mode == WEAR_MODE)
      mvprintw(1, 0, "----------- Wear/Consume-----------");
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
    mvprintw(14, 0, "(Wand)    %s", (d->player->equipment.wand != nullptr 
                      ? d->player->equipment.wand->toString().c_str() : "-----"));
    mvprintw(15, 0, "(Ammo)    %s", (d->player->equipment.ammunition != nullptr 
                      ? d->player->equipment.ammunition->toString().c_str() : "-----"));
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
                      ? d->player->equipment.boots->toString().c_str() : "Empty"));
    mvprintw(10, 0, "(i) %s", (d->player->equipment.amulet != nullptr 
                      ? d->player->equipment.amulet->toString().c_str() : "Empty"));
    mvprintw(11, 0, "(j) %s", (d->player->equipment.light != nullptr 
                      ? d->player->equipment.light->toString().c_str() : "Empty"));
    mvprintw(12, 0, "(k) %s", (d->player->equipment.ring_one != nullptr 
                      ? d->player->equipment.ring_one->toString().c_str() : "Empty"));
    mvprintw(13, 0, "(l) %s", (d->player->equipment.ring_two != nullptr 
                      ? d->player->equipment.ring_two->toString().c_str() : "Empty"));
    mvprintw(14, 0, "(m) %s", (d->player->equipment.wand != nullptr 
                      ? d->player->equipment.wand->toString().c_str() : "Empty"));
    mvprintw(15, 0, "(n) %s", (d->player->equipment.ammunition != nullptr 
                      ? d->player->equipment.ammunition->toString().c_str() : "Empty"));
  }

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
    if(d->player->equipment.offhand->getDamage()->getBase() != 0 
          || d->player->equipment.offhand->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.offhand->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.ranged != nullptr) {
    if(d->player->equipment.ranged->getDamage()->getBase() != 0 
          || d->player->equipment.ranged->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.ranged->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.armor != nullptr) {
    if(d->player->equipment.armor->getDamage()->getBase() != 0 
          || d->player->equipment.armor->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.armor->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.helmet != nullptr) {
    if(d->player->equipment.helmet->getDamage()->getBase() != 0 
          || d->player->equipment.helmet->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.helmet->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.cloak != nullptr) {
    if(d->player->equipment.cloak->getDamage()->getBase() != 0 
          || d->player->equipment.cloak->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.cloak->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.gloves != nullptr) {
    if(d->player->equipment.gloves->getDamage()->getBase() != 0 
          || d->player->equipment.gloves->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.gloves->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.boots != nullptr) {
    if(d->player->equipment.boots->getDamage()->getBase() != 0 
          || d->player->equipment.boots->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.boots->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.amulet != nullptr) {
    if(d->player->equipment.amulet->getDamage()->getBase() != 0 
          || d->player->equipment.amulet->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.amulet->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.light != nullptr) {
    if(d->player->equipment.light->getDamage()->getBase() != 0 
          || d->player->equipment.light->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.light->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.ring_one != nullptr) {
    if(d->player->equipment.ring_one->getDamage()->getBase() != 0 
          || d->player->equipment.ring_one->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.ring_one->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.ring_two != nullptr) {
    if(d->player->equipment.ring_two->getDamage()->getBase() != 0 
          || d->player->equipment.ring_two->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.ring_two->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.wand != nullptr) {
    if(d->player->equipment.wand->getDamage()->getBase() != 0 
          || d->player->equipment.wand->getDamage()->getNumber() != 0) {

      dmg.append(" ");
      dmg.append(d->player->equipment.wand->getDamage()->toString()); 
    }
  }
  if(d->player->equipment.ammunition != nullptr) {
    if(d->player->equipment.ammunition->getDamage()->getBase() != 0 
          || d->player->equipment.ammunition->getDamage()->getNumber() != 0) {
      dmg.append(" ");
      dmg.append(d->player->equipment.ammunition->getDamage()->toString()); 
    }
  }

  mvprintw(0, 0, d->disp_msg.c_str());
  mvprintw(1, 0, "%s ", d->player->getName().c_str());
  mvprintw(1, d->player->getName().size() + 1, "%s", d->player->getDesc().c_str());
  mvprintw(2, 0, "HP:           - %d(%d)", d->player->getHp(), d->player->getMaxHp());
  mvprintw(3, 0, "Dodge:        - %d", d->player->getDodge());
  mvprintw(4, 0, "Hit:          - %d", d->player->getHit() - 70);
  mvprintw(5, 0, "Intellect:    - %d", d->player->getIntellect());
  mvprintw(6, 0, "Strength:     - %d", d->player->getStrength());
  mvprintw(7, 0, "Speed:        - %d", d->player->getSpeed());
  mvprintw(8, 0, "Weight:       - %d", d->player->getWeight());
  mvprintw(9, 0, "Light radius: - %d", d->player->getLightRadius());
  mvprintw(10, 0, "Damage:       - %s", dmg.c_str());

  refresh();
}

void cursesDisplay::displayItemDescription(item *i) {
  clear();

  mvprintw(0, 0, d->disp_msg.c_str());
  mvprintw(1, 0, "---------- %s ----------", i->getName().c_str());
  mvprintw(2, 0, "%s", i->getDesc().c_str());

  refresh(); 
}

void cursesDisplay::displayNextMessage(string s) {
  /*          *
   * __TODO__ *
   *          *
   *          */
}

void cursesDisplay::displayRangedAttack(displayMode mode) {
  int i, j; 

  for(j = 1; j < 5; j++) {
    for(i = 0; i < 70; i++) {
      mvprintw(j, i, " ");
    }
  }

  if(mode == RANGED_SELECT_MODE) {
    string attack1 = "(1): ";
    string attack2 = "(2): Posion Ball(20)"; 
    string attack3 = "(3): Fireball(20)";
    string attack4 = "(4): Heal(30)";
    string mana = "Mana: ";

    mana.append(std::to_string(d->player->getMana()));

    if(d->player->equipment.ranged != nullptr)
      attack1.append(d->player->equipment.ranged->getName());
    else 
      attack1.append("------");

    mvprintw(1, 0, "Select ranged weapon or spell");
    mvprintw(2, 0, "%s", attack1.c_str());
    mvprintw(2, attack1.size() + 1, "%s", attack2.c_str());
    if(d->player->fireball_is_learned)
      mvprintw(2, attack1.size() + attack2.size() + 2, "%s", attack3.c_str());
    if(d->player->heal_is_learned)
      mvprintw(2, attack1.size() + attack2.size() + attack3.size() + 3, 
                                           "%s", attack4.c_str());
    mvprintw(3, 0, "%s - ", mana.c_str());

    if(d->player->equipment.ammunition != nullptr) {
      mvprintw(3, mana.size() + 1, "%s remaining: %d", d->player->equipment.ammunition->getName().c_str(),
                                         d->player->equipment.ammunition->getAttribute());
    }

    else {
      mvprintw(3, mana.size() + 1, "No ammo"); 
    }
  }

  else if(mode == RANGED_ATTACK_MODE) {
    mvprintw(1, 0, "Select direction to attack");  
  }

  refresh();
}

void cursesDisplay::displayProjectile(projectile *p) {
  /* Very bad code */
  displayMap();
  int x, y;
  int py, px; 

  px = pcX;
  py = pcY;
  
  while(p->move()) {
    x = p->getX();
    y = p->getY();

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
      case ter_border_north:
        mvaddch(py, px, '-' | COLOR_PAIR(COLOR_WHITE));
        break;
      case ter_border_south:
        mvaddch(py, px, '-' | COLOR_PAIR(COLOR_WHITE));
        break;
      case ter_border_west:
        mvaddch(py, px, '|' | COLOR_PAIR(COLOR_WHITE));
        break;
      case ter_border_east:
        mvaddch(py, px, '|' | COLOR_PAIR(COLOR_WHITE));
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

    attron(COLOR_PAIR(COLOR_YELLOW));
    mvprintw(pcY, pcX, "@");
    attroff(COLOR_PAIR(COLOR_YELLOW));

    switch(p->getDirection()) {
      case NW:
        py--;
        px--;
        break;
      case N:
        py--;
        break;
      case NE:
        px++;
        py--;
        break;
      case E:
        px++;
        break;
      case SE:
        px++;
        py++;
        break;
      case S:
        py++;
        break;
      case SW:
        px--;
        py++;
        break;
      case W:
        px--;
        break;
    }

    if(inLosRange(x, y)) {
      attron(COLOR_PAIR(p->getColor()));
      mvprintw(py, px, "%c", p->getSymbol());   
      attroff(COLOR_PAIR(p->getColor()));
    }

    refresh();
    usleep(40000);
  }

  x = p->getX();
  y = p->getY();

  switch(d->nofog ? mapxy(x, y) : d->player->pcmap[y][x]) {
    case ter_wall:
    case ter_wall_immutable:
      mvaddch(py, px, ' ' | COLOR_PAIR(COLOR_WHITE));
      break;
    case ter_floor:
    case ter_floor_room:
      mvaddch(py, px, '.' | COLOR_PAIR(COLOR_WHITE));
      break;
    case ter_border_north:
      mvaddch(py, px, '-' | COLOR_PAIR(COLOR_WHITE));
      break;
    case ter_border_south:
      mvaddch(py, px, '-' | COLOR_PAIR(COLOR_WHITE));
      break;
    case ter_border_west:
      mvaddch(py, px, '|' | COLOR_PAIR(COLOR_WHITE));
      break;
    case ter_border_east:
      mvaddch(py, px, '|' | COLOR_PAIR(COLOR_WHITE));
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

  attron(COLOR_PAIR(COLOR_YELLOW));
  mvprintw(pcY, pcX, "@");
  attroff(COLOR_PAIR(COLOR_YELLOW));

  switch(p->getDirection()) {
    case NW:
      py--;
      px--;
      break;
    case N:
      py--;
      break;
    case NE:
      px++;
      py--;
      break;
    case E:
      px++;
      break;
    case SE:
      px++;
      py++;
      break;
    case S:
      py++;
      break;
    case SW:
      px--;
      py++;
      break;
    case W:
      px--;
      break;
  }

  effect *e;

  if((e = p->getEffect())) {
    displayEffect(e, px, py);
  }
   
  delete p;
}

void cursesDisplay::displayEffect(effect *e, int f, int s) {
  char **ra = e->getRenderArray();
  int x_l, x_h, y_l, y_h, x, y;

  int px = f - 2 ;
  int py = s - 1;

  x_h = 5;
  x_l = 0;
  y_h = 3;
  y_l = 0;
  
  attron(COLOR_PAIR(e->getColor()));

  for(y = y_l; y < y_h; y++) {
    for(x = x_l; x < x_h; x++) {
      if(ra[y][x] != ' ') {
        mvprintw(py, px, "%c", ra[y][x]);
      }
      px++;
    }
    py++;
    px = f - 2;
  }

  attroff(COLOR_PAIR(e->getColor()));

  refresh();

  /* Could be adjusted */
  usleep(99990);
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
  else if(mode == ERROR_MODE) {
    clear();
    mvprintw(11, 29, "ERROR - NO SUCH FILE");
    mvprintw(12, 19, "(Make sure ~/.rlg327/ folder is there)");
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
