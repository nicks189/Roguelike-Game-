#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "heap.h"

/* --from Dr Sheaffer-- */
#define DUNGEON_X 160
#define DUNGEON_Y 105
#define MIN_ROOMS 20 
#define MAX_ROOMS 30
#define ROOM_MIN_X 7
#define ROOM_MIN_Y 5
#define ROOM_MAX_X 20
#define ROOM_MAX_Y 15

#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))
#define mapxy(x, y) (d->map[y][x])
#define hardnessxy(x, y) (d->hardness[y][x])
#define mappair(pair) (d->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (d->map[y][x])
#define hardnesspair(pair) (d->hardness[pair[dim_y]][pair[dim_x]])
#define hardnessxy(x, y) (d->hardness[y][x])
#define tunnelxy(x, y) (d->tunnel_map[y][x])
#define tunnelpair(pair) (d->tunnel_map[pair[dim_y]][pair[dim_x]])
#define ntunnelxy(x, y) (d->non_tunnel_map[y][x])
#define ntunnelpair(pair) (d->non_tunnel_map[pair[dim_y]][pair[dim_x]])
#define searchmapxy(x, y) (mon->search_map[y][x])
#define searchmappair(pair) (mon->search_map[pair[dim_y]][pair[dim_x]])
#define char_gridxy(x, y) (d->char_grid[y][x])
#define char_gridpair(pair) (d->char_grid[pair[dim_y]][pair[dim_x]])


/* --from Dr Sheaffer-- */ 
typedef struct corridor_path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} corridor_path_t;

/* --from Dr Sheaffer-- */
typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef int16_t pair_t[num_dims];

/* --from Dr Sheaffer-- */
typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_wall,
  ter_wall_immutable,
  ter_floor,
  ter_floor_room,
  ter_floor_hall,
  endgame_flag
} _terrain_type;

enum event_type {
  npc_type,
  pc_type
};

typedef struct pc {
  uint8_t x, y, speed;
  uint8_t curroom;
} _pc;

typedef struct npc {
  uint8_t x, y, dead, curroom, pc_los, searching, speed;
  uint32_t trait; 
  char type;
  pair_t pc_lsp;
  corridor_path_t search_map[DUNGEON_Y][DUNGEON_X];
  pair_t search_to;
} _npc;

typedef struct event {
  uint32_t time;
  enum event_type type;
  uint32_t sequence;
  union {
    _pc *pc;
    _npc *npc;
  } u;
} event_t;

typedef struct room {
  uint8_t x, y, length, height;
} _room;

typedef struct dungeon {
  uint32_t num_rooms;
  long int seed; 
  _room *rooms;
  uint32_t nummon;
  _terrain_type map[DUNGEON_Y][DUNGEON_X];
  _npc *char_grid[DUNGEON_Y][DUNGEON_X];
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  corridor_path_t tunnel_map[DUNGEON_Y][DUNGEON_X];
  corridor_path_t non_tunnel_map[DUNGEON_Y][DUNGEON_X];
  _pc player;
  _npc *npc_arr;
} _dungeon;

int end_game(int mode, _dungeon *d);
int smooth_hardness(_dungeon *d);
void empty_dungeon(_dungeon *d);
int in_room(_dungeon *d, int16_t y, int16_t x);
void make_rooms(_dungeon *d);
void fill_rooms(_dungeon *d);
int create_cycle(_dungeon *d);
void connect_rooms(_dungeon *d);
void print(_dungeon *d);
void load_dungeon(_dungeon *d);
int in_room(_dungeon *d, int16_t y, int16_t x);
int read_from_file(_dungeon *d, char *path);
void save_to_file(_dungeon *d, char* path);
void delete_dungeon(_dungeon *d);
void init_dungeon(_dungeon *d, uint8_t load);
uint8_t update_dungeon(_dungeon *d);
int create_dungeon(_dungeon *d, uint8_t load, 
   uint8_t save, char *rpath, char* spath,
     uint8_t pc_loaded, pair_t pc_loc);

#endif
