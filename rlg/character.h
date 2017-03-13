#ifndef CHARACTER_H
#define CHARACTER_H

#include "dungeon.h" 

#define PC_MODE 0
#define NPC_MODE 1

uint8_t attack_pos(_dungeon *d, uint8_t x_i, uint8_t y_i, int16_t *temp, uint8_t mode);
void check_cur_room(_dungeon *d, _npc *m, uint8_t mode);

#endif
