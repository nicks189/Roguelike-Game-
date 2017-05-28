#ifndef TERRAIN_H
#define TERRAIN_H

/* --from Dr Sheaffer-- */
typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_wall,
  ter_wall_immutable,
  ter_border_south,
  ter_border_north,
  ter_border_east,
  ter_border_west,
  ter_floor,
  ter_floor_room,
  ter_floor_hall,
  ter_stairs_up,
  ter_stairs_down,
  endgame_flag
} terrain_t;

#endif
