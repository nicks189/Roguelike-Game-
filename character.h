#ifndef CHARACTER_H
#define CHARACTER_H

#include "dungeon.h" 

void place_pc(_dungeon *d, uint8_t loaded, pair_t pc_loc); 
void move_pc(_dungeon *d);
uint8_t move_npc(_dungeon *d, _npc *mon); 
void init_monsters(_dungeon *d);

#endif
