#include "pc.h"
#include "character_util.h"
#include "pathfinding.h"
#include "character.h"
#include "npc.h"

_pc *pc_init(_dungeon *d, uint8_t loaded, pair_t pc_loc) {
  _pc *p;
  if((p = malloc(sizeof(_pc)))) {
    memset(p, 0, sizeof(_pc));
    p->speed = 10;

    if(loaded == 1) {
      p->x = pc_loc[dim_x];
      p->y = pc_loc[dim_y];
    }
    else {
      p->x = rand_range(d->rooms[0].x + 1,
                                 d->rooms[0].length - 1 + d->rooms[0].x);
      p->y = rand_range(d->rooms[0].y + 1,
                                 d->rooms[0].height - 1 + d->rooms[0].y);
    }
  }
  return p;
}

void delete_pc(_pc *p) {
  if(p) {
    free(p);
  } 
}

int move_pc(_dungeon *d, int32_t move) {
  pair_t next;
  if((move == MV_UP_LEFT_1 || move == MV_UP_LEFT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player->x - 1;
    next[dim_y] = d->player->y - 1;
  }

  else if(move == MV_UP_1 || move == MV_UP_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = d->player->x;
      next[dim_y] = d->player->y - 1;
    }

    else {
      d->lcoords[dim_y] = (d->lcoords[dim_y] >= 19 ?
          d->lcoords[dim_y] - 10 : 9);
      return 1;
    }
  }

  else if((move == MV_UP_RIGHT_1 || move == MV_UP_RIGHT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player->x + 1;
    next[dim_y] = d->player->y - 1;
  }

  else if(move == MV_RIGHT_1 || move == MV_RIGHT_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = d->player->x + 1;
      next[dim_y] = d->player->y;
    }

    else {
      d->lcoords[dim_x] = (d->lcoords[dim_x] <= 99 ?
          d->lcoords[dim_x] + 20: 119);
      return 1;
    }
  }

  else if((move == MV_DWN_RIGHT_1 || move == MV_DWN_RIGHT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player->x + 1;
    next[dim_y] = d->player->y + 1;
  }

  else if(move == MV_DWN_1 || move == MV_DWN_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = d->player->x;
      next[dim_y] = d->player->y + 1;
    }

    else {
      d->lcoords[dim_y] = (d->lcoords[dim_y] <= 83 ?
          d->lcoords[dim_y] + 10: 93);
      return 1;
    }
  }

  else if((move == MV_DWN_LEFT_1 || move == MV_DWN_LEFT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player->x - 1;
    next[dim_y] = d->player->y + 1;
  }

  else if(move == MV_LEFT_1 || move == MV_LEFT_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = d->player->x - 1;
      next[dim_y] = d->player->y;
    }

    else {
      d->lcoords[dim_x] = (d->lcoords[dim_x] >= 59 ?
          d->lcoords[dim_x] - 20: 39);
      return 1;
    }
  }

  else if((move == REST_1|| move == REST_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = d->player->x;
    next[dim_y] = d->player->y;
  }

  else if(move == MV_UP_STAIRS) {
    return mv_up_stairs(d);
  }

  else if(move == MV_DWN_STAIRS) {
    return mv_dwn_stairs(d);
  }

  else if(move == LOOK_MODE) {
    if(d->view_mode != LOOK_MODE) {
      d->view_mode = LOOK_MODE;
      if(d->lcoords[dim_y] < 9) d->lcoords[dim_y] = 9;
      else if(d->lcoords[dim_y] > 93) d->lcoords[dim_y] = 93;
      if(d->lcoords[dim_x] > 119) d->lcoords[dim_x] = 119;
      else if(d->lcoords[dim_x] < 39) d->lcoords[dim_x] = 39;
    }
    return 1;
  }

  else if(move == CONTROL_MODE) {
    if(d->view_mode != CONTROL_MODE) {
      d->view_mode = CONTROL_MODE;
      d->lcoords[dim_x] = d->player->x;
      d->lcoords[dim_y] = d->player->y;
      print(d);
    }
    return 1;
  }

  else if(move == QUIT_GAME) {
    return end_game(d, BOTH_MODE);
  }

  else {
    return 1;
  }

  if(hardnesspair(next) == 0) {
    attack_pos(d, d->player->x, d->player->y, next, PC_MODE);
    d->player->x = next[dim_x];
    d->player->y = next[dim_y];
    _npc m;
    check_cur_room(d, &m, PC_MODE);
    pathfinding(d, d->player->x, d->player->y, d->tunnel_map, TUNNEL_MODE);
    pathfinding(d, d->player->x, d->player->y, d->non_tunnel_map, NON_TUNNEL_MODE);

   return 0;
  }

  else {
    return 1;
  }
}

