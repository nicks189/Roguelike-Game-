#include "pathfinding.h"

int32_t weigh_hardness(uint8_t h) {
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

void pathfinding(_dungeon *d, uint8_t x_to, uint8_t y_to, corridor_path_t temp_path[DUNGEON_Y][DUNGEON_X], uint8_t mode) {
  corridor_path_t *p;
  heap_t h;
  uint32_t x, y;
  pair_t from;

  from[dim_x] = x_to;
  from[dim_y] = y_to;

  /* non tunneling monster map */
  if(mode == NON_TUNNEL_MODE) {
    temp_path[from[dim_y]][from[dim_x]].cost = 0;
    heap_init(&h, corridor_path_cmp, NULL);

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
          temp_path[y][x].pos[dim_y] = y;
          temp_path[y][x].pos[dim_x] = x;
      }
    }
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if(x != from[dim_x] || y != from[dim_y]) {
          temp_path[y][x].cost = INT_MAX;
        }
      }
    }
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (hardnessxy(x, y) == 0) {
          temp_path[y][x].hn = heap_insert(&h, &temp_path[y][x]);
        } else {
          temp_path[y][x].hn = NULL;
        }
      }
    }

    while ((p = heap_remove_min(&h))) {
      p->hn = NULL;

      if ((temp_path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
          (temp_path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
           (p->cost + 1))) {
        temp_path[p->pos[dim_y] - 1][p->pos[dim_x]].cost =
          (p->cost + 1);
        temp_path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
        temp_path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] - 1]
                                             [p->pos[dim_x]].hn);
      }
      if ((temp_path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
          (temp_path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
           (p->cost + 1))) {
        temp_path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
          (p->cost + 1);
        temp_path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
        temp_path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y]    ]
                                             [p->pos[dim_x] - 1].hn);
      }
      if ((temp_path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
          (temp_path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
           (p->cost + 1))) {
        temp_path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
          (p->cost + 1);
        temp_path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
        temp_path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y]    ]
                                             [p->pos[dim_x] + 1].hn);
      }
      if ((temp_path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
          (temp_path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
           (p->cost + 1))) {
        temp_path[p->pos[dim_y] + 1][p->pos[dim_x]].cost =
          (p->cost + 1);
        temp_path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
        temp_path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] + 1]
                                             [p->pos[dim_x]].hn);
      }
      if ((temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
          (temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
           (p->cost + 1))) {
        temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
          (p->cost + 1);
        temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
        temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] + 1]
                                             [p->pos[dim_x] + 1].hn);

      }
      if ((temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
         (temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
          (p->cost + 1))) {
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
         (p->cost + 1);
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] - 1]
                                            [p->pos[dim_x] + 1].hn);

      }
      if ((temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
         (temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
          (p->cost + 1))) {
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
         (p->cost + 1);
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] - 1]
                                            [p->pos[dim_x] - 1].hn);

      }
      if ((temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
         (temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
          (p->cost + 1))) {
       temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
         (p->cost + 1);
       temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
       temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] + 1]
                                            [p->pos[dim_x] - 1].hn);

      }
    }
    //memcpy(&path, &temp_path, sizeof(temp_path));
    heap_delete(&h);
    
  }
  
  /* tunneling monster map */
  if(mode == TUNNEL_MODE) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        temp_path[y][x].pos[dim_y] = y;
        temp_path[y][x].pos[dim_x] = x;
      }
    }
    
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        temp_path[y][x].cost = INT_MAX;
      }
    }

    temp_path[from[dim_y]][from[dim_x]].cost = 0;
    heap_init(&h, corridor_path_cmp, NULL);

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (mapxy(x, y) != ter_wall_immutable) {
          temp_path[y][x].hn = heap_insert(&h, &temp_path[y][x]);
        } else {
          temp_path[y][x].hn = NULL;
        }
      }
    }

    while ((p = heap_remove_min(&h))) {
      p->hn = NULL;
      
      if ((temp_path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
          (temp_path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
           p->cost + weigh_hardness(hardnesspair(p->pos)))) {
        temp_path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
          p->cost + weigh_hardness(hardnesspair(p->pos));
        temp_path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
        temp_path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] - 1]
                                             [p->pos[dim_x]    ].hn);
      }
      if ((temp_path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
          (temp_path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
           p->cost + weigh_hardness(hardnesspair(p->pos)))) {
        temp_path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost =
          p->cost + weigh_hardness(hardnesspair(p->pos));
        temp_path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
        temp_path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y]    ]
                                             [p->pos[dim_x] - 1].hn);
      }
      if ((temp_path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
          (temp_path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
           p->cost + weigh_hardness(hardnesspair(p->pos)))) {
        temp_path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
          p->cost + weigh_hardness(hardnesspair(p->pos));
        temp_path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
        temp_path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y]    ]
                                             [p->pos[dim_x] + 1].hn);
      }
      if ((temp_path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
          (temp_path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
           p->cost + weigh_hardness(hardnesspair(p->pos)))) {
        temp_path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
          p->cost + weigh_hardness(hardnesspair(p->pos));
        temp_path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
        temp_path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
        heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] + 1]
                                             [p->pos[dim_x]    ].hn);
      } 
      if ((temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn) &&
         (temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
          p->cost + weigh_hardness(hardnesspair(p->pos)))) {
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost =
         p->cost + weigh_hardness(hardnesspair(p->pos));
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] - 1]
                                            [p->pos[dim_x] + 1].hn);

      }
      if ((temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn) &&
         (temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
          p->cost + weigh_hardness(hardnesspair(p->pos)))) {
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost =
         p->cost + weigh_hardness(hardnesspair(p->pos));
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
       temp_path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] - 1]
                                            [p->pos[dim_x] - 1].hn);

      }
      if ((temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn) &&
         (temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
          p->cost + weigh_hardness(hardnesspair(p->pos)))) {
       temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost =
         p->cost + weigh_hardness(hardnesspair(p->pos));
       temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
       temp_path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] + 1]
                                            [p->pos[dim_x] - 1].hn);

      } 
      if ((temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
         (temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
          p->cost + weigh_hardness(hardnesspair(p->pos)))) {
       temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
         p->cost + weigh_hardness(hardnesspair(p->pos));
       temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
       temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
       heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] + 1]
                                            [p->pos[dim_x] + 1].hn);

      }
      if ((temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn) &&
           (temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
            p->cost + weigh_hardness(hardnesspair(p->pos)))) {
          temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost =
            p->cost + weigh_hardness(hardnesspair(p->pos));
          temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
          temp_path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
          heap_decrease_key_no_replace(&h, temp_path[p->pos[dim_y] + 1]
                                            [p->pos[dim_x] + 1].hn);

     }

    }
    //memcpy(&path, &temp_path, sizeof(temp_path));
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

        else if(mapxy(x, y) == character_pc) {
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

        if(mapxy(x, y) == character_pc) {
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
  uint32_t x, y;

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

  heap_init(&h, corridor_path_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        if (mapxy(x, y) != ter_floor_room && mapxy(x,y) != character_pc) {
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
  }
}

/* --from Dr Sheaffer-- */
void dijkstra_corridor_inv(_dungeon *d, pair_t from, pair_t to)
{
  static corridor_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

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

  heap_init(&h, corridor_path_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (mapxy(x, y) != ter_wall_immutable) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = heap_remove_min(&h))) {
    p->hn = NULL;

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
  }
}




