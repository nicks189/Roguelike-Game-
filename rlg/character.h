#ifndef CHARACTER_H
#define CHARACTER_H

#include <stdint.h>

#include "dimensions.h"

#define BAD_ROOM 3063

/* controls */
#define MV_UP_LEFT_1 067 
#define MV_UP_LEFT_2 0171 
#define MV_UP_1 070
#define MV_UP_2 0153 
#define MV_UP_RIGHT_1 071
#define MV_UP_RIGHT_2 0165 
#define MV_RIGHT_1 066
#define MV_RIGHT_2 0154 
#define MV_DWN_RIGHT_1 063
#define MV_DWN_RIGHT_2 0156 
#define MV_DWN_1 062
#define MV_DWN_2 0152
#define MV_DWN_LEFT_1 061
#define MV_DWN_LEFT_2 0142 
#define MV_LEFT_1 064
#define MV_LEFT_2 0150 
#define REST_1 065
#define REST_2 040 
#define MV_UP_STAIRS 074
#define MV_DWN_STAIRS 076
#define LOOK_MODE 0114
#define CONTROL_MODE 033
#define QUIT_GAME 0121


typedef struct dungeon _dungeon;

class character {
  protected:
    int x, y;
    int trait;
    int *color;
    char **description;
    int speed;
    int dead;
    char symbol;
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
    char getSymbol() { return symbol; }
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
