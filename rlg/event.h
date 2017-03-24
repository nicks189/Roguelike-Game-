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
  character *c;
} event_t;

int32_t event_cmp(const void *key, const void *with);
event_t *init_pc_event(character *p);
event_t *init_npc_event(character *m, int event_sequence);
void delete_event(void *e);

#endif
