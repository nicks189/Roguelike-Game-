#include "../include/character.h"
#include "../include/dungeon.h"
#include "../include/npc.h"

using std::string;

character::character(_dungeon *dun) : d(dun) {
  x = 0;
  y = 0;
  speed = 0;
  dead = 0;
  next[dim_x] = 0;
  next[dim_y] = 0;
  prev[dim_x] = 0;
  prev[dim_y] = 0;
  traits = 0;
  symbol = '?';
  hp = 0;
  hit = DEFAULT_HIT;
  dodge = 0;
  maxhp = 0;
  level = 0;
  color = COLOR_WHITE;
  mana = DEFAULT_MANA;
  maxMana = DEFAULT_MANA;
  intellect = 0;
  damage = nullptr;
}

int character::findRandNeighbor() {
  int rand_x, rand_y, done;
  done = 0;

  do {
    rand_y = rand_range(y - 1, y + 1);
    rand_x = rand_range(x - 1, x + 1);
    if(rand_x != x || rand_y != y) {
      if(mapxy(rand_x, rand_y) == ter_wall_immutable) {
        // Do nothing
      } else if(traits & NPC_TUNNEL || traits & NPC_PASS_WALL) {
        next[dim_x] = rand_x;
        next[dim_y] = rand_y;
        done = 1;
      } else {
        if(mapxy(rand_x, rand_y) == ter_floor_hall
            || mapxy(rand_x, rand_y) == ter_floor_room) {
          next[dim_x] = rand_x;
          next[dim_y] = rand_y;
          done = 1;
        }
      }
    }
  } while(!done);
  return 1;
}

int character::getRoom() {
  if(mappair(prev) != ter_floor_room || mapxy(x, y) != ter_floor_room) {
    int16_t i;
    for (i = 0; i < d->numrooms; i++) {
      if ((x >= d->rooms[i].x) &&
          (x < (d->rooms[i].x + d->rooms[i].length)) &&
          (y >= d->rooms[i].y) &&
          (y < (d->rooms[i].y + d->rooms[i].height))) {
        return i;
      }
    }
  }
  return BAD_ROOM;
}
