#include "pathfinding.h"

inline int weigh_hardness(uint8_t h) {
  if(h < 85) {
    return 1;
  }
  else if(h < 170) {
    return 2;
  }
  else {
    return 3;
  } 
}

void pathfinding(_dungeon *d, int16_t x_to, int16_t y_to, uint8_t mode) {
  corridor_path_t *p;
  heap_t h;
  uint8_t x, y;
  pair_t from;

  static int init1 = 0;
  static int init0 = 0;

  from[dim_x] = x_to;
  from[dim_y] = y_to;

  /* non tunneling monster map */
  if(mode == NON_TUNNEL_MODE) {

    if(!init0) {
      init0 = 1;
      for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
            d->non_tunnel_map[y][x].pos[dim_y] = y;
            d->non_tunnel_map[y][x].pos[dim_x] = x;
        }
      }
    }

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if(x != from[dim_x] || y != from[dim_y]) {
          d->non_tunnel_map[y][x].cost = INT_MAX;
        }
      }
    }

    d->non_tunnel_map[from[dim_y]][from[dim_x]].cost = 0;
    heap_init(&h, corridor_path_cmp, nullptr);

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (hardnessxy(x, y) == 0) {
          d->non_tunnel_map[y][x].hn = heap_insert(&h, &d->non_tunnel_map[y][x]);
        } else {
          d->non_tunnel_map[y][x].hn = nullptr;
        }
      }
    }

    while ((p = (corridor_path_t *) heap_remove_min(&h))) {
      p->hn = nullptr;

      if ((d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
          (d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
           (p->cost + 1))) {
        d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]].cost =
          (p->cost + 1);
        d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
        d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, d->non_tunnel_map[p->pos[dim_y] - 1]
                                             [p->pos[dim_x]].hn);
      }
      if ((d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
          (d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
           (p->cost + 1))) {
        d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
          (p->cost + 1);
        d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
        d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, d->non_tunnel_map[p->pos[dim_y]    ]
                                             [p->pos[dim_x] - 1].hn);
      }
      if ((d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
          (d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
           (p->cost + 1))) {
        d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
          (p->cost + 1);
        d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
        d->non_tunnel_map[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, d->non_tunnel_map[p->pos[dim_y]    ]
                                             [p->pos[dim_x] + 1].hn);
      }
      if ((d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
          (d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
           (p->cost + 1))) {
        d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]].cost =
          (p->cost + 1);
        d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
        d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, d->non_tunnel_map[p->pos[dim_y] + 1]
                                             [p->pos[dim_x]].hn);
      }
      if ((d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
          (d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
           (p->cost + 1))) {
        d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
          (p->cost + 1);
        d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
        d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, d->non_tunnel_map[p->pos[dim_y] + 1]
                                             [p->pos[dim_x] + 1].hn);

      }
      if ((d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
         (d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
          (p->cost + 1))) {
       d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
         (p->cost + 1);
       d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
       d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, d->non_tunnel_map[p->pos[dim_y] - 1]
                                            [p->pos[dim_x] + 1].hn);

      }
      if ((d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
         (d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
          (p->cost + 1))) {
       d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
         (p->cost + 1);
       d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
       d->non_tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, d->non_tunnel_map[p->pos[dim_y] - 1]
                                            [p->pos[dim_x] - 1].hn);

      }
      if ((d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
         (d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
          (p->cost + 1))) {
       d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
         (p->cost + 1);
       d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
       d->non_tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, d->non_tunnel_map[p->pos[dim_y] + 1]
                                            [p->pos[dim_x] - 1].hn);

      }
    }
    //memcpy(&path, &d->tunnel_map, sizeof(temp_path));
    heap_delete(&h);
    
  }
  
  /* tunneling monster map */
  if(mode == TUNNEL_MODE) {
    if(!init1) {
      init1 = 1;
      for (y = 0; y < DUNGEON_Y; y++) {
        for (x = 0; x < DUNGEON_X; x++) {
          d->tunnel_map[y][x].pos[dim_y] = y;
          d->tunnel_map[y][x].pos[dim_x] = x;
        }
      }
    }
    
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        d->tunnel_map[y][x].cost = INT_MAX;
      }
    }

    d->tunnel_map[from[dim_y]][from[dim_x]].cost = 0;
    heap_init(&h, corridor_path_cmp, nullptr);

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (mapxy(x, y) != ter_wall_immutable) {
          d->tunnel_map[y][x].hn = heap_insert(&h, &d->tunnel_map[y][x]);
        } else {
          d->tunnel_map[y][x].hn = nullptr;
        }
      }
    }

    while ((p = (corridor_path_t *) heap_remove_min(&h))) {
      p->hn = nullptr;
      
      if ((d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
          (d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
           p->cost + weigh_hardness(hardnesspair(p->pos)))) {
        d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
          p->cost + weigh_hardness(hardnesspair(p->pos));
        d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
        d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, d->tunnel_map[p->pos[dim_y] - 1]
                                             [p->pos[dim_x]    ].hn);
      }
      if ((d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
          (d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
           p->cost + weigh_hardness(hardnesspair(p->pos)))) {
        d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
          p->cost + weigh_hardness(hardnesspair(p->pos));
        d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
        d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, d->tunnel_map[p->pos[dim_y]    ]
                                             [p->pos[dim_x] - 1].hn);
      }
      if ((d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
          (d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
           p->cost + weigh_hardness(hardnesspair(p->pos)))) {
        d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
          p->cost + weigh_hardness(hardnesspair(p->pos));
        d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
        d->tunnel_map[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, d->tunnel_map[p->pos[dim_y]    ]
                                             [p->pos[dim_x] + 1].hn);
      }
      if ((d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
          (d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
           p->cost + weigh_hardness(hardnesspair(p->pos)))) {
        d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
          p->cost + weigh_hardness(hardnesspair(p->pos));
        d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
        d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, d->tunnel_map[p->pos[dim_y] + 1]
                                             [p->pos[dim_x]    ].hn);
      } 
      if ((d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
         (d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
          p->cost + weigh_hardness(hardnesspair(p->pos)))) {
       d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
         p->cost + weigh_hardness(hardnesspair(p->pos));
       d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
       d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, d->tunnel_map[p->pos[dim_y] - 1]
                                            [p->pos[dim_x] + 1].hn);

      }
      if ((d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
         (d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
          p->cost + weigh_hardness(hardnesspair(p->pos)))) {
       d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
         p->cost + weigh_hardness(hardnesspair(p->pos));
       d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
       d->tunnel_map[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, d->tunnel_map[p->pos[dim_y] - 1]
                                            [p->pos[dim_x] - 1].hn);

      }
      if ((d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
         (d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
          p->cost + weigh_hardness(hardnesspair(p->pos)))) {
       d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
         p->cost + weigh_hardness(hardnesspair(p->pos));
       d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
       d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, d->tunnel_map[p->pos[dim_y] + 1]
                                            [p->pos[dim_x] - 1].hn);

      } 
      if ((d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
         (d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
          p->cost + weigh_hardness(hardnesspair(p->pos)))) {
       d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
         p->cost + weigh_hardness(hardnesspair(p->pos));
       d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
       d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, d->tunnel_map[p->pos[dim_y] + 1]
                                            [p->pos[dim_x] + 1].hn);

      }
      if ((d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
           (d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
            p->cost + weigh_hardness(hardnesspair(p->pos)))) {
          d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
            p->cost + weigh_hardness(hardnesspair(p->pos));
          d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
          d->tunnel_map[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
          heap_decrease_key_no_replace(&h, d->tunnel_map[p->pos[dim_y] + 1]
                                            [p->pos[dim_x] + 1].hn);

     }

    }
    //memcpy(&path, &d->tunnel_map, sizeof(temp_path));
    heap_delete(&h);  
  }
}

void print_maps(_dungeon *d, uint8_t l) {
  int x, y;
  if(l) { 

    putchar('\n');
    for(y = 1; y < DUNGEON_Y - 1; y++) {
      for(x = 1; x < DUNGEON_X - 1; x++) {

        if(hardnessxy(x, y) != 0) {
          putchar(' ');
        }

        else if(x == d->player->getX() && y == d->player->getY()) {
          putchar('@');
        }

        else {
          printf("%d", d->non_tunnel_map[y][x].cost % 10);
        }

      }
      putchar('\n');
    }
    putchar('\n');
    for(y = 1; y < DUNGEON_Y - 1; y++) {
      for(x = 1; x < DUNGEON_X - 1; x++) {

        if(x == d->player->getX() && y == d->player->getY()) {
          putchar('@');
        }

        else {
          printf("%d", d->tunnel_map[y][x].cost % 10);
        } 

      }
    putchar('\n');
    }
  }
}

/* --from Dr Sheaffer-- */
int32_t corridor_path_cmp(const void *key, const void *with) {
  return ((corridor_path_t *) key)->cost - ((corridor_path_t *) with)->cost;
}

/* --from Dr Sheaffer-- */
void dijkstra_corridor(_dungeon *d, pair_t from, pair_t to)
{
  static corridor_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  int16_t x, y;

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, corridor_path_cmp, nullptr);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = nullptr;
      }
    }
  }

  while ((p = (corridor_path_t *) heap_remove_min(&h))) {
    p->hn = nullptr;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        if (mapxy(x, y) != ter_floor_room && (x != d->player->getX() || y != d->player->getY())) {
          mapxy(x, y) = ter_floor_hall;
          hardnessxy(x, y) = 0;
        }
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair(p->pos))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair(p->pos);
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
    p = nullptr;
  }
}

/* --from Dr Sheaffer-- */
void dijkstra_corridor_inv(_dungeon *d, pair_t from, pair_t to)
{
  static corridor_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  int16_t x, y;

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, corridor_path_cmp, nullptr);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = nullptr;
      }
    }
  }

  while ((p = (corridor_path_t *) heap_remove_min(&h))) {
    p->hn = nullptr;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        if (mapxy(x, y) != ter_floor_room) {
          mapxy(x, y) = ter_floor_hall;
          hardnessxy(x, y) = 0;
        }
      }
      heap_delete(&h);
      return;
    }

#define hardnesspair_inv(p) (in_room(d, p[dim_y], p[dim_x]) ? \
                             224                            : \
                             (255 - hardnesspair(p)))

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
    p = nullptr;
  }
}

