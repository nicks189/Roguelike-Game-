#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "dungeon.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
typedef void pc;
typedef void npc;
typedef void character;
#endif

#define TUNNEL_MODE 0
#define NON_TUNNEL_MODE 1
#define BOTH_MODE 2

int32_t weigh_hardness(uint8_t h);
void pathfinding(_dungeon *d, uint8_t x_to, uint8_t y_to, uint8_t mode);
void print_maps(_dungeon *d, uint8_t l);
void dijkstra_corridor(_dungeon *d, pair_t from, pair_t to);
void dijkstra_corridor_inv(_dungeon *d, pair_t from, pair_t to);
int32_t corridor_path_cmp(const void *key, const void *with);

#ifdef __cplusplus
}
#endif

#endif
