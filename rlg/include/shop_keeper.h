#ifndef SHOP_KEEPER_H
#define SHOP_KEEPER_H

#include "dimensions.h"
#include "character.h"
#include "item.h"

#define SHOP_INVENTORY_SIZE 20

typedef struct dungeon _dungeon;

class shop_keeper : public character {
  private:
    int attackPos() { return 0; }

  public:
    item *inventory[SHOP_INVENTORY_SIZE];
    int move() { return findRandNeighbor(); }  
};

#endif
