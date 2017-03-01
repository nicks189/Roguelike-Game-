#ifndef CHARACTER_H
#define CHARACTER_H

#include "dungeon.h" 

#define MV_UP_LEFT_1 067 
#define MV_UP_LEFT_2 171 
#define MV_UP_1 070
#define MV_UP_2 153 
#define MV_UP_RIGHT_1 071
#define MV_UP_RIGHT_2 165 
#define MV_RIGHT_1 066
#define MV_RIGHT_2 154 
#define MV_DWN_RIGHT_1 063
#define MV_DWN_RIGHT_2 156 
#define MV_DWN_1 062
#define MV_DWN_2 152
#define MV_DWN_LEFT_1 061
#define MV_DWN_LEFT_2 142 
#define MV_LEFT_1 064
#define MV_LEFT_2 150 
#define REST_1 065
#define REST_2 040 
#define MV_UP_STAIRS 074
#define MV_DWN_STAIRS 076
#define LOOK_MODE 114
#define CONTROL_MODE 033
#define QUIT_GAME 121

void place_pc(_dungeon *d, uint8_t loaded, pair_t pc_loc); 
void move_pc(_dungeon *d, int32_t move);
uint8_t move_npc(_dungeon *d, _npc *mon); 
void init_monsters(_dungeon *d);

#endif
