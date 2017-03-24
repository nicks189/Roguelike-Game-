#ifndef PC_H
#define PC_H

#define DUNGEON_X 160
#define DUNGEON_Y 105

#include <stdint.h>

#include "character.h"
#include "dimensions.h"
#include "terrain.h"

typedef struct dungeon _dungeon;

class pc : public character {
  private:
    void initMap();
  public:
    pc(_dungeon *dun);
    pc(_dungeon *dun, int16_t *loc);
    terrain_t pcmap[DUNGEON_Y][DUNGEON_X];
    void updateMap();
    virtual ~pc() {}
    int move(int);
    int move() { return findRandNeighbor(); }
};

#endif
