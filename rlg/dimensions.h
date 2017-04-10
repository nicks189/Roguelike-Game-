#ifndef DIMENSIONS_H
#define DIMENSIONS_H

#include <stdint.h>

/* --from Dr Sheaffer-- */
typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef int16_t pair_t[num_dims];

#endif
