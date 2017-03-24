#ifndef NPC_H
#define NPC_H

#include <stdint.h>

#include "dimensions.h"
#include "character.h"

#define NPC_SMART 0x00000001
#define NPC_TELEPATH 0x00000002
#define NPC_TUNNEL 0x00000004
#define NPC_ERRATIC 0x00000008
#define VISUAL_RANGE 20
#define NW 0
#define N 1
#define NE 2
#define E 3 
#define SE 4 
#define S 5 
#define SW 6 
#define W 7 

typedef struct dungeon _dungeon;

class npc : public character {
  private:
    pair_t pc_lsp;
    int search_dir;
  public:
    npc(_dungeon *dun);
    virtual ~npc() {}
    void search();
    int move();
  protected:
    int searchHelper(int16_t *p);
    int checkLos();
    inline int isOpen(int xt, int yt); 
};

#endif 
