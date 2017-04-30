#include "item.h"
#include "dungeon.h"

item::item(_dungeon *dun) {
  d = dun;
  known = false;
  level = 0;
  
  int tx, ty, tempRoom;
  do {

    tempRoom = rand_range(0, d->num_rooms - 1); 

    tx = rand_range(d->rooms[tempRoom].x + 1,
         d->rooms[tempRoom].length - 2 + d->rooms[tempRoom].x);

    ty = rand_range(d->rooms[tempRoom].y + 1, 
         d->rooms[tempRoom].height - 2 + d->rooms[tempRoom].y);

  } while(!isOpen(tx, ty));

  x = tx;
  y = ty;
}

item::item(item &n) {
  d = n.d;
  name = n.name;
  level = n.level;
  description = n.description;
  type = n.type;
  color = n.color;
  hit = n.hit;
  damage = new dice(); 
  damage->setBase(n.damage->getBase());
  damage->setNumber(n.damage->getNumber());
  damage->setSides(n.damage->getSides());
  defense = n.defense;
  dodge = n.dodge;
  weight = n.weight;
  speed = n.speed;
  attribute = n.attribute;
  value = n.value;
  symbol = n.symbol;
  known = n.known;

  int tx, ty, tempRoom;
  do {

    tempRoom = rand_range(0, d->num_rooms - 1); 

    tx = rand_range(d->rooms[tempRoom].x + 1,
         d->rooms[tempRoom].length - 2 + d->rooms[tempRoom].x);

    ty = rand_range(d->rooms[tempRoom].y + 1, 
         d->rooms[tempRoom].height - 2 + d->rooms[tempRoom].y);

  } while(!isOpen(tx, ty));
  
  x = tx;
  y = ty;
}

string item::toString() {
  string ret;

  ret = name;
  ret.append(" - ");

  if(damage->getBase() != 0 || damage->getNumber() != 0) {
    ret.append(" Dm:");
    ret.append(damage->toString());
  }
  if(hit != 0) {
    ret.append(" H:");
    ret.append(std::to_string(hit));
  }
  if(defense != 0) {
    ret.append(" Df:");
    ret.append(std::to_string(defense));
  }
  if(dodge != 0) {
    ret.append(" Ddg:");
    ret.append(std::to_string(dodge));
  }
  if(weight != 0) {
    ret.append(" W:");
    ret.append(std::to_string(weight));
  }
  if(speed != 0) {
    ret.append(" S:");
    ret.append(std::to_string(speed));
  }
  if(attribute != 0) {
    ret.append(" A:");
    ret.append(std::to_string(attribute));
  }
  if(value != 0) {
    ret.append(" $");
    ret.append(std::to_string(value));
  }

  return ret;
}

int item::isOpen(int xt, int yt) {
  if(mapxy(xt, yt) == ter_floor_room 
        && d->item_grid[yt][xt] == nullptr) {
    return 1;
  }
  return 0;      
}

void item::setSymbol(objectType t) {
  if(t == WEAPON_TYPE)
    symbol = '|';
  else if(t == OFFHAND_TYPE)
      symbol = ')';
  else if(t == RANGED_TYPE)
    symbol = '}';
  else if(t == ARMOR_TYPE)
      symbol = '[';
  else if(t == HELMET_TYPE)
    symbol = ']';
  else if(t == CLOAK_TYPE)
      symbol = '(';
  else if(t == GLOVE_TYPE)
    symbol = '{';
  else if(t == BOOT_TYPE)
      symbol = '\\';
  else if(t == RING_TYPE)
    symbol = '=';
  else if(t == AMULET_TYPE)
      symbol = '\"';
  else if(t == LIGHT_TYPE)
    symbol = '_';
  else if(t == SCROLL_TYPE)
      symbol = '~';
  else if(t == BOOK_TYPE)
    symbol = '?';
  else if(t == FLASK_TYPE)
      symbol = '!';
  else if(t == GOLD_TYPE)
    symbol = '$';
  else if(t == AMMUNITION_TYPE)
      symbol = '/';
  else if(t == FOOD_TYPE || t == WATER_TYPE)
    symbol = ',';
  else if(t == WAND_TYPE)
      symbol = '-';
  else if(t == CONTAINER_TYPE)
    symbol = '%';
  else 
    symbol = '*';
}
