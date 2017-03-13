#include "character.h"
#include "character_util.h"
#include "pathfinding.h"
#include "npc.h"
#include "pc.h"

inline uint8_t attack_pos(_dungeon *d, uint8_t x_i, 
    uint8_t y_i, int16_t *temp, uint8_t mode) {
  if(mode == NPC_MODE) {
    if(temp[dim_x] == d->player->x && temp[dim_y] == d->player->y)  {
      mappair(temp) = endgame_flag; 
      return 1;
    }
    
    else if(char_gridpair(temp) && (temp[dim_x] != x_i 
          || temp[dim_y] != y_i)) {
      char_gridpair(temp)->x = char_gridpair(temp)->prev[dim_x]; 
      char_gridpair(temp)->y = char_gridpair(temp)->prev[dim_y];
      char_gridxy(char_gridpair(temp)->x, char_gridpair(temp)->y) = char_gridpair(temp);
      char_gridpair(temp) = NULL;
    }
  }

  else {
    if(char_gridpair(temp) != NULL && (temp[dim_x] != x_i 
          || temp[dim_y] != y_i)) {
      char_gridpair(temp)->dead = 1; 
      char_gridpair(temp) = NULL;
    }
  }

  return 0;
} 

/* looks through the room array for matching position if we are 
 * currently in a room and haven't already determined what room we're in; 
 * also only checking for non-telepathic monsters */
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
