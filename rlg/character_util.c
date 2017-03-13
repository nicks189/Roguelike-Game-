#include "character_util.h"
#include "pathfinding.h"
#include "event.h"

int32_t event_cmp(const void *key, const void *with) {
  return ((event_t *) key)->time - ((event_t *) with)->time;
}

