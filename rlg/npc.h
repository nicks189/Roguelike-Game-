#ifndef NPC_H
#define NPC_H

#include <stdint.h>

#include "dimensions.h"
#include "character.h"
#include "item.h"

#define NPC_SMART 0x00000001
#define NPC_TELEPATH 0x00000002
#define NPC_TUNNEL 0x00000004
#define NPC_ERRATIC 0x00000008
#define NPC_PASS_WALL 0x00000010
#define NPC_DESTROY_ITEM 0x00000020
#define NPC_PICKUP_ITEM 0x00000040

#define VISUAL_RANGE 20

typedef struct dungeon _dungeon;

class npc : public character {
  private:
    pair_t pc_lsp;
    int search_dir;
    item *npcItem;
    bool checkShortestPath(int16_t *pos, bool pcLos);
  public:
    npc(_dungeon *dun);
    npc(npc &n);
    ~npc() { 
      if(npcItem) {
        delete npcItem;
        npcItem = nullptr;
      }
    }
    void search();
    void setItem(item *t) { npcItem = t; }
    int move();
    int attackPos();
    item *getItem() { return npcItem; }
  protected:
    int searchHelper(int16_t *p);
    int checkLos();
    int isOpen(int xt, int yt); 
};

#endif 
