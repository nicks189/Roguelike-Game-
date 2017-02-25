#include "character_util.h"
#include "pathfinding.h"

int32_t event_cmp(const void *key, const void *with) {
  return ((event_t *) key)->time - ((event_t *) with)->time;
}

uint8_t add_mon(_dungeon *d, _npc *mon) {

  if(mapxy(mon->x, mon->y) == ter_floor_room) {
    if(mon->trait == 15)
      mon->type = npc_f;
    else if(mon->trait == 14)
      mon->type = npc_e;
    else if(mon->trait == 13)
      mon->type = npc_d;
    else if(mon->trait == 12)
      mon->type = npc_c;
    else if(mon->trait == 11)
      mon->type = npc_b;
    else if(mon->trait == 10)
      mon->type = npc_a;
    else if(mon->trait == 9)
      mon->type = npc_9;
    else if(mon->trait == 8)
      mon->type = npc_8;
    else if(mon->trait == 7)
      mon->type = npc_7;
    else if(mon->trait == 6)
      mon->type = npc_6;
    else if(mon->trait == 5)
      mon->type = npc_5; 
    else if(mon->trait == 4)
      mon->type = npc_4;
    else if(mon->trait == 3)
      mon->type = npc_3;
    else if(mon->trait == 2)
      mon->type = npc_2;
    else if(mon->trait == 1)
      mon->type = npc_1;
    else if(mon->trait == 0)
      mon->type = npc_0;   

    mon->prev = mapxy(mon->x, mon->y);
    mapxy(mon->x, mon->y) = mon->type;

    return 0;
  }
  return 1;      
}

uint8_t handle_killing(_dungeon *d, uint8_t x_i, uint8_t y_i, int16_t *temp) {
  uint32_t i; 

  if(mappair(temp) == character_pc)  {
    mappair(temp) = endgame_flag; 
    return 1;
  }
  
  if(mappair(temp) != ter_wall && mappair(temp) != ter_wall_immutable && mappair(temp) != ter_floor 
      && mappair(temp) != ter_floor_room && mappair(temp) != ter_floor_hall) {
    for(i = 0; i < d->nummon; i++) {
      if(d->npc_arr[i].x == temp[dim_x] && d->npc_arr[i].y == temp[dim_y]
             && !(x_i == d->npc_arr[i].x && y_i == d->npc_arr[i].y)) {

        d->npc_arr[i].dead = 1;
        mappair(temp) = d->npc_arr[i].prev;
        break; 
      }
    }
  }
  return 0;
} 

void search_for_pc(_dungeon *d, _npc *mon, int16_t *ret) {
  uint8_t x_l, x_h, y_l, y_h, x, y;
  x_l = mon->x - 1;
  x_h = mon->x + 1;
  y_l = mon->y - 1;
  y_h = mon->y + 1;

  if(mon->searching) {
    if(mon->x == mon->search_to[dim_x] && mon->y == mon->search_to[dim_y]) {
      mon->searching = 0;
    }

    for(x = x_l; x <= x_h; x++) {
      for(y = y_l; y <= y_h; y++) {
        if(x != mon->x || y != mon->y) {
          if(mon->search_map[y][x].cost <= mon->search_map[ret[dim_y]][ret[dim_x]].cost) {
            ret[dim_x] = x;
            ret[dim_y] = y;
          }
        }
      }
    }
  }

  if(!mon->searching) {
    mon->searching = 1;

    if(mon->curroom + 1 < d->num_rooms) {
      mon->search_to[dim_x] = d->rooms[mon->curroom + 1].x + 1;
      mon->search_to[dim_y] = d->rooms[mon->curroom + 1].y + 1;
    }

    else {
      mon->search_to[dim_x] = d->rooms[0].x + 1;
      mon->search_to[dim_y] = d->rooms[0].y + 1;
    }

    if(mon->trait & NPC_TUNNEL) 
      pathfinding(d, mon->search_to[dim_x], mon->search_to[dim_y], mon->search_map, TUNNEL_MODE);
    else 
      pathfinding(d, mon->search_to[dim_x], mon->search_to[dim_y], mon->search_map, NON_TUNNEL_MODE);
  }

}

