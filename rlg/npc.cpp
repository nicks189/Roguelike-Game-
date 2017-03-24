#include "npc.h"
#include "pathfinding.h"
#include "character.h"
#include "dungeon.h"

npc::npc(_dungeon *dun) {
  d = dun;
  srand(d->seed++);
  isPc = 0;
  next[dim_x] = 0;
  next[dim_y] = 0;
  dead = 0;
  speed = rand_range(5, 20);
  trait = rand_range(0, 15);
  search_dir = rand_range(0, 7);
  pc_lsp[dim_x] = 0;
  pc_lsp[dim_y] = 0;
  type = 0;
  int tx, ty;
  do {
    int tempRoom = rand_range(1, d->num_rooms - 1); 
    tx = rand_range(d->rooms[tempRoom].x + 1,
         d->rooms[tempRoom].length - 2 + d->rooms[tempRoom].x);
    ty = rand_range(d->rooms[tempRoom].y + 1, 
         d->rooms[tempRoom].height - 2 + d->rooms[tempRoom].y);
  } while(isOpen(tx, ty));
  
  x = tx;
  y = ty;
  prev[dim_x] = x;
  prev[dim_y] = y;
}

int npc::searchHelper(int16_t *p) {
  if(mappair(p) == ter_floor_room || mappair(p) == ter_floor_hall) {
    return 0;
  }

  else {
    search_dir = (search_dir < 7 ? search_dir + 1 : 0);
    return 1;
  }
}

void npc::search() {
  pair_t p; 
  p[dim_x] = 0;
  p[dim_y] = 0;

  do {
    switch(search_dir) {
      case NW:
        p[dim_y] = y - 1;
        p[dim_x] = x - 1;
        break;
      case N:
        p[dim_y] = y - 1;
        p[dim_x] = x;
        break;
      case NE:
        p[dim_x] = x + 1;
        p[dim_y] = y - 1;
        break;
      case E:
        p[dim_x] = x + 1;
        p[dim_y] = y;
        break;
      case SE:
        p[dim_x] = x + 1;
        p[dim_y] = y + 1;
        break;
      case S:
        p[dim_y] = y - 1;
        p[dim_x] = x;
        break;
      case SW:
        p[dim_x] = x - 1;
        p[dim_y] = y + 1;
        break;
      case W:
        p[dim_x] = x - 1;
        p[dim_y] = y;
        break; 
    } 
  } while(searchHelper(p));
 
  next[dim_x] = p[dim_x];
  next[dim_y] = p[dim_y];
}

