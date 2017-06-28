#include "../include/npc.h"
#include "../include/pathfinding.h"
#include "../include/character.h"
#include "../include/dungeon.h"

npc::npc(_dungeon *dun) : character(dun), npcItem() {
  search_dir = rand_range(0, 7);

  int tx, ty, tempRoom;
  do {
    tempRoom = rand_range(1, d->numrooms - 1);
    tx = rand_range(d->rooms[tempRoom].x + 1,
         d->rooms[tempRoom].length - 2 + d->rooms[tempRoom].x);

    ty = rand_range(d->rooms[tempRoom].y + 1,
         d->rooms[tempRoom].height - 2 + d->rooms[tempRoom].y);
  } while(!isOpen(tx, ty));

  x = tx;
  y = ty;
  prev[dim_x] = x;
  prev[dim_y] = y;
  pc_lsp[dim_x] = 0;
  pc_lsp[dim_y] = 0;
}

npc::npc(npc &n) : character(n.d), npcItem() {
  search_dir = rand_range(0, 7);
  level = n.level;
  name = n.name;
  description = n.description;
  damage = new dice();
  damage->setBase(n.damage->getBase());
  damage->setNumber(n.damage->getNumber());
  damage->setSides(n.damage->getSides());
  hp = n.hp;
  color = n.color;
  traits = n.traits;
  speed = n.speed;
  dead = n.dead;
  symbol = n.symbol;

  int tx, ty, tempRoom;
  do {
    tempRoom = rand_range(1, d->numrooms - 1);
    tx = rand_range(d->rooms[tempRoom].x + 1,
         d->rooms[tempRoom].length - 2 + d->rooms[tempRoom].x);

    ty = rand_range(d->rooms[tempRoom].y + 1,
         d->rooms[tempRoom].height - 2 + d->rooms[tempRoom].y);
  } while(!isOpen(tx, ty));

  x = tx;
  y = ty;
  prev[dim_x] = x;
  prev[dim_y] = y;
  pc_lsp[dim_x] = 0;
  pc_lsp[dim_y] = 0;
}

int npc::attackPos() {
  character *c = char_gridpair(next);

  if(next[dim_x] == d->player->getX() && next[dim_y] == d->player->getY())  {
    int temphit = hit;
    temphit -= d->player->getDodge();

    if(temphit < rand_range(0, 100)) {
      d->disp_msg = "You dodged ";
      d->disp_msg.append(name);
      d->disp_msg.append("'s attack");
      next[dim_x] = x;
      next[dim_y] = y;
      return 1;
    }
    int dmg = damage->roll();
    d->disp_msg = "";
    d->disp_msg.append(name);
    d->disp_msg.append(" hit you for ");
    d->disp_msg.append(std::to_string(dmg));
    d->player->setHp(d->player->getHp() - dmg);

    if(d->player->getHp() < 1) {
      d->disp_msg.append(". Everything goes dark...");
      return end_game(d, NPC_MODE);
    }
    next[dim_x] = x;
    next[dim_y] = y;
  }

  if(c != nullptr && this != c) {
    c->setX(x);
    c->setY(y);
  }
  char_gridxy(x, y) = c;
  return 0;
}

int npc::searchHelper(int16_t *p) {
  if((traits & NPC_PASS_WALL) && (mappair(p) != ter_wall_immutable)) {
    return 0;
  } else if(mappair(p) == ter_floor_room || mappair(p) == ter_floor_hall) {
    return 0;
  } else {
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
        p[dim_y] = y + 1;
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

// Uses Bresenham's Line Algorithm
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
  } else {
    del[dim_x] = first[dim_x] - second[dim_x];
    f[dim_x] = -1;
  }
  if (second[dim_y] > first[dim_y]) {
    del[dim_y] = second[dim_y] - first[dim_y];
    f[dim_y] = 1;
  } else {
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
      } else {
        c += b;
        first[dim_y] += f[dim_y];
      }
    }
    return 1;
  } else {
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
      } else {
        c += b;
        first[dim_x] += f[dim_x];
      }
    }
    return 1;
  }
}

