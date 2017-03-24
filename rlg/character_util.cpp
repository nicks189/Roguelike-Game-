#include "character_util.h"
#include "character.h"
#include "npc.h"
#include "pc.h"

#ifdef __cplusplus 
extern "C" {
#endif

void *pc_init(_dungeon *d, uint8_t loaded, int16_t *loc) {
  pc *p;
  if(loaded) {
    p = new pc(d, loc); 
  }
  else {
    p = new pc(d);
  }
  return (void *) p;
}

void *npc_init(_dungeon *d) {
  npc *n = new npc(d);
  return (void *) n;
}

void delete_pc(void *pt) {
  delete (pc*) pt;
}

void delete_npc(void *nt) {
  delete (npc *) nt;
}

int get_characterx(void *c) {
  character *ch = (character *) c;
  return ch->getX();
}

void update_pc_map(void *pt) {
  pc *p = (pc *) pt;
  p->updateMap();
}

int get_charactery(void *c) {
  character *ch = (character *) c;
  return ch->getY();
}

int move_pc(void *pt, int m) {
  pc *p = (pc *) pt;
  return p->move(m);
}

int move_npc(void *nt) {
  npc *n = (npc *) nt;
  return n->move();
}

char get_type(void *nt) {
  npc *n = (npc *) nt;
  return n->getType();
}

int is_dead(void *ct) {
  character *c = (character *) ct;
  return c->isDead();
}

int get_speed(void *ct) {
  character *c = (character *) ct;
  return c->getSpeed();
}

terrain_t get_pc_map(int xt, int yt, void *pt) {
  pc *p = (pc *) pt;
  return p->pcmap[yt][xt];
}

#ifdef __cplusplus
}
#endif
