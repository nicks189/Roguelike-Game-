#ifndef PC_H
#define PC_h

#include "dungeon.h"

/* controls */
#define MV_UP_LEFT_1 067 
#define MV_UP_LEFT_2 0171 
#define MV_UP_1 070
#define MV_UP_2 0153 
#define MV_UP_RIGHT_1 071
#define MV_UP_RIGHT_2 0165 
#define MV_RIGHT_1 066
#define MV_RIGHT_2 0154 
#define MV_DWN_RIGHT_1 063
#define MV_DWN_RIGHT_2 0156 
#define MV_DWN_1 062
#define MV_DWN_2 0152
#define MV_DWN_LEFT_1 061
#define MV_DWN_LEFT_2 0142 
#define MV_LEFT_1 064
#define MV_LEFT_2 0150 
#define REST_1 065
#define REST_2 040 
#define MV_UP_STAIRS 074
#define MV_DWN_STAIRS 076
#define LOOK_MODE 0114
#define CONTROL_MODE 033
#define QUIT_GAME 0121

typedef struct pc {
  uint8_t x, y, speed;
  uint8_t curroom;
} _pc;

_pc *pc_init(_dungeon *d, uint8_t loaded, pair_t pc_loc); 
void delete_pc(_pc *p);
int move_pc(_dungeon *d, int32_t move);

#endif
