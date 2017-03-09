#include "character.h"
#include "character_util.h"
#include "pathfinding.h"

void place_pc(_dungeon *d, uint8_t loaded, pair_t pc_loc) {
  d->player.speed = 10;

  if(loaded == 1) {
    d->player.x = pc_loc[dim_x];
    d->player.y = pc_loc[dim_y];
    d->player.curroom = 0;
  }
  else {
    d->player.curroom = 0;
    d->player.x = rand_range(d->rooms[0].x + 1,
                               d->rooms[0].length - 1 + d->rooms[0].x);
    d->player.y = rand_range(d->rooms[0].y + 1,
                               d->rooms[0].height - 1 + d->rooms[0].y);
  }
}

int move_pc(_dungeon *d, int32_t move) {
  pair_t next;
  if((move == MV_UP_LEFT_1 || move == MV_UP_LEFT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player.x - 1;
    next[dim_y] = d->player.y - 1;
  }

  else if(move == MV_UP_1 || move == MV_UP_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = d->player.x;
      next[dim_y] = d->player.y - 1;
    }

    else {
      d->lcoords[dim_y] = (d->lcoords[dim_y] >= 19 ?
          d->lcoords[dim_y] - 10 : 9);
      return 1;
    }
  }

  else if((move == MV_UP_RIGHT_1 || move == MV_UP_RIGHT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player.x + 1;
    next[dim_y] = d->player.y - 1;
  }

  else if(move == MV_RIGHT_1 || move == MV_RIGHT_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = d->player.x + 1;
      next[dim_y] = d->player.y;
    }

    else {
      d->lcoords[dim_x] = (d->lcoords[dim_x] <= 99 ?
          d->lcoords[dim_x] + 20: 119);
      return 1;
    }
  }

  else if((move == MV_DWN_RIGHT_1 || move == MV_DWN_RIGHT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player.x + 1;
    next[dim_y] = d->player.y + 1;
  }

  else if(move == MV_DWN_1 || move == MV_DWN_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = d->player.x;
      next[dim_y] = d->player.y + 1;
    }

    else {
      d->lcoords[dim_y] = (d->lcoords[dim_y] <= 83 ?
          d->lcoords[dim_y] + 10: 93);
      return 1;
    }
  }

  else if((move == MV_DWN_LEFT_1 || move == MV_DWN_LEFT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player.x - 1;
    next[dim_y] = d->player.y + 1;
  }

  else if(move == MV_LEFT_1 || move == MV_LEFT_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = d->player.x - 1;
      next[dim_y] = d->player.y;
    }

    else {
      d->lcoords[dim_x] = (d->lcoords[dim_x] >= 59 ?
          d->lcoords[dim_x] - 20: 39);
      return 1;
    }
  }

  else if((move == REST_1|| move == REST_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player.x;
    next[dim_y] = d->player.y;
  }

  else if(move == MV_UP_STAIRS) {
    return mv_up_stairs(d);
  }

  else if(move == MV_DWN_STAIRS) {
    return mv_dwn_stairs(d);
  }

  else if(move == LOOK_MODE) {
    if(d->view_mode != LOOK_MODE) {
      d->view_mode = LOOK_MODE;
      if(d->lcoords[dim_y] < 9) d->lcoords[dim_y] = 9;
      else if(d->lcoords[dim_y] > 93) d->lcoords[dim_y] = 93;
      if(d->lcoords[dim_x] > 119) d->lcoords[dim_x] = 119;
      else if(d->lcoords[dim_x] < 39) d->lcoords[dim_x] = 39;
    }
    return 1;
  }

  else if(move == CONTROL_MODE) {
    if(d->view_mode != CONTROL_MODE) {
      d->view_mode = CONTROL_MODE;
      d->lcoords[dim_x] = d->player.x;
      d->lcoords[dim_y] = d->player.y;
      print(d);
    }
    return 1;
  }

  else if(move == QUIT_GAME) {
    return end_game(d, BOTH_MODE);
  }

  else {
    return 1;
  }

  if(hardnesspair(next) == 0) {
    handle_killing(d, d->player.x, d->player.y, next, PC_MODE);
    d->player.x = next[dim_x];
    d->player.y = next[dim_y];
    _npc m;
    check_cur_room(d, &m, PC_MODE);
    pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
    pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);

   return 0;
  }

  else {
    return 1;
  }

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

      if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

        if(mappair(best) == ter_wall) {
          mappair(best) = ter_floor_hall;
          pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
          pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
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

      if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

        if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

          if(mappair(best) == ter_wall) {
            mappair(best) = ter_floor_hall;
            pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
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

        if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

        if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

        if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

          if(mappair(best) == ter_wall) {
            mappair(best) = ter_floor_hall;
            pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
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

        if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

          if(mappair(best) == ter_wall) {
            mappair(best) = ter_floor_hall;
            pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
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

        if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

        if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

          if(mappair(best) == ter_wall) {
            mappair(best) = ter_floor_hall;
            pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
            pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
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

        if(handle_killing(d, mon->x, mon->y, best, NPC_MODE) == 1) {
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

void init_monsters(_dungeon *d) {
  srand(d->seed);
  uint32_t i, mon_room;
  _npc tmp;

  for(i = 0; i < d->nummon; i++) {
    do {
      tmp.speed = rand_range(5, 20);
      tmp.trait = rand() % 16;
      mon_room = rand_range(1, d->num_rooms - 1);

      tmp.x = rand_range(d->rooms[mon_room].x + 1,
                 d->rooms[mon_room].length - 1 + d->rooms[mon_room].x);
      tmp.y = rand_range(d->rooms[mon_room].y + 1, d->rooms[mon_room].height - 1 + d->rooms[mon_room].y);
      tmp.curroom = mon_room;
      tmp.searching = 0;
      tmp.pc_los = 0;
      tmp.pc_lsp[dim_x] = 0;
      tmp.pc_lsp[dim_y] = 0;
      tmp.prev[dim_x] = 0;
      tmp.prev[dim_y] = 0;
      tmp.search_to[dim_x] = 0;
      tmp.search_to[dim_y] = 0;

    } while(add_mon(d, &tmp));
    d->npc_arr[i] = tmp;
  }
  init_char_array(d);
}
