#include "character.h"
#include "dungeon.h"

character::character() {
  x = 0;
  y = 0;
  speed = 0;
  dead = 0;
  next[dim_x] = 0;
  next[dim_y] = 0;
  prev[dim_x] = 0;
  prev[dim_y] = 0;
  trait = 0;
  symbol = 'x';
  d = nullptr;
}

character::character(_dungeon *dun) {
  x = 0;
  y = 0;
  speed = 0;
  dead = 0;
  next[dim_x] = 0;
  next[dim_y] = 0;
  prev[dim_x] = 0;
  prev[dim_y] = 0;
  trait = 0;
  symbol = 'x';
  d = dun;
}

int character::attackPos() {
  if(symbol != '@') {
    if(next[dim_x] == d->player->getX() && next[dim_y] == d->player->getY())  {
      return end_game(d, NPC_MODE);
    }
  }

  if(char_gridpair(next) != nullptr && (next[dim_x] != x
                           || next[dim_y] != y)) {
    character *n = (character*) char_gridpair(next);
    n->kill(); 
    char_gridpair(next) = nullptr;
  }
  return 0;
}

int character::findRandNeighbor() {
  uint8_t rand_x, rand_y, done;       
  done = 0;
  
  do {
    rand_y = rand_range(y - 1, y + 1);
    rand_x = rand_range(x - 1, x + 1);

    if(rand_x != x || rand_y != y) {
      if(trait & NPC_TUNNEL) {
        next[dim_x] = rand_x;
        next[dim_y] = rand_y;
        done = 1;
      }  

      else {
        if(mapxy(rand_x, rand_y) != ter_wall) {
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
  int16_t i;
  for (i = 0; i < d->num_rooms; i++) {
    if ((x >= d->rooms[i].x) &&
        (x < (d->rooms[i].x + d->rooms[i].length)) &&
        (y >= d->rooms[i].y) &&
        (y < (d->rooms[i].y + d->rooms[i].height))) {
      return i;
    }
  }
  return BAD_ROOM;
}
