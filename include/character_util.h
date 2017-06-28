/* This file was used to allow C and C++ to be
 * used together in the same program and can
 * therefore be removed. It will need to be modified
 * to be used again.
 */

#ifndef CHARACTER_UTIL_H
#define CHARACTER_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dungeon.h"


void *pc_init(_dungeon *d, uint8_t loaded, int16_t *loc);
void *npc_init(_dungeon *d);
void delete_pc(void *pt);
void delete_npc(void *nt);
void update_pc_map(void *pt);
int get_characterx(void *c);
int get_charactery(void *c);
int move_pc(void *pt, int m);
int move_npc(void *nt);
char get_type(void *nt);
int is_dead(void *ct);
int get_speed(void *ct);
terrain_t get_pc_map(int xt, int yt, void *pt);

#ifdef __cplusplus
}
#endif

#endif
