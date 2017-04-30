#ifndef DIMENSIONS_H
#define DIMENSIONS_H

#include <stdint.h>

#define NW 0
#define N 1
#define NE 2
#define E 3 
#define SE 4 
#define S 5 
#define SW 6 
#define W 7 

/* --from Dr Sheaffer-- */
typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef int16_t pair_t[num_dims];

#endif
