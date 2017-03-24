#ifndef CHARACTER_H
#define CHARACTER_H

#include <stdint.h>

#include "dimensions.h"

typedef struct dungeon _dungeon;

class character {
  protected:
    int x, y;
    int trait;
    int isPc;
    int speed;
    uint8_t dead;
    pair_t next;
    pair_t prev;
    _dungeon *d;
    int attackPos();
    int check_los() { return 0; }
    int findRandNeighbor();
  public:
    character(); 
    character(_dungeon *dun);
    virtual ~character() {}
    virtual int move() = 0;
    int getX() { return x; }
    int getY() { return y; }
    void setX(int t) { x = t; }
    void setY(int t) { y = t; } 
    void kill() { dead = 1; }
    int getSpeed() { return speed; }
    _dungeon *getDungeon() { return d; }
    int getRoom();
    int8_t isDead() { return dead; }
    int16_t *getNext() { return next; }
    int getTrait() { return trait; }
    int getPrevX() { return prev[dim_x]; }
    int getPrevY() { return prev[dim_y]; }
};

#endif