int npc::isOpen(int xt, int yt) {
  if(mapxy(xt, yt) == ter_floor_room && char_gridxy(xt, yt) == nullptr) {
    return 1;
  }
  return 0;
}

inline bool npc::checkShortestPath(int16_t *pos, bool pcLos) {
  /* __TODO__ */
  if(pcLos != 1) {
    return true;
  }
  return false;
}

/* Here there be monsters (as in really crappy code)
 * You've been warned...
 * --
 * Refactor this into multiple functions
 */
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

  /* check if Erratic */
  if(traits & NPC_ERRATIC && ((rand() % 2) == 1)) {
    if(traits & NPC_TUNNEL) {
      findRandNeighbor();
      attackPos();
      if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
        if(hardnesspair(next) < 85) {
          hardnesspair(next) = 0;
        } else {
          hardnesspair(next) -= 85;
        }
        pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
      }
      if(hardnesspair(next) == 0) {
        if(mappair(next) == ter_wall || mappair(next) == ter_border_south
            || mappair(next) == ter_border_north || mappair(next) == ter_border_west
            || mappair(next) == ter_border_east) {
          mappair(next) = ter_floor_hall;
          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
          pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
        }

        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
    } else {
      findRandNeighbor();
      attackPos();

      prev[dim_x] = x;
      prev[dim_y] = y;
      x = next[dim_x];
      y = next[dim_y];
      char_gridpair(next) = this;
    }
    return 0;
  }

  /* Smart monsters */
  if(traits & NPC_SMART) {
    /* Smart Telepathic monsters */
    if(traits & NPC_TELEPATH) {
      /* Smart Telepath Pass monsters */
      if(traits & NPC_PASS_WALL) {
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
        attackPos();
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
      /* Smart Telepathic Tunneling monsters */
      else if(traits & NPC_TUNNEL) {
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
        attackPos();
        if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
          if(hardnesspair(next) < 85) {
            hardnesspair(next) = 0;
          } else {
            hardnesspair(next) -= 85;
          }
          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
        }
        if(hardnesspair(next) == 0) {
          if(mappair(next) == ter_wall || mappair(next) == ter_border_south
              || mappair(next) == ter_border_north || mappair(next) == ter_border_west
              || mappair(next) == ter_border_east) {
            mappair(next) = ter_floor_hall;
            pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
            pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
          }

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
        attackPos();
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
    }
    /* Smart Non-Telepathic monsters */
    else {
      /*Smart Non-Telepathic Pass monsters */
      if(traits & NPC_PASS_WALL && !(traits & NPC_TELEPATH)) {
        /* If pc is in LoS */
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
        }
        /* If pc is not in LoS and has a LSP */
        else if(!(pc_lsp[dim_y] == 0)) {
          if(pc_lsp[dim_x] < x) {
            if(pc_lsp[dim_y] < y) {
              next[dim_x] = x - 1;
              next[dim_y] = y - 1;
            } else if(pc_lsp[dim_y] > y) {
              next[dim_x] = x - 1;
              next[dim_y] = y + 1;
            } else {
              next[dim_x] = x - 1;
              next[dim_y] = y;
            }
          } else if(pc_lsp[dim_x] > x) {
            if(pc_lsp[dim_y] < y) {
              next[dim_x] = x + 1;
              next[dim_y] = y - 1;
            } else if(pc_lsp[dim_y] > y) {
              next[dim_x] = x + 1;
              next[dim_y] = y + 1;
            } else {
              next[dim_x] = x + 1;
              next[dim_y] = y;
            }
          } else if(pc_lsp[dim_y] > y) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          } else if(pc_lsp[dim_y] < y) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          } else {
            next[dim_y] = y;
            next[dim_x] = x;
          }
        } else {
          search();
        }
        attackPos();
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
      /*Smart Non-Telepathic Non-Tunneling monsters */
      else if(!(traits & NPC_TUNNEL)) {
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
        }
        /* If pc is not in LoS and has a LSP */
        else if(!(pc_lsp[dim_y] == 0)) {
          if(pc_lsp[dim_x] < x) {
            if(pc_lsp[dim_y] < y && hardnessxy(x - 1, y - 1) == 0) {
              next[dim_x] = x - 1;
              next[dim_y] = y - 1;
            } else if(pc_lsp[dim_y] > y && hardnessxy(x - 1, y + 1) == 0) {
              next[dim_x] = x - 1;
              next[dim_y] = y + 1;
            } else if(hardnessxy(x - 1, y) == 0){
              next[dim_x] = x - 1;
              next[dim_y] = y;
            }
          } else if(pc_lsp[dim_x] > x) {
            if(pc_lsp[dim_y] < y && hardnessxy(x + 1, y - 1) == 0) {
              next[dim_x] = x + 1;
              next[dim_y] = y - 1;
            } else if(pc_lsp[dim_y] > y && hardnessxy(x + 1, y + 1) == 0) {
              next[dim_x] = x + 1;
              next[dim_y] = y + 1;
            } else if(hardnessxy(x + 1, y) == 0){
              next[dim_x] = x + 1;
              next[dim_y] = y;
            }
          } else if(pc_lsp[dim_y] > y && hardnessxy(x, y + 1) == 0) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          } else if(pc_lsp[dim_y] < y && hardnessxy(x, y - 1) == 0) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          } else {
            next[dim_y] = y;
            next[dim_x] = x;
          }
        } else {
          search();
        }
        attackPos();
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
            } else if(pc_lsp[dim_y] > y) {
              next[dim_x] = x - 1;
              next[dim_y] = y + 1;
            } else {
              next[dim_x] = x - 1;
              next[dim_y] = y;
            }
          } else if(pc_lsp[dim_x] > x) {
            if(pc_lsp[dim_y] < y) {
              next[dim_x] = x + 1;
              next[dim_y] = y - 1;
            } else if(pc_lsp[dim_y] > y) {
              next[dim_x] = x + 1;
              next[dim_y] = y + 1;
            } else {
              next[dim_x] = x + 1;
              next[dim_y] = y;
            }
          } else if(pc_lsp[dim_y] > y) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          } else if(pc_lsp[dim_y] < y) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          }
        } else {
          search();
        }
        attackPos();
        if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
          if(hardnesspair(next) < 85) {
            hardnesspair(next) = 0;
          } else {
            hardnesspair(next) -= 85;
          }
          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
        }
        if(hardnesspair(next) == 0) {
          if(mappair(next) == ter_wall || mappair(next) == ter_border_south
              || mappair(next) == ter_border_north || mappair(next) == ter_border_west
              || mappair(next) == ter_border_east) {
            mappair(next) = ter_floor_hall;
            pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
            pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
          }
          prev[dim_x] = x;
          prev[dim_y] = y;
          x = next[dim_x];
          y = next[dim_y];
          char_gridpair(next) = this;
        }
      }
    }
  }
  /* Half way there... */
  /* Non Smart monsters */
  else {
    /* Non-Smart Telepathic monsters */
    if(traits & NPC_TELEPATH) {
      /* Non-Smart Telepathic Pass monsters */
      if(traits & NPC_PASS_WALL) {
        if(d->player->getX() < x) {
          if(d->player->getY() < y) {
            next[dim_x] = x - 1;
            next[dim_y] = y - 1;
          } else if(d->player->getY() > y) {
            next[dim_x] = x - 1;
            next[dim_y] = y + 1;
          } else {
            next[dim_x] = x - 1;
            next[dim_y] = y;
          }
        } else if(d->player->getX() > x) {
          if(d->player->getY() < y) {
            next[dim_x] = x + 1;
            next[dim_y] = y - 1;
          } else if(d->player->getY() > y) {
            next[dim_x] = x + 1;
            next[dim_y] = y + 1;
          } else {
            next[dim_x] = x + 1;
            next[dim_y] = y;
          }
        } else if(d->player->getY() > y) {
          next[dim_x] = x;
          next[dim_y] = y + 1;
        } else if(d->player->getY() < y) {
          next[dim_x] = x;
          next[dim_y] = y - 1;
        }
        attackPos();
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
      /* Non-Smart Telepathic Tunneling monsters */
      else if(traits & NPC_TUNNEL) {
        if(d->player->getX() < x) {
          if(d->player->getY() < y) {
            next[dim_x] = x - 1;
            next[dim_y] = y - 1;
          } else if(d->player->getY() > y) {
            next[dim_x] = x - 1;
            next[dim_y] = y + 1;
          } else {
            next[dim_x] = x - 1;
            next[dim_y] = y;
          }
        } else if(d->player->getX() > x) {
          if(d->player->getY() < y) {
            next[dim_x] = x + 1;
            next[dim_y] = y - 1;
          } else if(d->player->getY() > y) {
            next[dim_x] = x + 1;
            next[dim_y] = y + 1;
          } else {
            next[dim_x] = x + 1;
            next[dim_y] = y;
          }
        } else if(d->player->getY() > y) {
          next[dim_x] = x;
          next[dim_y] = y + 1;
        } else if(d->player->getY() < y) {
          next[dim_x] = x;
          next[dim_y] = y - 1;
        }
        attackPos();
        if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
          if(hardnesspair(next) < 85) {
            hardnesspair(next) = 0;
          } else {
            hardnesspair(next) -= 85;
          }
          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
        }
        if(hardnesspair(next) == 0) {
          if(mappair(next) == ter_wall || mappair(next) == ter_border_south
              || mappair(next) == ter_border_north || mappair(next) == ter_border_west
              || mappair(next) == ter_border_east) {
            mappair(next) = ter_floor_hall;
            pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
            pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
          }
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
        } else if(d->player->getY() < y && hardnessxy(x, y - 1) == 0) {
          next[dim_x] = x;
          next[dim_y] = y - 1;
        } else if(d->player->getX() > x && hardnessxy(x + 1, y) == 0) {
          next[dim_x] = x + 1;
          next[dim_y] = y;
        } else if(d->player->getY() > y && hardnessxy(x, y + 1) == 0) {
          next[dim_x] = x;
          next[dim_y] = y + 1;
        } else if(d->player->getY() < y && hardnessxy(x - 1, y - 1) == 0) {
          next[dim_x] = x - 1;
          next[dim_y] = y - 1;
        } else if(d->player->getY() > y && hardnessxy(x - 1, y + 1) == 0) {
          next[dim_x] = x - 1;
          next[dim_y] = y + 1;
        } else if(d->player->getY() < y && hardnessxy(x + 1, y - 1) == 0) {
          next[dim_x] = x + 1;
          next[dim_y] = y - 1;
        } else if(d->player->getY() > y && hardnessxy(x + 1, y + 1) == 0) {
          next[dim_x] = x + 1;
          next[dim_y] = y + 1;
        } else {
          next[dim_y] = y;
          next[dim_x] = x;
        }
        attackPos();
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
    }
    /* Non-Smart Non-Telepathic Monsters */
    else {
      /* Non-Smart Non-Telepathic Pass monsters */
      if(traits & NPC_PASS_WALL) {
        if(checkLos()) {
          if(d->player->getX() < x) {
            if(d->player->getY() < y) {
              next[dim_x] = x - 1;
              next[dim_y] = y - 1;
            } else if(d->player->getY() > y) {
              next[dim_x] = x - 1;
              next[dim_y] = y + 1;
            } else {
              next[dim_x] = x - 1;
              next[dim_y] = y;
            }
          } else if(d->player->getX() > x) {
            if(d->player->getY() < y) {
              next[dim_x] = x + 1;
              next[dim_y] = y - 1;
            } else if(d->player->getY() > y) {
              next[dim_x] = x + 1;
              next[dim_y] = y + 1;
            } else {
              next[dim_x] = x + 1;
              next[dim_y] = y;
            }
          } else if(d->player->getY() > y) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          } else if(d->player->getY() < y) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          }
        } else {
          search();
        }
        attackPos();
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
      /* Non-Smart Non-Telepathic Tunneling monsters */
      else if(traits & NPC_TUNNEL) {
        if(checkLos()) {
          if(d->player->getX() < x) {
            if(d->player->getY() < y) {
              next[dim_x] = x - 1;
              next[dim_y] = y - 1;
            } else if(d->player->getY() > y) {
              next[dim_x] = x - 1;
              next[dim_y] = y + 1;
            } else {
              next[dim_x] = x - 1;
              next[dim_y] = y;
            }
          } else if(d->player->getX() > x) {
            if(d->player->getY() < y) {
              next[dim_x] = x + 1;
              next[dim_y] = y - 1;
            } else if(d->player->getY() > y) {
              next[dim_x] = x + 1;
              next[dim_y] = y + 1;
            } else {
              next[dim_x] = x + 1;
              next[dim_y] = y;
            }
          } else if(d->player->getY() > y) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          } else if(d->player->getY() < y) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          }
        } else {
          search();
        }
        attackPos();
        if(hardnesspair(next) != 0 && hardnesspair(next) != 255) {
          if(hardnesspair(next) < 85) {
            hardnesspair(next) = 0;
          } else {
            hardnesspair(next) -= 85;
          }
          pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
        }
        if(hardnesspair(next) == 0) {
          if(mappair(next) == ter_wall || mappair(next) == ter_border_south
              || mappair(next) == ter_border_north || mappair(next) == ter_border_west
              || mappair(next) == ter_border_east) {
            mappair(next) = ter_floor_hall;
            pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
            pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
          }
          //check_cur_room(d, mon, NPC_MODE);
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
          } else if(d->player->getY() < y && hardnessxy(x, y - 1) == 0) {
            next[dim_x] = x;
            next[dim_y] = y - 1;
          } else if(d->player->getX() > x && hardnessxy(x + 1, y) == 0) {
            next[dim_x] = x + 1;
            next[dim_y] = y;
          } else if(d->player->getY() > y && hardnessxy(x, y + 1) == 0) {
            next[dim_x] = x;
            next[dim_y] = y + 1;
          } else if(d->player->getY() < y && hardnessxy(x - 1, y - 1) == 0) {
            next[dim_x] = x - 1;
            next[dim_y] = y - 1;
          } else if(d->player->getY() > y && hardnessxy(x - 1, y + 1) == 0) {
            next[dim_x] = x - 1;
            next[dim_y] = y + 1;
          } else if(d->player->getY() < y && hardnessxy(x + 1, y - 1) == 0) {
            next[dim_x] = x + 1;
            next[dim_y] = y - 1;
          } else if(d->player->getY() > y && hardnessxy(x + 1, y + 1) == 0) {
            next[dim_x] = x + 1;
            next[dim_y] = y + 1;
          } else {
            next[dim_y] = y;
            next[dim_x] = x;
          }
        } else {
          search();
        }
        attackPos();
        //check_cur_room(d, mon, NPC_MODE);
        prev[dim_x] = x;
        prev[dim_y] = y;
        x = next[dim_x];
        y = next[dim_y];
        char_gridpair(next) = this;
      }
    }
  }
  if(traits & NPC_DESTROY_ITEM) {
    if(d->item_grid[y][x] != nullptr) {
      delete d->item_grid[y][x];
      d->item_grid[y][x] = nullptr;
    }
  }
  if(traits & NPC_PICKUP_ITEM) {
    if(d->item_grid[y][x] != nullptr) {
      npcItem = d->item_grid[y][x];
      d->item_grid[y][x] = nullptr;
    }
  }
  return 0;
  /* That escalated very quickly...
  * As a side note, I'm fully aware how terrible this is
  * and I will fix it at some point.
  */
}
