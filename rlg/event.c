#include "event.h"

event_t *init_pc_event(_pc *p) {
  event_t *e;
  if((e = malloc(sizeof(event_t)))) {
    memset(e, 0, sizeof(event_t));
    e->type = pc_type;
    e->u.pc = p;
  }
  return e;
}

event_t *init_npc_event(_npc *m, int event_sequence) {
  event_t *e;
  if((e = malloc(sizeof(event_t)))) {
    memset(e, 0, sizeof(event_t));
    e->type = npc_type;
    e->sequence = event_sequence;
    e->u.npc = m;
  } 
  return e;
}

void delete_event(void *e) {
  event_t *event = e;
  if(event) {
    free(event);
  }
}
