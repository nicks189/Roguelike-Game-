#ifndef EVENT_H
#define EVENT_H

#include "dungeon.h"

enum event_type {
  npc_type,
  pc_type
};

typedef struct event {
  uint32_t time;
  enum event_type type;
  uint32_t sequence;
  union {
    _pc *pc;
    _npc *npc;
  } u;
} event_t;

event_t *init_pc_event(_pc *p);
event_t *init_npc_event(_npc *m, int event_sequence);
void delete_event(void *e);

#endif
