#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <string>

#include "dimensions.h"
#include "effect.h"

#define MAX_TRAVEL_DISTANCE 8

typedef struct dungeon _dungeon;

class projectile {
  private:
    _dungeon *d;
    effect *eff;
    int x, y;
    int travelDistance;
    int direction;
    char symbol;
    int color;
    int damage;
    void increment();

  public:
    projectile(_dungeon *dun, int x, int y) : projectile(dun, x, y, N, 1) {}
    projectile(_dungeon *dun, int x, int y, int dir, int dam);
    projectile(_dungeon *dun, int x, int y, int dir, int dam, int col)
                                     : projectile(dun, x, y, dir, dam) {
      color = col;
    }
    virtual ~projectile() {
      if(eff) {
        delete eff;
        eff = nullptr;
      }
    }
    int getX() { return x; }
    int getY() { return y; }
    char getSymbol() { return symbol; }
    int getDamage() { return damage; }
    int getDirection() { return direction; }
    int getColor() { return color; }
    void setX(int n) { x = n; }
    void setY(int n) { y = n; }
    void setColor(int n) { color = n; }
    void setDamage(int n) { damage = n; }
    void setSymbol(char c) { symbol = c; }
    void setDirection(int n) { direction = n; }
    virtual void setEffect(effect *e) { eff = e; }
    virtual effect *getEffect() { return eff; }
    virtual int move();
};

#endif
