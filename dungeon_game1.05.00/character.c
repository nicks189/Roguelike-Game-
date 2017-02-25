#include "character.h"
#include "character_util.h"
#include "pathfinding.h"

void place_pc(_dungeon *d, uint8_t loaded, pair_t pc_loc) {
  d->player.speed = 10;

  if(loaded == 1) {
    d->player.x = pc_loc[dim_x];
    d->player.y = pc_loc[dim_y]; 
    d->player.prev = mappair(pc_loc);
    d->player.curroom = 0;
  }
  else {
    d->player.curroom = 0;
    d->player.x = rand_range(d->rooms[0].x + 1, 
                               d->rooms[0].length - 1 + d->rooms[0].x);
    d->player.y = rand_range(d->rooms[0].y + 1, 
                               d->rooms[0].height - 1 + d->rooms[0].y);
    d->player.prev = mapxy(d->player.x, d->player.y);
  }

  mapxy(d->player.x, d->player.y) = character_pc;
  printf("PC is at (y, x): %d, %d\n", d->player.y, d->player.x);
}

/* really bad temp */
void move_pc(_dungeon *d) {
  static pair_t pc_to; 
  static int pc_init = 0;
  pair_t temp;
  int16_t distx1, distx2, disty1, disty2;
   
  distx1 = d->player.x - d->rooms[0].x; 
  distx2 = (d->rooms[0].x + d->rooms[0].length) - d->player.x; 
  disty1 = d->player.y - d->rooms[0].y; 
  disty2 = (d->rooms[0].y + d->rooms[0].height) - d->player.y; 

  if(!pc_init) {
    if(distx1 >= distx2) {
      pc_to[dim_x] = d->rooms[0].x;
      if(disty1 >= disty2) {
        pc_to[dim_y] = d->rooms[0].y;
      }
      else {
        pc_to[dim_y] = d->rooms[0].y + d->rooms[0].height - 1;
      }
    }
    else {
      pc_to[dim_x] = d->rooms[0].x + d->rooms[0].length - 1;
      if(disty2 >= disty1) {
        pc_to[dim_y] = d->rooms[0].y + d->rooms[0].height - 1;
      }
      else {
        pc_to[dim_y] = d->rooms[0].y;
      }
    }

  pc_init = 1;
  }

  if(d->player.x > pc_to[dim_x]) {
    temp[dim_x] = d->player.x - 1;
    temp[dim_y] = d->player.y;
    handle_killing(d, 0, 0, temp);
    mapxy(d->player.x, d->player.y) = d->player.prev;
    d->player.prev = mapxy(d->player.x - 1, d->player.y);
    d->player.x -= 1;
    mapxy(d->player.x, d->player.y) = character_pc;
  }
  
  else if(d->player.x < pc_to[dim_x]) {
    temp[dim_x] = d->player.x + 1;
    temp[dim_y] = d->player.y;
    handle_killing(d, 0, 0, temp);
    mapxy(d->player.x, d->player.y) = d->player.prev;
    d->player.prev = mapxy(d->player.x + 1, d->player.y);
    d->player.x += 1;
    mapxy(d->player.x, d->player.y) = character_pc;
  }

  else if(d->player.y > pc_to[dim_y]) { 
    temp[dim_x] = d->player.x;
    temp[dim_y] = d->player.y - 1;
    handle_killing(d, 0, 0, temp);
    mapxy(d->player.x, d->player.y) = d->player.prev;
    d->player.prev = mapxy(d->player.x, d->player.y - 1);
    d->player.y -= 1;
    mapxy(d->player.x, d->player.y) = character_pc;
  }

  else if(d->player.y < pc_to[dim_y]) {
    temp[dim_x] = d->player.x;
    temp[dim_y] = d->player.y + 1;
    handle_killing(d, 0, 0, temp);
    mapxy(d->player.x, d->player.y) = d->player.prev;
    d->player.prev = mapxy(d->player.x, d->player.y + 1);
    d->player.y += 1;
    mapxy(d->player.x, d->player.y) = character_pc;
  } 

  else {
    pc_init = 0;
  }

  _npc m;
  check_cur_room(d, &m, PC_MODE);
  pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
  pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
}



