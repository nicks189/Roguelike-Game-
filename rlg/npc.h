#ifndef NPC_H
#define NPC_H

#include "dungeon.h"

#define NPC_SMART 0x00000001 
#define NPC_TELEPATH 0x00000002
#define NPC_TUNNEL 0x00000004
#define NPC_ERRATIC 0x00000008
#define VISUAL_RANGE 20

typedef enum direction {
  NINIT, NW, N, NE, E, SE, S, SW, W 
} direction_t;

typedef struct npc {
  uint8_t x, y;
  uint8_t dead, curroom; 
  uint8_t pc_los, speed;
  uint32_t trait; 
  char type;
  pair_t pc_lsp;
  pair_t prev;
  direction_t search_dir;
} _npc;

_npc *npc_init(_dungeon *d);
void delete_npc(_npc *m);
int move_npc(_dungeon *d, _npc *mon);
void find_rand_neighbor(_dungeon *d, uint8_t x, uint8_t y, 
    uint8_t mode, int16_t *ret); 
void search_for_pc(_dungeon *d, _npc *mon, int16_t *ret); 
void check_los(_dungeon *d, _npc *m);
uint8_t add_mon(_dungeon *d, _npc *mon);

#endif 
