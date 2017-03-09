#ifndef CHARACTER_UTIL_H
#define CHARACTER_UTIL_H

#include "dungeon.h"

#define NPC_SMART 0x00000001 
#define NPC_TELEPATH 0x00000002
#define NPC_TUNNEL 0x00000004
#define NPC_ERRATIC 0x00000008
#define VISUAL_RANGE 20
#define PC_MODE 0
#define NPC_MODE 1

void init_char_array(_dungeon *d);
uint8_t handle_killing(_dungeon *d, uint8_t x_i, uint8_t y_i, int16_t *temp, uint8_t mode);
uint8_t add_mon(_dungeon *d, _npc *mon);
int32_t event_cmp(const void *key, const void *with);
uint8_t handle_events(_dungeon *d);
void find_rand_neighbor(_dungeon *d, uint8_t x, uint8_t y, uint8_t mode, int16_t *ret); 
void search_for_pc(_dungeon *d, _npc *mon, int16_t *ret); 
void check_cur_room(_dungeon *d, _npc *m, uint8_t mode);
void check_los(_dungeon *d, _npc *m);

#endif
