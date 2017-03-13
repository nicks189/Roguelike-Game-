#include "npc.h"
#include "pathfinding.h"
#include "character.h"
#include "character_util.h"
#include "pc.h"

_npc *npc_init(_dungeon *d) {
  srand(d->seed);
  _npc *tmp;
  if((tmp = malloc(sizeof(_npc)))) {
    memset(tmp, 0, sizeof(_npc));
    do {
      tmp->speed = rand_range(5, 20);
      tmp->trait = rand() % 16;
      tmp->curroom = rand_range(1, d->num_rooms - 1);
      tmp->x = rand_range(d->rooms[tmp->curroom].x + 1,
         d->rooms[tmp->curroom].length - 1 + d->rooms[tmp->curroom].x);
      tmp->y = rand_range(d->rooms[tmp->curroom].y + 1, 
         d->rooms[tmp->curroom].height - 1 + d->rooms[tmp->curroom].y);
    } while(add_mon(d, tmp));
  }

  return tmp;
}

void delete_npc(_npc *m) {
  if(m) {
    free(m);
  }
}

void find_rand_neighbor(_dungeon *d, uint8_t x, uint8_t y, 
    uint8_t mode, int16_t *ret) {
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

int search_helper(_dungeon *d, _npc *m, int16_t *p) {
  if(m->trait & NPC_TUNNEL && mappair(p) != ter_wall_immutable) {
    return 0;
  }  

  else {
    if(mappair(p) == ter_floor_room || mappair(p) == ter_floor_hall) {
      return 0;
    }

    else {
      m->search_dir = (m->search_dir < 9 ? m->search_dir + 1 : 0);
      return 1;
    }
  }
}

void search_for_pc(_dungeon *d, _npc *m, int16_t *ret) {
  pair_t p; 

  if(m->search_dir == NINIT) {
    m->search_dir = rand_range(1, 8); 
  }

  do {
    switch(m->search_dir) {
      case NINIT:
      case NW:
        p[dim_y] = m->y - 1;
        p[dim_x] = m->x - 1;
        break;
      case N:
        p[dim_y] = m->y - 1;
        p[dim_x] = m->x;
        break;
      case NE:
        p[dim_x] = m->x + 1;
        p[dim_y] = m->y - 1;
        break;
      case E:
        p[dim_x] = m->x + 1;
        p[dim_y] = m->y;
        break;
      case SE:
        p[dim_x] = m->x + 1;
        p[dim_y] = m->y + 1;
        break;
      case S:
        p[dim_y] = m->y - 1;
        p[dim_x] = m->x;
        break;
      case SW:
        p[dim_x] = m->x - 1;
        p[dim_y] = m->y + 1;
        break;
      case W:
        p[dim_x] = m->x - 1;
        p[dim_y] = m->y;
        break; 
    } 
  } while(search_helper(d, m, p));
 
  ret[dim_x] = p[dim_x];
  ret[dim_y] = p[dim_y];


  //uint8_t x_l, x_h, y_l, y_h, x, y;
  //x_l = mon->x - 1;
  //x_h = mon->x + 1;
  //y_l = mon->y - 1;
  //y_h = mon->y + 1;

  //if(mon->searching) {
  //  if(mon->x == mon->search_to[dim_x] && mon->y == mon->search_to[dim_y]) {
  //    mon->searching = 0;
  //  }

  //  for(x = x_l; x <= x_h; x++) {
  //    for(y = y_l; y <= y_h; y++) {
  //      if(x != mon->x || y != mon->y) {
  //        if(mon->search_map[y][x].cost <= mon->search_map[ret[dim_y]][ret[dim_x]].cost) {
  //          ret[dim_x] = x;
  //          ret[dim_y] = y;
  //        }
  //      }
  //    }
  //  }
  //}

  //if(!mon->searching) {
  //  mon->searching = 1;

  //  if(mon->curroom + 1 < d->num_rooms - 1) {
  //    mon->search_to[dim_x] = d->rooms[mon->curroom + 1].x + 1;
  //    mon->search_to[dim_y] = d->rooms[mon->curroom + 1].y + 1;
  //  }

  //  else {
  //    mon->search_to[dim_x] = d->rooms[0].x + 1;
  //    mon->search_to[dim_y] = d->rooms[0].y + 1;
  //  }

  //  if(mon->trait & NPC_TUNNEL) 
  //    pathfinding(d, mon->search_to[dim_x], mon->search_to[dim_y], mon->search_map, TUNNEL_MODE);
  //  else 
  //    pathfinding(d, mon->search_to[dim_x], mon->search_to[dim_y], mon->search_map, NON_TUNNEL_MODE);
  //}
}

/* Uses Bresenham's Line Algorithm */
void check_los(_dungeon *d, _npc *m) {
  if(m->curroom == d->player->curroom && m->curroom != 255) {
    m->pc_los = 1;
    return;
  }

  else {
    m->pc_los = 0;
  }
  
  pair_t first, second;
  pair_t del, f;
  int32_t a, b, c, i; 

  first[dim_x] = m->x;
  first[dim_y] = m->x;
  second[dim_x] = d->player->x;
  second[dim_y] = d->player->y;

  if((abs(first[dim_x] - second[dim_x]) > VISUAL_RANGE) || 
      (abs(first[dim_y] - second[dim_y]) > VISUAL_RANGE)) {
    m->pc_los = 0;
    return; 
  }
      
  /* Find deltax & delta y */
  if (second[dim_x] > first[dim_x]) {
    del[dim_x] = second[dim_x] - first[dim_x];
    f[dim_x] = 1;
  } 
  
  else {
    del[dim_x] = first[dim_x] - second[dim_x];
    f[dim_x] = -1;
  }

  if (second[dim_y] > first[dim_y]) {
    del[dim_y] = second[dim_y] - first[dim_y];
    f[dim_y] = 1;
  }
 
  else {
    del[dim_y] = first[dim_y] - second[dim_y];
    f[dim_y] = -1;
  }

  if (del[dim_x] > del[dim_y]) {
    a = del[dim_y] + del[dim_y];
    c = a - del[dim_x];
    b = c - del[dim_x];
    for (i = 0; i <= del[dim_x]; i++) {
      if ((mappair(first) < ter_floor) && i && (i != del[dim_x])) {
        return;
      }
      first[dim_x] += f[dim_x];
      if (c < 0) {
        c += a;
      }

      else {
        c += b;
        first[dim_y] += f[dim_y];
      }
    }
    m->pc_los = 1;
    return;
  } 

  else {
    a = del[dim_x] + del[dim_x];
    c = a - del[dim_y];
    b = c - del[dim_y];
    for (i = 0; i <= del[dim_y]; i++) {
      if ((mappair(first) < ter_floor) && i && (i != del[dim_y])) {
        return;
      }
      first[dim_y] += f[dim_y];
      if (c < 0) {
        c += a;
      } 
      
      else {
        c += b;
        first[dim_x] += f[dim_x];
      }
    }
    m->pc_los = 1;
    return;
  }
}

inline uint8_t add_mon(_dungeon *d, _npc *mon) {
  if(mapxy(mon->x, mon->y) == ter_floor_room 
      && char_gridxy(mon->x, mon->y) == NULL) {
    if(mon->trait == 15)
      mon->type = 'f';
    else if(mon->trait == 14)
      mon->type = 'e';
    else if(mon->trait == 13)
      mon->type = 'd';
    else if(mon->trait == 12)
      mon->type = 'c';
    else if(mon->trait == 11)
      mon->type = 'b';
    else if(mon->trait == 10)
      mon->type = 'a';
    else if(mon->trait == 9)
      mon->type = '9';
    else if(mon->trait == 8)
      mon->type = '8';
    else if(mon->trait == 7)
      mon->type = '7';
    else if(mon->trait == 6)
      mon->type = '6';
    else if(mon->trait == 5)
      mon->type = '5'; 
    else if(mon->trait == 4)
      mon->type = '4';
    else if(mon->trait == 3)
      mon->type = '3';
    else if(mon->trait == 2)
      mon->type = '2';
    else if(mon->trait == 1)
      mon->type = '1';
    else if(mon->trait == 0)
      mon->type = '0';   
    return 0;
  }
  return 1;      
}

/* Here there be monsters (as in really crappy code)
 * You've been warned... */
int move_npc(_dungeon *d, _npc *mon) {
  uint8_t x_l, x_h, y_l, y_h, x, y;
  x_l = mon->x - 1;
  x_h = mon->x + 1;
  y_l = mon->y - 1;
  y_h = mon->y + 1;

  pair_t best;
  best[dim_x] = mon->x;
  best[dim_y] = mon->y;

  /* monster has no traits */

  /* check if Erratic */
  if(mon->trait & NPC_ERRATIC && ((rand() % 2) == 1)) {

    if(mon->trait & NPC_TUNNEL) {
      find_rand_neighbor(d, mon->x, mon->y, TUNNEL_MODE, best);

      if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
        return 1;
      }

      if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
        if(hardnesspair(best) < 85) {
          hardnesspair(best) = 0;
        }

        else {
          hardnesspair(best) -= 85;
        }
        pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
      }

      if(hardnesspair(best) == 0) {

        if(mappair(best) == ter_wall) {
          mappair(best) = ter_floor_hall;
          pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
          pathfinding(d, d->player->x, d->player->y, d->non_tunnel_map, NON_TUNNEL_MODE);
        }

        char_gridxy(mon->x, mon->y) = NULL;
        mon->prev[dim_x] = mon->x;
        mon->prev[dim_y] = mon->y;
        mon->x = best[dim_x];
        mon->y = best[dim_y];
        char_gridpair(best) = mon;
      }
    }

    else {
      find_rand_neighbor(d, mon->x, mon->y, NON_TUNNEL_MODE, best);

      if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
        return 1;
      }

      char_gridxy(mon->x, mon->y) = NULL;
      mon->prev[dim_x] = mon->x;
      mon->prev[dim_y] = mon->y;
      mon->x = best[dim_x];
      mon->y = best[dim_y];
      char_gridpair(best) = mon;
    }

    check_cur_room(d, mon, NPC_MODE);

    return 0;

  }

  /* Smart monsters */
  if(mon->trait & NPC_SMART) {

    /* Smart Telepathic monsters */
    if(mon->trait & NPC_TELEPATH) {

      /* Smart Telepathic Tunneling monsters */
      if(mon->trait & NPC_TUNNEL) {

        for(x = x_l; x <= x_h; x++) {
          for(y = y_l; y <= y_h; y++) {
            if(x != mon->x || y != mon->y) {
              if(tunnelxy(x, y).cost <= tunnelpair(best).cost) {
                best[dim_x] = x;
                best[dim_y] = y;
              }
            }
          }
        }

        if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
          return 1;
        }

        if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
          if(hardnesspair(best) < 85) {
            hardnesspair(best) = 0;
          }

          else {
            hardnesspair(best) -= 85;
          }
          pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
        }

        if(hardnesspair(best) == 0) {

          if(mappair(best) == ter_wall) {
            mappair(best) = ter_floor_hall;
            pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player->x, d->player->y, d->non_tunnel_map, NON_TUNNEL_MODE);
          }

          char_gridxy(mon->x, mon->y) = NULL;
          mon->prev[dim_x] = mon->x;
          mon->prev[dim_y] = mon->y;
          mon->x = best[dim_x];
          mon->y = best[dim_y];
          char_gridpair(best) = mon;
        }
      }

      /* Smart Telepathic Non-Tunneling monsters */
      else {
        for(x = x_l; x <= x_h; x++) {
          for(y = y_l; y <= y_h; y++) {
            if(x != mon->x || y != mon->y) {
              if(ntunnelxy(x, y).cost <= ntunnelpair(best).cost) {
                best[dim_x] = x;
                best[dim_y] = y;
              }
            }
          }
        }

        if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
          return 1;
        }

        char_gridxy(mon->x, mon->y) = NULL;
        mon->prev[dim_x] = mon->x;
        mon->prev[dim_y] = mon->y;
        mon->x = best[dim_x];
        mon->y = best[dim_y];
        char_gridpair(best) = mon;
      }
    }

    /* Smart Non-Telepathic monsters */
    else {

      /*Smart Non-Telepathic Non-Tunneling monsters */
      if(!(mon->trait & NPC_TUNNEL)) {
        check_los(d, mon);

        /* If pc is in LoS */
        if(mon->pc_los == 1) {

          for(x = x_l; x <= x_h; x++) {
            for(y = y_l; y <= y_h; y++) {
              if(x != mon->x || y != mon->y) {
                if(ntunnelxy(x, y).cost <= ntunnelpair(best).cost) {
                  best[dim_x] = x;
                  best[dim_y] = y;
                }
              }
            }
          }
          mon->pc_lsp[dim_y] = d->player->y;
          mon->pc_lsp[dim_x] = d->player->x;
        }

        /* If pc is not in LoS and has a LSP */
        else if(!(mon->pc_lsp[dim_y] == 0)) {
          if(mon->pc_lsp[dim_x] < mon->x) {
            if(mon->pc_lsp[dim_y] < mon->y && hardnessxy(mon->x - 1, mon->y - 1) == 0) {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y - 1;
            }

            else if(mon->pc_lsp[dim_y] > mon->y && hardnessxy(mon->x - 1, mon->y + 1) == 0) {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y + 1;
            }

            else if(hardnessxy(mon->x - 1, mon->y) == 0){
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y;
            }
          }

          else if(mon->pc_lsp[dim_x] > mon->x) {
            if(mon->pc_lsp[dim_y] < mon->y && hardnessxy(mon->x + 1, mon->y - 1) == 0) {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y - 1;
            }

            else if(mon->pc_lsp[dim_y] > mon->y && hardnessxy(mon->x + 1, mon->y + 1) == 0) {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y + 1;
            }

            else if(hardnessxy(mon->x + 1, mon->y) == 0){
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y;
            }
          }

          else if(mon->pc_lsp[dim_y] > mon->y && hardnessxy(mon->x, mon->y + 1) == 0) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y + 1;
          }

          else if(mon->pc_lsp[dim_y] < mon->y && hardnessxy(mon->x, mon->y - 1) == 0) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y - 1;
          }

          else {
            best[dim_y] = mon->y;
            best[dim_x] = mon->x;
          }
        }

        else {
          search_for_pc(d, mon, best);
        }

        if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
          return 1;
        }

        check_cur_room(d, mon, NPC_MODE);
        char_gridxy(mon->x, mon->y) = NULL;
        mon->prev[dim_x] = mon->x;
        mon->prev[dim_y] = mon->y;
        mon->x = best[dim_x];
        mon->y = best[dim_y];
        char_gridpair(best) = mon;
      }

      /* Smart Non-Telepathic Tunneling monsters */
      else {
        check_los(d, mon);
        /* if pc is in LoS */
        if(mon->pc_los == 1) {
          for(x = x_l; x <= x_h; x++) {
            for(y = y_l; y <= y_h; y++) {
              if(x != mon->x || y != mon->y) {
                if(tunnelxy(x, y).cost <= tunnelpair(best).cost) {
                  best[dim_x] = x;
                  best[dim_y] = y;
                }
              }
            }
          }
          mon->pc_lsp[dim_y] = d->player->y;
          mon->pc_lsp[dim_x] = d->player->x;
        }

        /* if pc has LSP */
        else if(mon->pc_lsp[dim_y] != 0) {
          if(mon->pc_lsp[dim_x] < mon->x) {
            if(mon->pc_lsp[dim_y] < mon->y) {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y - 1;
            }

            else if(mon->pc_lsp[dim_y] > mon->y) {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y + 1;
            }

            else {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y;
            }
          }

          else if(mon->pc_lsp[dim_x] > mon->x) {
            if(mon->pc_lsp[dim_y] < mon->y) {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y - 1;
            }

            else if(mon->pc_lsp[dim_y] > mon->y) {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y + 1;
            }

            else {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y;
            }
          }

          else if(mon->pc_lsp[dim_y] > mon->y) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y + 1;
          }

          else if(mon->pc_lsp[dim_y] < mon->y) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y - 1;
          }
        }

        else {
          search_for_pc(d, mon, best);
        }

        if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
          return 1;
        }

        if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
          if(hardnesspair(best) < 85) {
            hardnesspair(best) = 0;
          }

          else {
            hardnesspair(best) -= 85;
          }
          pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
        }

        if(hardnesspair(best) == 0) {

          if(mappair(best) == ter_wall) {
            mappair(best) = ter_floor_hall;
            pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player->x, d->player->y, d->non_tunnel_map, NON_TUNNEL_MODE);
          }

          check_cur_room(d, mon, NPC_MODE);
          char_gridxy(mon->x, mon->y) = NULL;
          mon->prev[dim_x] = mon->x;
          mon->prev[dim_y] = mon->y;
          mon->x = best[dim_x];
          mon->y = best[dim_y];
          char_gridpair(best) = mon;
        }
      }
    }
  }

  /* Non Smart monsters */
  else {
    /* Non-Smart Telepathic monsters */
    if(mon->trait & NPC_TELEPATH) {

      /* Non-Smart Telepathic Tunneling monsters */
      if(mon->trait & NPC_TUNNEL) {

        if(d->player->x < mon->x) {
          if(d->player->y < mon->y) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player->y > mon->y) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y + 1;
          }

          else {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y;
          }
        }

        else if(d->player->x > mon->x) {
          if(d->player->y < mon->y) {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player->y > mon->y) {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y + 1;
          }

          else {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y;
          }
        }

        else if(d->player->y > mon->y) {
          best[dim_x] = mon->x;
          best[dim_y] = mon->y + 1;
        }

        else if(d->player->y < mon->y) {
          best[dim_x] = mon->x;
          best[dim_y] = mon->y - 1;
        }

        if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
          return 1;
        }

        if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
          if(hardnesspair(best) < 85) {
            hardnesspair(best) = 0;
          }

          else {
            hardnesspair(best) -= 85;
          }
          pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
        }

        if(hardnesspair(best) == 0) {

          if(mappair(best) == ter_wall) {
            mappair(best) = ter_floor_hall;
            pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player->x, d->player->y, d->non_tunnel_map, NON_TUNNEL_MODE);
          }

          char_gridxy(mon->x, mon->y) = NULL;
          mon->prev[dim_x] = mon->x;
          mon->prev[dim_y] = mon->y;
          mon->x = best[dim_x];
          mon->y = best[dim_y];
          char_gridpair(best) = mon;
        }
      }

      /* Non-Smart Telepathic Non-Tunneling monsters */
      else {

        if(d->player->x < mon->x && hardnessxy(mon->x - 1, mon->y) == 0) {
          best[dim_x] = mon->x - 1;
          best[dim_y] = mon->y;
        }

        else if(d->player->y < mon->y && hardnessxy(mon->x, mon->y - 1) == 0) {
          best[dim_x] = mon->x;
          best[dim_y] = mon->y - 1;
        }

        else if(d->player->x > mon->x && hardnessxy(mon->x + 1, mon->y) == 0) {
          best[dim_x] = mon->x + 1;
          best[dim_y] = mon->y;
        }

        else if(d->player->y > mon->y && hardnessxy(mon->x, mon->y + 1) == 0) {
          best[dim_x] = mon->x;
          best[dim_y] = mon->y + 1;
        }

        else if(d->player->y < mon->y && hardnessxy(mon->x - 1, mon->y - 1) == 0) {
          best[dim_x] = mon->x - 1;
          best[dim_y] = mon->y - 1;
        }

        else if(d->player->y > mon->y && hardnessxy(mon->x - 1, mon->y + 1) == 0) {
          best[dim_x] = mon->x - 1;
          best[dim_y] = mon->y + 1;
        }

        else if(d->player->y < mon->y && hardnessxy(mon->x + 1, mon->y - 1) == 0) {
          best[dim_x] = mon->x + 1;
          best[dim_y] = mon->y - 1;
        }

        else if(d->player->y > mon->y && hardnessxy(mon->x + 1, mon->y + 1) == 0) {
          best[dim_x] = mon->x + 1;
          best[dim_y] = mon->y + 1;
        }

        else {
          best[dim_y] = mon->y;
          best[dim_x] = mon->x;
        }

        if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
         return 1;
        }

        char_gridxy(mon->x, mon->y) = NULL;
        mon->prev[dim_x] = mon->x;
        mon->prev[dim_y] = mon->y;
        mon->x = best[dim_x];
        mon->y = best[dim_y];
        char_gridpair(best) = mon;
      }
    }

    /* Non-Smart Non-Telepathic Monsters */
    else {

      /* Non-Smart Non-Telepathic Tunneling monsters */
      if(mon->trait & NPC_TUNNEL) {
        check_los(d, mon);

        if(mon->pc_los == 1) {
          if(d->player->x < mon->x) {
            if(d->player->y < mon->y) {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y - 1;
            }

            else if(d->player->y > mon->y) {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y + 1;
            }

            else {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y;
            }
          }

          else if(d->player->x > mon->x) {
            if(d->player->y < mon->y) {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y - 1;
            }

            else if(d->player->y > mon->y) {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y + 1;
            }

            else {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y;
            }
          }

          else if(d->player->y > mon->y) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y + 1;
          }

          else if(d->player->y < mon->y) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y - 1;
          }
        }

        else {
          search_for_pc(d, mon, best);
        }

        if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
          return 1;
        }

        if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
          if(hardnesspair(best) < 85) {
            hardnesspair(best) = 0;
          }

          else {
            hardnesspair(best) -= 85;
          }

          pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
        }

        if(hardnesspair(best) == 0) {

          if(mappair(best) == ter_wall) {
            mappair(best) = ter_floor_hall;
            pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player->x, d->player->y, d->non_tunnel_map, NON_TUNNEL_MODE);
          }

          check_cur_room(d, mon, NPC_MODE);
          char_gridxy(mon->x, mon->y) = NULL;
          mon->prev[dim_x] = mon->x;
          mon->prev[dim_y] = mon->y;
          mon->x = best[dim_x];
          mon->y = best[dim_y];
          char_gridpair(best) = mon;
        }
      }

      /* Non-Smart Non-Telepathic Non-Tunneling monsters */
      else {
        check_los(d, mon);

        if(mon->pc_los == 1) {
          if(d->player->x < mon->x && hardnessxy(mon->x - 1, mon->y) == 0) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y;
          }

          else if(d->player->y < mon->y && hardnessxy(mon->x, mon->y - 1) == 0) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player->x > mon->x && hardnessxy(mon->x + 1, mon->y) == 0) {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y;
          }

          else if(d->player->y > mon->y && hardnessxy(mon->x, mon->y + 1) == 0) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y + 1;
          }

          else if(d->player->y < mon->y && hardnessxy(mon->x - 1, mon->y - 1) == 0) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player->y > mon->y && hardnessxy(mon->x - 1, mon->y + 1) == 0) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y + 1;
          }

          else if(d->player->y < mon->y && hardnessxy(mon->x + 1, mon->y - 1) == 0) {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player->y > mon->y && hardnessxy(mon->x + 1, mon->y + 1) == 0) {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y + 1;
          }

          else {
            best[dim_y] = mon->y;
            best[dim_x] = mon->x;
          }

        }

        else {
          search_for_pc(d, mon, best);
        }

        if(attack_pos(d, mon->x, mon->y, best, NPC_MODE) == 1) {
          return 1;
        }

        check_cur_room(d, mon, NPC_MODE);
        char_gridxy(mon->x, mon->y) = NULL;
        mon->prev[dim_x] = mon->x;
        mon->prev[dim_y] = mon->y;
        mon->x = best[dim_x];
        mon->y = best[dim_y];
        char_gridpair(best) = mon;
      }
    }
  }
  return 0;

  /* As a side note, I'm fully aware how terrible this is */
}


