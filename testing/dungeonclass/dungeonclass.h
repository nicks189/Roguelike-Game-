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

//#include "heap.h"
//#include "dimensions.h"
//#include "terrain.h"
//#include "dice.h"
//#include "character.h"
//#include "item.h"
//#include "npc.h"
//#include "pc.h"
//#include "display.h"

#define DUNGEON_X   160
#define DUNGEON_Y   105
#define MIN_ROOMS   20 
#define MAX_ROOMS   30
#define ROOM_MIN_X  7
#define ROOM_MIN_Y  5
#define ROOM_MAX_X  20
#define ROOM_MAX_Y  15
#define PC_MODE     0
#define NPC_MODE    1

#define rand_range(min, max)  ((rand() % (((max) + 1) - (min))) + (min))
#define mapxy(x, y)           (d->map[y][x])
#define hardnessxy(x, y)      (d->hardness[y][x])
#define mappair(pair)         (d->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y)           (d->map[y][x])
#define hardnesspair(pair)    (d->hardness[pair[dim_y]][pair[dim_x]])
#define hardnessxy(x, y)      (d->hardness[y][x])
#define tunnelxy(x, y)        (d->tunnel_map[y][x])
#define tunnelpair(pair)      (d->tunnel_map[pair[dim_y]][pair[dim_x]])
#define ntunnelxy(x, y)       (d->non_tunnel_map[y][x])
#define ntunnelpair(pair)     (d->non_tunnel_map[pair[dim_y]][pair[dim_x]])
#define searchmapxy(x, y)     (mon->search_map[y][x])
#define searchmappair(pair)   (mon->search_map[pair[dim_y]][pair[dim_x]])
#define char_gridxy(x, y)     (d->char_grid[y][x])
#define char_gridpair(pair)   (d->char_grid[pair[dim_y]][pair[dim_x]])

using std::string;

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

/* Dungeon class - in reality this is 
 * more of a top level game class */
class dungeonclass {
  public:
    bool nofog;
    bool custom;
    uint8_t hardness[DUNGEON_Y][DUNGEON_X]; /* Hardness map for entire dungeon */
    int numrooms, nummon, numitems;
    int level;
    int view_mode;
    long seed; 
    heap_t event_heap;
    pair_t lcoords;                         /* Coordinates for look mode */
    room_t *rooms;
    terrain_t map[DUNGEON_Y][DUNGEON_X];    /* Terrain map */
    character *char_grid[DUNGEON_Y][DUNGEON_X];
    item *item_grid[DUNGEON_Y][DUNGEON_X];
    /* Maps for monster pathfinding. 
     * At some point at one more 
     * will be needed for PASS_WALL monsters.*/
    corridor_path_t tunnel_map[DUNGEON_Y][DUNGEON_X]; 
    corridor_path_t non_tunnel_map[DUNGEON_Y][DUNGEON_X];
    pc *player;
    string disp_msg;
    string level_msg;
    abstractDisplay *display;               /* Display object for drawing to screen */

    /* Methods */
    dungeonclass();
    ~dungeonclass();
    int init();
    int init_load(const char *path);
    int run();
    void printMonster(character *cp);
    void printItems(item *ip);
    void setupDisplay();
    void create_monsters();
    void createItems();
    int mv_up_stairs();
    int mv_dwn_stairs();
    int smooth_hardness();
    void empty_dungeon();
    void make_rooms();
    void fill_rooms();
    int place_stairs();
    int create_cycle();
    void connect_rooms();
    void print();
    void load_dungeon();
    int in_room(int16_t y, int16_t x);
    int read_from_file(const char *path);
    void save_to_file(const char* path);
    void delete_dungeon();
    void init_dungeon(uint8_t load);
    int end_game(int mode);
    void print_to_term();
};

#endif
