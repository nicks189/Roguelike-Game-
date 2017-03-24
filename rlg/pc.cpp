#include "pc.h"
#include "character_util.h"
#include "pathfinding.h"
#include "dungeon.h"

pc::pc(_dungeon *dun, int16_t *loc) {
  trait = 0;
  next[dim_x] = 0;
  next[dim_y] = 0;
  dead = 0;
  d = dun;
  isPc = 1;
  speed = 10;
  x = loc[dim_x];
  y = loc[dim_y];
  prev[dim_x] = x;
  prev[dim_y] = y;
  initMap();
  updateMap();
}

pc::pc(_dungeon *dun) {
  d = dun;
  trait = 0;
  isPc = 1;
  speed = 10;
  x = rand_range(d->rooms[0].x + 1,
                             d->rooms[0].length - 1 + d->rooms[0].x);
  y = rand_range(d->rooms[0].y + 1,
                             d->rooms[0].height - 1 + d->rooms[0].y);
  dead = 0;
  next[dim_x] = 0;
  next[dim_y] = 0;
  prev[dim_x] = x;
  prev[dim_y] = y;
  initMap();
  updateMap();
}

int pc::move(int move) {
  if((move == MV_UP_LEFT_1 || move == MV_UP_LEFT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x - 1;
    next[dim_y] = y - 1;
  }

  else if(move == MV_UP_1 || move == MV_UP_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = x;
      next[dim_y] = y - 1;
    }

    else {
      d->lcoords[dim_y] = (d->lcoords[dim_y] >= 19 ?
          d->lcoords[dim_y] - 10 : 9);
      return 1;
    }
  }

  else if((move == MV_UP_RIGHT_1 || move == MV_UP_RIGHT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x + 1;
    next[dim_y] = y - 1;
  }

  else if(move == MV_RIGHT_1 || move == MV_RIGHT_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = x + 1;
      next[dim_y] = y;
    }

    else {
      d->lcoords[dim_x] = (d->lcoords[dim_x] <= 99 ?
          d->lcoords[dim_x] + 20: 119);
      return 1;
    }
  }

  else if((move == MV_DWN_RIGHT_1 || move == MV_DWN_RIGHT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x + 1;
    next[dim_y] = y + 1;
  }

  else if(move == MV_DWN_1 || move == MV_DWN_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = x;
      next[dim_y] = y + 1;
    }

    else {
      d->lcoords[dim_y] = (d->lcoords[dim_y] <= 83 ?
          d->lcoords[dim_y] + 10: 93);
      return 1;
    }
  }

  else if((move == MV_DWN_LEFT_1 || move == MV_DWN_LEFT_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x - 1;
    next[dim_y] = y + 1;
  }

  else if(move == MV_LEFT_1 || move == MV_LEFT_2) {
    if(d->view_mode == CONTROL_MODE) {
      next[dim_x] = x - 1;
      next[dim_y] = y;
    }

    else {
      d->lcoords[dim_x] = (d->lcoords[dim_x] >= 59 ?
          d->lcoords[dim_x] - 20: 39);
      return 1;
    }
  }

  else if((move == REST_1|| move == REST_2) && d->view_mode == CONTROL_MODE) {
    next[dim_x] = x;
    next[dim_y] = y;
    return 0;
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
      d->lcoords[dim_x] = x;
      d->lcoords[dim_y] = y;
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
    attackPos();
    x = next[dim_x];
    y = next[dim_y];
    updateMap();
    return 0;
  }

  else {
    return 1;
  }
}

void pc::updateMap() {
  for(int j = y - 5; j < y + 6; j++) {
    for(int i = x - 5; i < x + 6; i++) {
      if(j < DUNGEON_Y && j >= 0 && i < DUNGEON_X && i >= 0) {
        pcmap[j][i] = mapxy(i, j); 
      }
    }
  }  
}

void pc::initMap() {
  for(int j = 0; j < DUNGEON_Y; j++) {
    for(int i = 0; i < DUNGEON_X; i++) {
      pcmap[j][i] = ter_wall; 
    }
  } 
}
