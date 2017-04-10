#ifndef DUNGEON_H
#define DUNGEON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <ncurses.h>
#include <string>
#include <sstream>

#include "heap.h"
#include "dimensions.h"
#include "terrain.h"
#include "dice.h"
#include "character.h"
#include "item.h"
#include "npc.h"
#include "pc.h"
#include "display.h"

/* --from Dr Sheaffer-- */
#define DUNGEON_X 160
#define DUNGEON_Y 105
#define MIN_ROOMS 20 
#define MAX_ROOMS 30
#define ROOM_MIN_X 7
#define ROOM_MIN_Y 5
#define ROOM_MAX_X 20
#define ROOM_MAX_Y 15
#define PC_MODE 0
#define NPC_MODE 1

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

using std::string;
using std::stringstream;

/* --from Dr Sheaffer-- */ 
typedef struct corridor_path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} corridor_path_t;

typedef struct room {
  uint8_t x, y, length, height;
} room_t;

typedef struct dungeon {
  dungeon() : char_grid(), item_grid() {}
  bool nofog;
  heap_t event_heap;
  int16_t view_mode;
  pair_t lcoords;
  int num_rooms, nummon, numitems;
  long int seed; 
  room_t *rooms;
  terrain_t map[DUNGEON_Y][DUNGEON_X];
  character *char_grid[DUNGEON_Y][DUNGEON_X];
  item *item_grid[DUNGEON_Y][DUNGEON_X];
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  corridor_path_t tunnel_map[DUNGEON_Y][DUNGEON_X];
  corridor_path_t non_tunnel_map[DUNGEON_Y][DUNGEON_X];
  pc *player;
  string disp_msg;
  string level_msg;
  int level;
  abstractDisplay *display;
} _dungeon;

int dungeon_init(_dungeon *d);  
void printMonster(character *cp);
void printItems(item *ip);
uint8_t run_dungeon(_dungeon *d);
void create_monsters(_dungeon *d);
void createItems(_dungeon *d);
int mv_up_stairs(_dungeon *d);
int mv_dwn_stairs(_dungeon *d);
int smooth_hardness(_dungeon *d);
void empty_dungeon(_dungeon *d);
int in_room(_dungeon *d, int16_t y, int16_t x);
void make_rooms(_dungeon *d);
void fill_rooms(_dungeon *d);
int place_stairs(_dungeon *d);
int create_cycle(_dungeon *d);
void connect_rooms(_dungeon *d);
void print(_dungeon *d);
void load_dungeon(_dungeon *d);
int in_room(_dungeon *d, int16_t y, int16_t x);
int read_from_file(_dungeon *d, char *path);
void save_to_file(_dungeon *d, char* path);
void delete_dungeon(_dungeon *d);
void init_dungeon(_dungeon *d, uint8_t load);
int end_game(_dungeon *d, int mode);
void mount_ncurses(void);
void unmount_ncurses(void);

#endif