void find_rand_neighbor(_dungeon *d, uint8_t x, uint8_t y, uint8_t mode, int16_t *ret) {
  uint8_t rand_x, rand_y, done;       
  done = 0;
  
  do {
    rand_y = rand_range(y - 1, y + 1);
    rand_x = rand_range(x - 1, x + 1);

    if(rand_x != x || rand_y != y) {
      if(mode == TUNNEL_MODE) {
        ret[dim_x] = rand_x;
        ret[dim_y] = rand_y;
        done = 1;
      }  

      else {
        if(mapxy(rand_x, rand_y) != ter_wall) {
          ret[dim_x] = rand_x;
          ret[dim_y] = rand_y;
          done = 1;
        }
      }
    }
  } while(!done); 
}

void check_cur_room(_dungeon *d, _npc *m, uint8_t mode) {
  uint16_t i;

  if(mapxy(m->x, m->y) != ter_floor_room) {
    m->curroom = 255;
    return;
  }

  for (i = 0; i < d->num_rooms; i++) {
    if ((m->x >= d->rooms[i].x) &&
        (m->x < (d->rooms[i].x + d->rooms[i].length)) &&
        (m->y >= d->rooms[i].y) &&
        (m->y < (d->rooms[i].y + d->rooms[i].height))) {
      if(mode == PC_MODE) d->player.curroom = i;
      else m->curroom = i;
      return;
    }
  } 
}

void check_los(_dungeon *d, _npc *m) {
  uint8_t x, y; 

  if(m->curroom == d->player.curroom) {
    m->pc_los = 1;
    return;
  }
  else {
    m->pc_los = 0;
  }

  if(m->x > d->player.x) { 
    if(m->y < d->player.y) {
      x = m->x - 1;
      y = m->y + 1;  
      /* SW */
      while(hardnessxy(x, y) == 0) {
        if(mapxy(x, y) == character_pc) {
          m->pc_los = 1;
          return;
        }
        x--;
        y++;
      }
    }

    else if(m->y > d->player.y) { 
      x = m->x - 1;
      y = m->y - 1;  
      /* NW */
      while(hardnessxy(x, y) == 0) {
        if(mapxy(x,y ) == character_pc) {
          m->pc_los = 1;
          return;
        }
        x--;
        y--;
      }
    }

    else {
      x = m->x - 1;
      y = m->y;  
      /* W */
      while(hardnessxy(x, y) == 0) {
        if(mapxy(x, y) == character_pc) {
          m->pc_los = 1;
          return;
        }
        x--;
      }
    }
  }

  else if(m->x < d->player.x) {
    if(m->y < d->player.y) {
      x = m->x + 1;
      y = m->y + 1;  
      /* SE */
      while(hardnessxy(x, y) == 0) {
        if(mapxy(x, y) == character_pc) {
          m->pc_los = 1;
          return;
        }
        x++;
        y++;
      }
    }
      
    else if(m->y > d->player.y) {
      x = m->x + 1;
      y = m->y - 1;  
      /* NE */
      while(hardnessxy(x, y) == 0) {
        if(mapxy(x, y) == character_pc) {
          m->pc_los = 1;
          return;
        }
        x++;
        y--;
      }
    }

    else {
      x = m->x + 1;
      y = m->y;  
      /* E */
      while(hardnessxy(x, y) == 0) {
        if(mapxy(x, y) == character_pc) {
          m->pc_los = 1;
          return;
        }
        x++;
      }
    }
  }

  else {
    if(m->y > d->player.y) { 
      x = m->x;
      y = m->y + 1;  
      /* N */
      while(hardnessxy(x, y) == 0) {
        if(mapxy(x, y) == character_pc) {
          m->pc_los = 1;
          return;
        }
        y++;
      }
    }

    else {
      x = m->x;
      y = m->y + 1;  
      /* S */
      while(hardnessxy(x, y) == 0) {
        if(mapxy(x, y) == character_pc) {
          m->pc_los = 1;
          return;
        }
        y++;
      }
    }
  }
}
