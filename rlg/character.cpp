#include "character.h"
//#include "character_util.h"
//#include "pathfinding.h"
#include "dungeon.h"

character::character() {
  x = 0;
  y = 0;
  isPc = 0;
  speed = 0;
  dead = 0;
  next[dim_x] = 0;
  next[dim_y] = 0;
  prev[dim_x] = 0;
  prev[dim_y] = 0;
  trait = 0;
  d = NULL;
}

character::character(_dungeon *dun) {
  x = 0;
  y = 0;
  isPc = 0;
  speed = 0;
  dead = 0;
  next[dim_x] = 0;
  next[dim_y] = 0;
  prev[dim_x] = 0;
  prev[dim_y] = 0;
  trait = 0;
  d = dun;
}

int character::attackPos() {
  if(!isPc) {
    if(next[dim_x] == d->player->getX() && next[dim_y] == d->player->getY())  {
      return end_game(d, NPC_MODE);
    }

    else if(char_gridpair(next) != NULL && (next[dim_x] != x
          || next[dim_y] != y)) {
      //pair_t pt;
      //pt[dim_x] = char_gridpair(next)->getPrevX(); 
      //pt[dim_y] = char_gridpair(next)->getPrevY(); 
      //char_gridpair(next)->setX(pt[dim_x]);
      //char_gridpair(next)->setY(pt[dim_y]);
      //char_gridxy(char_gridpair(next)->getX(), char_gridpair(next)->getY()) = char_gridpair(next);
      npc *n = (npc *) char_gridpair(next);
      n->kill(); 
      char_gridpair(next) = nullptr;
    }
  }

  else {
    if(char_gridpair(next) != NULL && (next[dim_x] != x
          || next[dim_y] != y)) {
      npc *n = (npc *) char_gridpair(next);
      n->kill(); 
      char_gridpair(next) = nullptr;
    }
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

/* looks through the room array for matching position if we are
 * currently in a room and haven't already determined what room we're in;
 * also only checking for non-telepathic monsters
void check_cur_room(_dungeon *d, _npc *m, uint8_t mode) {
  uint16_t i;

  if(mode == PC_MODE) {
    if(mapxy(d->player->x, d->player->y) != ter_floor_room) {
      d->player->curroom = 255;
    }

    else if(d->player->curroom == 255) {
      for (i = 0; i < d->num_rooms; i++) {
        if ((d->player->x >= d->rooms[i].x) &&
            (d->player->x < (d->rooms[i].x + d->rooms[i].length)) &&
            (d->player->y >= d->rooms[i].y) &&
            (d->player->y < (d->rooms[i].y + d->rooms[i].height))) {
          d->player->curroom = i;
          return;
        }
      }
    }
  }

  else if(mode == NPC_MODE && !(m->type & NPC_TELEPATH)) {
    if(mapxy(m->x, m->y) != ter_floor_room) {
      m->curroom = 255;
    }

    else if(m->curroom == 255) {
      for (i = 0; i < d->num_rooms; i++) {
        if ((m->x >= d->rooms[i].x) &&
            (m->x < (d->rooms[i].x + d->rooms[i].length)) &&
            (m->y >= d->rooms[i].y) &&
            (m->y < (d->rooms[i].y + d->rooms[i].height))) {
          m->curroom = i;
          return;
        }
      }
    }
  }
}
*/

int character::getRoom() {
  uint32_t i;
  for (i = 0; i < d->num_rooms; i++) {
    if ((x >= d->rooms[i].x) &&
        (x < (d->rooms[i].x + d->rooms[i].length)) &&
        (y >= d->rooms[i].y) &&
        (y < (d->rooms[i].y + d->rooms[i].height))) {
      return i;
    }
  }
  return 3000;
}