/* Uses Bresenham's Line Algorithm */
int npc::checkLos() {
  pair_t first, second;
  pair_t del, f;
  int32_t a, b, c, i; 

  first[dim_x] = x;
  first[dim_y] = y;
  second[dim_x] = d->player->getX();
  second[dim_y] = d->player->getY();

  if((abs(first[dim_x] - second[dim_x]) > VISUAL_RANGE) || 
      (abs(first[dim_y] - second[dim_y]) > VISUAL_RANGE)) {
    return 0; 
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
        return 0;
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
    return 1;
  } 

  else {
    a = del[dim_x] + del[dim_x];
    c = a - del[dim_y];
    b = c - del[dim_y];
    for (i = 0; i <= del[dim_y]; i++) {
      if ((mappair(first) < ter_floor) && i && (i != del[dim_y])) {
        return 0;
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
    return 1;
  }
}

inline int npc::isOpen(int xt, int yt) {
  if(mapxy(xt, yt) == ter_floor_room 
      && char_gridxy(xt, yt) == NULL) {
    if(trait == 15)
      type = 'f';
    else if(trait == 14)
      type = 'e';
    else if(trait == 13)
      type = 'd';
    else if(trait == 12)
      type = 'c';
    else if(trait == 11)
      type = 'b';
    else if(trait == 10)
      type = 'a';
    else if(trait == 9)
      type = '9';
    else if(trait == 8)
      type = '8';
    else if(trait == 7)
      type = '7';
    else if(trait == 6)
      type = '6';
    else if(trait == 5)
      type = '5'; 
    else if(trait == 4)
      type = '4';
    else if(trait == 3)
      type = '3';
    else if(trait == 2)
      type = '2';
    else if(trait == 1)
      type = '1';
    else if(trait == 0)
      type = '0';   
    return 0;
  }
  return 1;      
}

/* Here there be monsters (as in really crappy code)
 * You've been warned... */
int npc::move() {
  int x_l, x_h, y_l, y_h;
  int tx, ty;
  x_l = x - 1;
  x_h = x + 1;
  y_l = y - 1;
  y_h = y + 1;
  tx = x;
  ty = y;

  next[dim_x] = x;
  next[dim_y] = y;

  /* monster has no traits */

  /* check if Erratic */
  if(trait & NPC_ERRATIC && ((rand() % 2) == 1)) {

    if(trait & NPC_TUNNEL) {
      findRandNeighbor();
      attackPos();

      if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
        if(hardnesspair(next) < 85) {
          hardnesspair(next) = 0;
        }

        else {
          hardnesspair(next) -= 85;
        }
        pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
      }

      if(hardnesspair(next) == 0) {

        if(mappair(next) == ter_wall) {
          mappair(next) = ter_floor_hall;
          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
          pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
        }

        char_gridxy(x, y) = nullptr;
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
    }

    else {
      findRandNeighbor();
      attackPos();

      char_gridxy(x, y) = nullptr;
      prev[dim_x] = x;
      prev[dim_y] = y;
      x = next[dim_x];
      y = next[dim_y];
      char_gridpair(next) = this;
    }

    //check_cur_room(d, this, NPC_MODE);

    return 0;

  }

  /* Smart monsters */
  if(trait & NPC_SMART) {

    /* Smart Telepathic monsters */
    if(trait & NPC_TELEPATH) {

      /* Smart Telepathic Tunneling monsters */
      if(trait & NPC_TUNNEL) {
        for(tx = x_l; tx <= x_h; tx++) {
          for(ty = y_l; ty <= y_h; ty++) {
            if(tx != x || ty != y) {
              if(tunnelxy(tx, ty).cost <= tunnelpair(next).cost) {
                next[dim_x] = tx;
                next[dim_y] = ty;
              }
            }
          }
        }
        //x = tx;
        //y = ty;

        attackPos(); 

        if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
          if(hardnesspair(next) < 85) {
            hardnesspair(next) = 0;
          }

          else {
            hardnesspair(next) -= 85;
          }
          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
        }

        if(hardnesspair(next) == 0) {

          if(mappair(next) == ter_wall) {
            mappair(next) = ter_floor_hall;
            pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
            pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
          }

          char_gridxy(x, y) = nullptr;
          prev[dim_x] = x;
          prev[dim_y] = y;
          x = next[dim_x];
          y = next[dim_y];
          char_gridpair(next) = this;
        }
      }

      /* Smart Telepathic Non-Tunneling monsters */
      else {
        for(tx = x_l; tx <= x_h; tx++) {
          for(ty = y_l; ty <= y_h; ty++) {
            if(tx != x || ty != y) {
              if(ntunnelxy(tx, ty).cost <= ntunnelpair(next).cost) {
                next[dim_x] = tx;
                next[dim_y] = ty;
              }
            }
          }
        }
        //x = tx;
        //y = ty;
        attackPos();
        char_gridxy(x, y) = nullptr;
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
    }

    /* Smart Non-Telepathic monsters */
    else {

      /*Smart Non-Telepathic Non-Tunneling monsters */
      if(!(trait & NPC_TUNNEL)) {
        /* If pc is in LoS */
        if(checkLos()) {
          for(tx = x_l; tx <= x_h; tx++) {
            for(ty = y_l; ty <= y_h; ty++) {
              if(tx != x || ty != y) {
                if(ntunnelxy(tx, ty).cost <= ntunnelpair(next).cost) {
                  next[dim_x] = tx;
                  next[dim_y] = ty;
                }
              }
            }
          }
          pc_lsp[dim_y] = d->player->getY();
          pc_lsp[dim_x] = d->player->getX();
          //x = tx;
          //y = ty;
        }

        /* If pc is not in LoS and has a LSP */
        else if(!(pc_lsp[dim_y] == 0)) {
          if(pc_lsp[dim_x] < x) {
            if(pc_lsp[dim_y] < y && hardnessxy(x - 1, y - 1) == 0) {
              next[dim_x] = x - 1;
              next[dim_y] = y - 1;
            }

            else if(pc_lsp[dim_y] > y && hardnessxy(x - 1, y + 1) == 0) {
              next[dim_x] = x - 1;
              next[dim_y] = y + 1;
            }

            else if(hardnessxy(x - 1, y) == 0){
              next[dim_x] = x - 1;
              next[dim_y] = y;
            }
          }

          else if(pc_lsp[dim_x] > x) {
            if(pc_lsp[dim_y] < y && hardnessxy(x + 1, y - 1) == 0) {
              next[dim_x] = x + 1;
              next[dim_y] = y - 1;
            }

            else if(pc_lsp[dim_y] > y && hardnessxy(x + 1, y + 1) == 0) {
              next[dim_x] = x + 1;
              next[dim_y] = y + 1;
            }

            else if(hardnessxy(x + 1, y) == 0){
              next[dim_x] = x + 1;
              next[dim_y] = y;
            }
          }

          else if(pc_lsp[dim_y] > y && hardnessxy(x, y + 1) == 0) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          }

          else if(pc_lsp[dim_y] < y && hardnessxy(x, y - 1) == 0) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          }

          else {
            next[dim_y] = y;
            next[dim_x] = x;
          }
        }

        else {
          search();
        }
        attackPos();

        char_gridxy(x, y) = nullptr;
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }

      /* Smart Non-Telepathic Tunneling monsters */
      else {
        /* if pc is in LoS */
        if(checkLos()) {
          for(tx = x_l; tx <= x_h; tx++) {
            for(ty = y_l; ty <= y_h; ty++) {
              if(tx != x || ty != y) {
                if(tunnelxy(tx, ty).cost <= tunnelpair(next).cost) {
                  next[dim_x] = tx;
                  next[dim_y] = ty;
                }
              }
            }
          }
          pc_lsp[dim_y] = d->player->getY();
          pc_lsp[dim_x] = d->player->getX();
          //x = tx;
          //y = ty;
        }

        /* if pc has LSP */
        else if(pc_lsp[dim_y] != 0) {
          if(pc_lsp[dim_x] < x) {
            if(pc_lsp[dim_y] < y) {
              next[dim_x] = x - 1;
              next[dim_y] = y - 1;
            }

            else if(pc_lsp[dim_y] > y) {
              next[dim_x] = x - 1;
              next[dim_y] = y + 1;
            }

            else {
              next[dim_x] = x - 1;
              next[dim_y] = y;
            }
          }

          else if(pc_lsp[dim_x] > x) {
            if(pc_lsp[dim_y] < y) {
              next[dim_x] = x + 1;
              next[dim_y] = y - 1;
            }

            else if(pc_lsp[dim_y] > y) {
              next[dim_x] = x + 1;
              next[dim_y] = y + 1;
            }

            else {
              next[dim_x] = x + 1;
              next[dim_y] = y;
            }
          }

          else if(pc_lsp[dim_y] > y) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          }

          else if(pc_lsp[dim_y] < y) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          }
        }

        else {
          search();
        }
        attackPos();

        if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
          if(hardnesspair(next) < 85) {
            hardnesspair(next) = 0;
          }

          else {
            hardnesspair(next) -= 85;
          }
          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
        }

        if(hardnesspair(next) == 0) {

          if(mappair(next) == ter_wall) {
            mappair(next) = ter_floor_hall;
            pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
            pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
          }

          //check_cur_room(d, mon, NPC_MODE);
          char_gridxy(x, y) = nullptr;
          prev[dim_x] = x;
          prev[dim_y] = y;
          x = next[dim_x];
          y = next[dim_y];
          char_gridpair(next) = this;
        }
      }
    }
  }

  /* Non Smart monsters */
  else {
    /* Non-Smart Telepathic monsters */
    if(trait & NPC_TELEPATH) {

      /* Non-Smart Telepathic Tunneling monsters */
      if(trait & NPC_TUNNEL) {

        if(d->player->getX() < x) {
          if(d->player->getY() < y) {
            next[dim_x] = x - 1;
            next[dim_y] = y - 1;
          }

          else if(d->player->getY() > y) {
            next[dim_x] = x - 1;
            next[dim_y] = y + 1;
          }

          else {
            next[dim_x] = x - 1;
            next[dim_y] = y;
          }
        }

        else if(d->player->getX() > x) {
          if(d->player->getY() < y) {
            next[dim_x] = x + 1;
            next[dim_y] = y - 1;
          }

          else if(d->player->getY() > y) {
            next[dim_x] = x + 1;
            next[dim_y] = y + 1;
          }

          else {
            next[dim_x] = x + 1;
            next[dim_y] = y;
          }
        }

        else if(d->player->getY() > y) {
          next[dim_x] = x;
          next[dim_y] = y + 1;
        }

        else if(d->player->getY() < y) {
          next[dim_x] = x;
          next[dim_y] = y - 1;
        }
        attackPos();

        if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
          if(hardnesspair(next) < 85) {
            hardnesspair(next) = 0;
          }

          else {
            hardnesspair(next) -= 85;
          }
          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
        }

        if(hardnesspair(next) == 0) {

          if(mappair(next) == ter_wall) {
            mappair(next) = ter_floor_hall;
            pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
            pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
          }

          char_gridxy(x, y) = nullptr;
          prev[dim_x] = x;
          prev[dim_y] = y;
          x = next[dim_x];
          y = next[dim_y];
          char_gridpair(next) = this;
        }
      }

      /* Non-Smart Telepathic Non-Tunneling monsters */
      else {

        if(d->player->getX() < x && hardnessxy(x - 1, y) == 0) {
          next[dim_x] = x - 1;
          next[dim_y] = y;
        }

        else if(d->player->getY() < y && hardnessxy(x, y - 1) == 0) {
          next[dim_x] = x;
          next[dim_y] = y - 1;
        }

        else if(d->player->getX() > x && hardnessxy(x + 1, y) == 0) {
          next[dim_x] = x + 1;
          next[dim_y] = y;
        }

        else if(d->player->getY() > y && hardnessxy(x, y + 1) == 0) {
          next[dim_x] = x;
          next[dim_y] = y + 1;
        }

        else if(d->player->getY() < y && hardnessxy(x - 1, y - 1) == 0) {
          next[dim_x] = x - 1;
          next[dim_y] = y - 1;
        }

        else if(d->player->getY() > y && hardnessxy(x - 1, y + 1) == 0) {
          next[dim_x] = x - 1;
          next[dim_y] = y + 1;
        }

        else if(d->player->getY() < y && hardnessxy(x + 1, y - 1) == 0) {
          next[dim_x] = x + 1;
          next[dim_y] = y - 1;
        }

        else if(d->player->getY() > y && hardnessxy(x + 1, y + 1) == 0) {
          next[dim_x] = x + 1;
          next[dim_y] = y + 1;
        }

        else {
          next[dim_y] = y;
          next[dim_x] = x;
        }
        attackPos();

        char_gridxy(x, y) = nullptr;
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
    }

    /* Non-Smart Non-Telepathic Monsters */
    else {

      /* Non-Smart Non-Telepathic Tunneling monsters */
      if(trait & NPC_TUNNEL) {
        if(checkLos()) {
          if(d->player->getX() < x) {
            if(d->player->getY() < y) {
              next[dim_x] = x - 1;
              next[dim_y] = y - 1;
            }

            else if(d->player->getY() > y) {
              next[dim_x] = x - 1;
              next[dim_y] = y + 1;
            }

            else {
              next[dim_x] = x - 1;
              next[dim_y] = y;
            }
          }

          else if(d->player->getX() > x) {
            if(d->player->getY() < y) {
              next[dim_x] = x + 1;
              next[dim_y] = y - 1;
            }

            else if(d->player->getY() > y) {
              next[dim_x] = x + 1;
              next[dim_y] = y + 1;
            }

            else {
              next[dim_x] = x + 1;
              next[dim_y] = y;
            }
          }

          else if(d->player->getY() > y) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          }

          else if(d->player->getY() < y) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          }
        }

        else {
          search();
        }
        attackPos();

        if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
          if(hardnesspair(next) < 85) {
            hardnesspair(next) = 0;
          }

          else {
            hardnesspair(next) -= 85;
          }

          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
        }

        if(hardnesspair(next) == 0) {

          if(mappair(next) == ter_wall) {
            mappair(next) = ter_floor_hall;
            pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
            pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
          }

          //check_cur_room(d, mon, NPC_MODE);
          char_gridxy(x, y) = nullptr;
          prev[dim_x] = x;
          prev[dim_y] = y;
          x = next[dim_x];
          y = next[dim_y];
          char_gridpair(next) = this;
        }
      }

      /* Non-Smart Non-Telepathic Non-Tunneling monsters */
      else {
        if(checkLos()) {
          if(d->player->getX() < x && hardnessxy(x - 1, y) == 0) {
            next[dim_x] = x - 1;
            next[dim_y] = y;
          }

          else if(d->player->getY() < y && hardnessxy(x, y - 1) == 0) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          }

          else if(d->player->getX() > x && hardnessxy(x + 1, y) == 0) {
            next[dim_x] = x + 1;
            next[dim_y] = y;
          }

          else if(d->player->getY() > y && hardnessxy(x, y + 1) == 0) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          }

          else if(d->player->getY() < y && hardnessxy(x - 1, y - 1) == 0) {
            next[dim_x] = x - 1;
            next[dim_y] = y - 1;
          }

          else if(d->player->getY() > y && hardnessxy(x - 1, y + 1) == 0) {
            next[dim_x] = x - 1;
            next[dim_y] = y + 1;
          }

          else if(d->player->getY() < y && hardnessxy(x + 1, y - 1) == 0) {
            next[dim_x] = x + 1;
            next[dim_y] = y - 1;
          }

          else if(d->player->getY() > y && hardnessxy(x + 1, y + 1) == 0) {
            next[dim_x] = x + 1;
            next[dim_y] = y + 1;
          }

          else {
            next[dim_y] = y;
            next[dim_x] = x;
          }

        }

        else {
          search();
        }
        attackPos();

        //check_cur_room(d, mon, NPC_MODE);
        char_gridxy(x, y) = nullptr;
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
    }
  }
  return 0;

  /* As a side note, I'm fully aware how terrible this is */
}


