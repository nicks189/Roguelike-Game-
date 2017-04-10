#ifndef ITEM_H
#define ITEM_H

#include <string>

#include "dice.h"

using std::string;

typedef struct dungeon _dungeon;

typedef enum object_type {
  INVALID_TYPE,
  WEAPON_TYPE,
  OFFHAND_TYPE,
  RANGED_TYPE,
  ARMOR_TYPE,
  HELMET_TYPE,
  CLOAK_TYPE,
  GLOVE_TYPE,
  BOOT_TYPE,
  RING_TYPE,
  AMULET_TYPE,
  LIGHT_TYPE,
  SCROLL_TYPE,
  BOOK_TYPE,
  FLASK_TYPE,
  GOLD_TYPE,
  AMMUNITION_TYPE,
  FOOD_TYPE,
  WAND_TYPE,
  CONTAINER_TYPE
} objectType;

class item {
  private:
    _dungeon *d;
    int x, y;
    string name;
    string description;
    objectType type;
    int color;
    int hit;
    dice *damage;
    int defense;
    int dodge;
    int weight;
    int speed;
    int attribute;
    int value;
    char symbol;
    bool known;
    int isOpen(int, int);

  public:
    item(_dungeon *d);
    item(item &n);
    ~item() { 
      if(damage) {
        delete damage;
      }
    }
    int getX() { return x; }
    int getY() { return y; }
    string getName() { return name; }
    string getDesc() { return description; }
    objectType getType() { return type; }
    int getColor() { return color; }
    int getHit() { return hit; }
    dice *getDamage() { return damage; }
    int getDefense() { return defense; }
    int getDodge() { return dodge; }
    int getWeight() { return weight; }
    int getSpeed() { return speed; }
    int getAttribute() { return attribute; }
    int getValue() { return value; }
    bool isKnown() { return known; } 
    char getSymbol() { return symbol; }
    string toString();
    void setName(string n) { name = n; }
    void setDesc(string n) { description = n; }
    void setType(objectType n) { type = n; }
    void setColor(int n) { color = n; }
    void setHit(int n) { hit = n; }
    void setDamage(dice *n) { damage = n; }
    void setDefense(int n) { defense = n; }
    void setDodge(int n) { dodge = n; }
    void setWeight(int n) { weight = n; }
    void setSpeed(int n) { speed = n; }
    void setAttribute(int n) { attribute = n; }
    void setValue(int n) { value = n; }
    void setSymbol(char n) { symbol = n; }
    void setSymbol(objectType t);
    void setKnown() { known = true; }
};

#endif