/* Here there be monsters (as in really crappy code) 
 * You've been warned... */
uint8_t move_npc(_dungeon *d, _npc *mon) {

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

      if(handle_killing(d, mon->x, mon->y, best) == 1) {
        return 1;
      }

      if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
        if(hardnesspair(best) < 85) {
          hardnesspair(best) = 0;
        }     

        else {
          hardnesspair(best) -= 85;
        }
        pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
      }

      if(hardnesspair(best) == 0) {
        mapxy(mon->x, mon->y) = mon->prev;
        
        if(mappair(best) == ter_wall) {
          mon->prev = ter_floor_hall;
          pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
          pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
        }

        else {
          mon->prev = mappair(best);
        }

        mon->x = best[dim_x];
        mon->y = best[dim_y]; 
        mappair(best) = mon->type; 
      } 
    } 
    
    else {
      find_rand_neighbor(d, mon->x, mon->y, NON_TUNNEL_MODE, best);

      if(handle_killing(d, mon->x, mon->y, best) == 1) {
        return 1;
      }

      mapxy(mon->x, mon->y) = mon->prev; 
      mon->prev = mappair(best);
      mon->x = best[dim_x];
      mon->y = best[dim_y]; 
      mappair(best) = mon->type; 
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

        if(handle_killing(d, mon->x, mon->y, best) == 1) {
          return 1;
        }

        if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
          if(hardnesspair(best) < 85) {
            hardnesspair(best) = 0;
          }     

          else {
            hardnesspair(best) -= 85;
          }
          pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
        }

        if(hardnesspair(best) == 0) {
          mapxy(mon->x, mon->y) = mon->prev;
          
          if(mappair(best) == ter_wall) {
            mon->prev = ter_floor_hall;
            pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
          }

          else {
            mon->prev = mappair(best);
          }

          mon->x = best[dim_x];
          mon->y = best[dim_y]; 
          mappair(best) = mon->type; 
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

        mapxy(mon->x, mon->y) = mon->prev; 

        if(handle_killing(d, mon->x, mon->y, best) == 1) {
          return 1;
        }

        mon->prev = mappair(best);
        mon->x = best[dim_x];
        mon->y = best[dim_y]; 
        mappair(best) = mon->type; 
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
          mon->pc_lsp[dim_y] = d->player.y;
          mon->pc_lsp[dim_x] = d->player.x;
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

        mapxy(mon->x, mon->y) = mon->prev; 

        if(handle_killing(d, mon->x, mon->y, best) == 1) {
          return 1;
        }

        check_cur_room(d, mon, NPC_MODE);
        mon->prev = mappair(best);
        mon->x = best[dim_x];
        mon->y = best[dim_y]; 
        mappair(best) = mon->type; 
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
          mon->pc_lsp[dim_y] = d->player.y;
          mon->pc_lsp[dim_x] = d->player.x;
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

        if(handle_killing(d, mon->x, mon->y, best) == 1) {
          return 1;
        }

        if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
          if(hardnesspair(best) < 85) {
            hardnesspair(best) = 0;
          }     

          else {
            hardnesspair(best) -= 85;
          }
          pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
        }

        if(hardnesspair(best) == 0) {
          mapxy(mon->x, mon->y) = mon->prev;
          
          if(mappair(best) == ter_wall) {
            mon->prev = ter_floor_hall;
            pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
          }

          else {
            mon->prev = mappair(best);
          }

          check_cur_room(d, mon, NPC_MODE);
          mon->x = best[dim_x];
          mon->y = best[dim_y]; 
          mappair(best) = mon->type; 
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

        if(d->player.x < mon->x) {
          if(d->player.y < mon->y) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player.y > mon->y) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y + 1;
          }

          else {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y;
          }
        } 

        else if(d->player.x > mon->x) {
          if(d->player.y < mon->y) {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player.y > mon->y) {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y + 1;
          }

          else {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y;
          }
        }

        else if(d->player.y > mon->y) {
          best[dim_x] = mon->x;
          best[dim_y] = mon->y + 1;
        }

        else if(d->player.y < mon->y) {
          best[dim_x] = mon->x;
          best[dim_y] = mon->y - 1;
        }

        if(handle_killing(d, mon->x, mon->y, best) == 1) {
          return 1;
        }

        if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
          if(hardnesspair(best) < 85) {
            hardnesspair(best) = 0;
          }     

          else {
            hardnesspair(best) -= 85;
          }
          pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
        }

        if(hardnesspair(best) == 0) {
          mapxy(mon->x, mon->y) = mon->prev;
          
          if(mappair(best) == ter_wall) {
            mon->prev = ter_floor_hall;
            pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
          }

          else {
            mon->prev = mappair(best);
          }

          mon->x = best[dim_x];
          mon->y = best[dim_y]; 
          mappair(best) = mon->type; 
        }
      }

      /* Non-Smart Telepathic Non-Tunneling monsters */
      else {
        
        if(d->player.x < mon->x && hardnessxy(mon->x - 1, mon->y) == 0) {
          best[dim_x] = mon->x - 1;
          best[dim_y] = mon->y;
        }

        else if(d->player.y < mon->y && hardnessxy(mon->x, mon->y - 1) == 0) {
          best[dim_x] = mon->x;
          best[dim_y] = mon->y - 1;
        }

        else if(d->player.x > mon->x && hardnessxy(mon->x + 1, mon->y) == 0) {
          best[dim_x] = mon->x + 1;
          best[dim_y] = mon->y;
        }

        else if(d->player.y > mon->y && hardnessxy(mon->x, mon->y + 1) == 0) {
          best[dim_x] = mon->x;
          best[dim_y] = mon->y + 1;
        }

        else if(d->player.y < mon->y && hardnessxy(mon->x - 1, mon->y - 1) == 0) {
          best[dim_x] = mon->x - 1;
          best[dim_y] = mon->y - 1;
        }

        else if(d->player.y > mon->y && hardnessxy(mon->x - 1, mon->y + 1) == 0) {
          best[dim_x] = mon->x - 1;
          best[dim_y] = mon->y + 1;
        }

        else if(d->player.y < mon->y && hardnessxy(mon->x + 1, mon->y - 1) == 0) {
          best[dim_x] = mon->x + 1;
          best[dim_y] = mon->y - 1;
        }

        else if(d->player.y > mon->y && hardnessxy(mon->x + 1, mon->y + 1) == 0) {
          best[dim_x] = mon->x + 1;
          best[dim_y] = mon->y + 1;
        }

        else {
          best[dim_y] = mon->y;
          best[dim_x] = mon->x;
        }

        mapxy(mon->x, mon->y) = mon->prev; 

        if(handle_killing(d, mon->x, mon->y, best) == 1) {
         return 1;
        } 

        mon->prev = mappair(best);
        mon->x = best[dim_x];
        mon->y = best[dim_y]; 
        mappair(best) = mon->type; 
      }
    }

    /* Non-Smart Non-Telepathic Monsters */
    else {

      /* Non-Smart Non-Telepathic Tunneling monsters */
      if(mon->trait & NPC_TUNNEL) {
        check_los(d, mon);

        if(mon->pc_los == 1) {
          if(d->player.x < mon->x) {
            if(d->player.y < mon->y) {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y - 1;
            }

            else if(d->player.y > mon->y) {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y + 1;
            }

            else {
              best[dim_x] = mon->x - 1;
              best[dim_y] = mon->y;
            }
          } 

          else if(d->player.x > mon->x) {
            if(d->player.y < mon->y) {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y - 1;
            }

            else if(d->player.y > mon->y) {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y + 1;
            }

            else {
              best[dim_x] = mon->x + 1;
              best[dim_y] = mon->y;
            }
          }

          else if(d->player.y > mon->y) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y + 1;
          }

          else if(d->player.y < mon->y) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y - 1;
          }
        }

        else {
          search_for_pc(d, mon, best);
        }

        if(handle_killing(d, mon->x, mon->y, best) == 1) {
          return 1;
        }

        if(hardnesspair(best) != 0 && hardnesspair(best) != 255) {
          if(hardnesspair(best) < 85) {
            hardnesspair(best) = 0;
          }     

          else {
            hardnesspair(best) -= 85;
          }

          pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
        }

        if(hardnesspair(best) == 0) {
          mapxy(mon->x, mon->y) = mon->prev;
          
          if(mappair(best) == ter_wall) {
            mon->prev = ter_floor_hall;
            pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
          }

          else {
            mon->prev = mappair(best);
          }

          check_cur_room(d, mon, NPC_MODE);
          mon->x = best[dim_x];
          mon->y = best[dim_y]; 
          mappair(best) = mon->type; 
        }
      }

      /* Non-Smart Non-Telepathic Non-Tunneling monsters */
      else {
        check_los(d, mon);

        if(mon->pc_los == 1) {
          if(d->player.x < mon->x && hardnessxy(mon->x - 1, mon->y) == 0) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y;
          }

          else if(d->player.y < mon->y && hardnessxy(mon->x, mon->y - 1) == 0) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player.x > mon->x && hardnessxy(mon->x + 1, mon->y) == 0) {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y;
          }

          else if(d->player.y > mon->y && hardnessxy(mon->x, mon->y + 1) == 0) {
            best[dim_x] = mon->x;
            best[dim_y] = mon->y + 1;
          }

          else if(d->player.y < mon->y && hardnessxy(mon->x - 1, mon->y - 1) == 0) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player.y > mon->y && hardnessxy(mon->x - 1, mon->y + 1) == 0) {
            best[dim_x] = mon->x - 1;
            best[dim_y] = mon->y + 1;
          }

          else if(d->player.y < mon->y && hardnessxy(mon->x + 1, mon->y - 1) == 0) {
            best[dim_x] = mon->x + 1;
            best[dim_y] = mon->y - 1;
          }

          else if(d->player.y > mon->y && hardnessxy(mon->x + 1, mon->y + 1) == 0) {
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

        mapxy(mon->x, mon->y) = mon->prev; 

        if(handle_killing(d, mon->x, mon->y, best) == 1) {
          return 1;
        } 

        check_cur_room(d, mon, NPC_MODE);
        mon->prev = mappair(best);
        mon->x = best[dim_x];
        mon->y = best[dim_y]; 
        mappair(best) = mon->type; 
      }
    }
  }

  

  return 0;

  /* As a side note, I'm fully aware how terrible this is */
}

void init_monsters(_dungeon *d) {
  srand(d->seed);
  uint32_t i, mon_room;
  _npc tmp;

  for(i = 0; i < d->nummon; i++) {
    do {   
      tmp.speed = rand_range(5, 20);
      tmp.trait = rand() % 16; 
      mon_room = rand_range(1, d->num_rooms); 
   
      tmp.x = rand_range(d->rooms[mon_room].x + 1,                          
                 d->rooms[mon_room].length - 1 + d->rooms[mon_room].x);
      tmp.y = rand_range(d->rooms[mon_room].y + 1, 
                 d->rooms[mon_room].height - 1 + d->rooms[mon_room].y);
      tmp.curroom = mon_room;
  
    } while(add_mon(d, &tmp));
    d->npc_arr[i] = tmp;
  }
}
