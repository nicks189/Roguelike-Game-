#ifndef CHARACTER_H
#define CHARACTER_H

#include <stdint.h>
#include <string>

#include "dimensions.h"
#include "dice.h"

#define BAD_ROOM 3063
#define DEFAULT_HIT 70
#define DEFAULT_SPEED 10
#define DEFAULT_MANA 100

typedef struct dungeon _dungeon;

using std::string;

class character {
  protected:
    /* 
     * __TODO__ 
     * Handle multi-color  
     *
     */
    _dungeon *d;
    string name;
    string description;
    dice *damage;
    int x, y;
    pair_t next;
    pair_t prev;
    /*       *
     * Stats *
     *       */
    int hp;
    int maxhp;
    int color;
    int traits;
    int mana;
    int maxMana;
    int intellect;
    int hit;
    int dodge;
    int speed;
    int dead;
    int level;
    char symbol;
    /*                               *
     * Protected member functions    *
     *                               */
    virtual int attackPos() = 0;
    int check_los() { return 0; }
    int findRandNeighbor();

  public:
    character(_dungeon *dun);
    virtual ~character() {
      if(damage) {
        delete damage;
        damage = nullptr;
      }
    }
    /*                     * 
     * Getters and setters * 
     *                     */
    string getDesc() { return description; }
    string getName() { return name; }
    int getColor() { return color; }
    int getHp() { return hp; }
    int getMaxHp() { return maxhp; }
    dice *getDamage() { return damage; }
    char getSymbol() { return symbol; }
    int getX() { return x; }
    int getY() { return y; }
    int getHit() { return hit; }
    int getMana() { return mana; }
    int getMaxMana() { return maxMana; }
    int getIntellect() { return intellect; }
    int getDodge() { return dodge; }
    int getSpeed() { return speed; }
    int getTraits() { return traits; }
    int getLevel() { return level; }
    int8_t isDead() { return dead; }
    void setDesc(string d) { description = d; } 
    void setName(string d) { name = d; } 
    void setColor(int d) { color = d; } 
    void setHp(int d) { hp = d; } 
    void setMaxHp(int d) { maxhp = d; }
    void setDamage(dice *d) { damage = d; }
    void setSymbol(char d) { symbol = d; }
    void setX(int t) { x = t; }
    void setY(int t) { y = t; } 
    void setMana(int m) { mana = m; }
    void setMaxMana(int m) { maxMana = m; }
    void setIntellect(int n) { intellect = n; }
    void setHit(int t) { hit = t; }
    void setDodge(int t) { dodge = t; }
    void setSpeed(int d) { speed = d; }
    void setTraits(int d) { traits = d; }
    void setLevel(int d) { level = d; }
    void kill() { dead = 1; }
    /*                        * 
     * Other member functions *
     *                        */
    virtual int move() = 0;
    int getRoom();
    _dungeon *getDungeon() { return d; }
    int16_t *getNext() { return next; }
    int getPrevX() { return prev[dim_x]; }
    int getPrevY() { return prev[dim_y]; }
};

#endif
