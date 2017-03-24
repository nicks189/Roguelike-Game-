#include "event.h"

int32_t event_cmp(const void *key, const void *with) {
  return ((event_t *) key)->time - ((event_t *) with)->time;
}

event_t *init_pc_event(character *p) {
  event_t *e;
  if((e = (event_t *) malloc(sizeof(event_t)))) {
    memset(e, 0, sizeof(event_t));
    e->type = pc_type;
    e->c = p;
  }
  return e;
}

event_t *init_npc_event(character *m, int event_sequence) {
  event_t *e;
  if((e = (event_t*) malloc(sizeof(event_t)))) {
    memset(e, 0, sizeof(event_t));
    e->type = npc_type;
    e->sequence = event_sequence;
    e->c = m;
  } 
  return e;
}

void delete_event(void *e) {
  event_t *event = (event_t *) e;
  if(event) {
    free(event);
  }
}
