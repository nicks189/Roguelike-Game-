#ifndef EVENT_H
#define EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

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
    void *pc;
    void *npc;
  } u;
} event_t;

int32_t event_cmp(const void *key, const void *with);
event_t *init_pc_event(void *p);
event_t *init_npc_event(void *m, int event_sequence);
void delete_event(void *e);

#ifdef __cplusplus 
}
#endif

#endif
