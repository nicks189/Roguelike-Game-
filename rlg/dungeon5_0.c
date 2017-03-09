#include "dungeon.h"
#include "pathfinding.h"
#include "character.h"
#include "character_util.h"

/* --from Dr Sheaffer-- */
typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

/* --from Dr. Sheaffer-- */
int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};

void mount_ncurses(void) {
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, true);
  start_color();
  init_pair(1, COLOR_RED,     COLOR_BLACK);
  init_pair(2, COLOR_GREEN,   COLOR_BLACK);
  init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
  init_pair(4, COLOR_BLUE,    COLOR_BLACK);
  init_pair(5, COLOR_CYAN,    COLOR_BLACK);
  init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(7, COLOR_WHITE,   COLOR_BLACK);
}

void unmount_ncurses(void) {
  endwin();
}

int mv_up_stairs(_dungeon *d) {
  if(mapxy(d->player.x, d->player.y) == ter_stairs_up) {
    d->seed = time(NULL);
    delete_dungeon(d);
    d->nummon = rand_range(18, 25);
    create_dungeon(d, 0, 0, 0, 0, 0, 0);
  }
  return 1;
}

int mv_dwn_stairs(_dungeon *d) {
  if(mapxy(d->player.x, d->player.y) == ter_stairs_down) {
    d->seed = time(NULL);
    delete_dungeon(d);
    d->nummon = rand_range(18, 25);
    create_dungeon(d, 0, 0, 0, 0, 0, 0);
  }
  return 1;
}

int end_game(_dungeon *d, int mode) {
  clear();
  delete_dungeon(d);

  if(mode == NPC_MODE) {
    mvprintw(11, 31, "GAME OVER");
  }
  else if(mode == PC_MODE) {
    mvprintw(11, 32, "YOU WIN!");
  }
  else {
    mvprintw(11, 36, "QUIT");
  }

  getch();
  refresh();
  unmount_ncurses();
  exit(0);
  return 1;
}

/* --from Dr sheaffer */
int smooth_hardness(_dungeon *d) {
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  tail = head = NULL;

  memset(&hardness, 0, sizeof (hardness));

  /* Seed with some values */
  for (i = 1; i < 255; i += 5) {
    do {
      x = rand() % DUNGEON_X;
      y = rand() % DUNGEON_Y;
    } while (hardness[y][x]);
    hardness[y][x] = i;
    if (i == 1) {
      head = tail = malloc(sizeof (*tail));
    } else {
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = hardness[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !hardness[y - 1][x - 1]) {
      hardness[y - 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !hardness[y][x - 1]) {
      hardness[y][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < DUNGEON_Y && !hardness[y + 1][x - 1]) {
      hardness[y + 1][x - 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !hardness[y - 1][x]) {
      hardness[y - 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < DUNGEON_Y && !hardness[y + 1][x]) {
      hardness[y + 1][x] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < DUNGEON_X && y - 1 >= 0 && !hardness[y - 1][x + 1]) {
      hardness[y - 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < DUNGEON_X && !hardness[y][x + 1]) {
      hardness[y][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < DUNGEON_X && y + 1 < DUNGEON_Y && !hardness[y + 1][x + 1]) {
      hardness[y + 1][x + 1] = i;
      tail->next = malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < DUNGEON_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < DUNGEON_X) {
            s += gaussian[p][q];
            t += hardness[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      d->hardness[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < DUNGEON_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < DUNGEON_X) {
            s += gaussian[p][q];
            t += hardness[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      d->hardness[y][x] = t / s;
    }
  }

  return 0;
}

/* --from Dr Sheaffer-- */
void empty_dungeon(_dungeon *d) {
  uint8_t x, y;

  smooth_hardness(d);
  for(y = 0; y < DUNGEON_Y; y++) {
    for(x = 0; x < DUNGEON_X; x++) {
      mapxy(x, y) = ter_wall;
      if(y == 0 || y == DUNGEON_Y - 1 || x == 0 || x == DUNGEON_X - 1) {
        mapxy(x, y) = ter_wall_immutable;
        hardnessxy(x, y) = 255;
      }
    }
  }
}

/* --from Dr Sheaffer-- */
int in_room(_dungeon *d, int16_t y, int16_t x)
{
  int i;

  for (i = 0; i < d->num_rooms; i++) {
    if ((x >= d->rooms[i].x) &&
        (x < (d->rooms[i].y + d->rooms[i].length)) &&
        (y >= d->rooms[i].y) &&
        (y < (d->rooms[i].y + d->rooms[i].height))) {
          return 1;
    }
  }

  return 0;
}

void make_rooms(_dungeon *d) {
  for(uint32_t i = 0; i < d->num_rooms; i++) {
    while(1) {
      uint8_t bool_tmp = 1;
      _room r = {rand_range(1, DUNGEON_X - 9), rand_range(1, DUNGEON_Y - 7),
           rand_range(ROOM_MIN_X, ROOM_MAX_X), rand_range(ROOM_MIN_Y, ROOM_MAX_Y)};

      /* end points of new room */
      if(r.x + r.length >= DUNGEON_X) r.length = (DUNGEON_X - 2) - r.x;
      uint8_t r_x2 = r.x + r.length;
      if(r.y + r.height >= DUNGEON_Y) r.height = (DUNGEON_Y - 2) - r.y;
      uint8_t r_y2 = r.y + r.height;
      if(r.length == r.height) bool_tmp = 0;
      for(int j = 0; j < i; j++) {
        uint8_t a_x2 = d->rooms[j].x + d->rooms[j].length;
        uint8_t a_y2 = d->rooms[j].y + d->rooms[j].height;
        uint8_t a_x1 = d->rooms[j].x;
        uint8_t a_y1 = d->rooms[j].y;

        /* compare locations of arrays */
        if((r.x <= a_x2 && r_x2 >= a_x1) || (r_x2 <= a_x2 && r.x >= a_x1)) {
          if((r.y <= a_y2 && r_y2 >= a_y1) || (r_y2 <= a_y2 && r.y >= a_y1)) {
            bool_tmp = 0;
            break;
          }
        }
      }
      if(bool_tmp == 1) {
        d->rooms[i] = r;
        break;
      }
    } }
}

/* fill rooms with "." */
void fill_rooms(_dungeon *d) {
  for(uint32_t i = 0; i < d->num_rooms; i++) {
    uint8_t y1 = d->rooms[i].y;
    uint8_t x1 = d->rooms[i].x;
    uint8_t y2 = d->rooms[i].y + d->rooms[i].height;
    uint8_t x2 = d->rooms[i].x + d->rooms[i].length;
    for(uint8_t y = y1; y < y2; y++) {
      for(uint8_t x = x1; x < x2; x++) {
        mapxy(x, y) = ter_floor_room;
        hardnessxy(x, y) = 0;
      }
    }
  }
}

/* --from Dr Sheaffer-- */
int create_cycle(_dungeon *d)
{
  /* Find the (approximately) farthest two rooms, then connect *
   * them by the shortest path using inverted hardnesses.      */

  int32_t max, tmp, i, j, p, q;
  pair_t e1, e2;
  p = q = 0;

  for (i = max = 0; i < d->num_rooms - 1; i++) {
    for (j = i + 1; j < d->num_rooms; j++) {
      tmp = (((d->rooms[i].x - d->rooms[j].x)  *
              (d->rooms[i].x - d->rooms[j].x)) +
             ((d->rooms[i].y - d->rooms[j].y)  *
              (d->rooms[i].y - d->rooms[j].y)));
      if (tmp > max) {
        max = tmp;
        p = i;
        q = j;
      }
    }
  }

  e1[dim_y] = rand_range(d->rooms[p].y,
                         (d->rooms[p].y +
                          d->rooms[p].height - 1));
  e1[dim_x] = rand_range(d->rooms[p].x,
                         (d->rooms[p].x +
                          d->rooms[p].length - 1));
  e2[dim_y] = rand_range(d->rooms[q].y,
                         (d->rooms[q].y +
                          d->rooms[q].height - 1));
  e2[dim_x] = rand_range(d->rooms[q].x,
                         (d->rooms[q].x +
                          d->rooms[q].length - 1));

  dijkstra_corridor_inv(d, e1, e2);

  return 0;
}

void connect_rooms(_dungeon *d) {
  for(uint32_t i = 0; i < d->num_rooms - 1; i++) {
    pair_t e1, e2;

    e1[dim_x] = rand_range(d->rooms[i].x, d->rooms[i].length - 1 + d->rooms[i].x);
    e1[dim_y] = rand_range(d->rooms[i].y, d->rooms[i].height - 1 + d->rooms[i].y);
    e2[dim_x] = rand_range(d->rooms[i+1].x, d->rooms[i+1].length - 1 + d->rooms[i+1].x);
    e2[dim_y] = rand_range(d->rooms[i+1].y, d->rooms[i+1].height - 1 + d->rooms[i+1].y);

    dijkstra_corridor(d, e1, e2);

  }

  create_cycle(d);
}

int place_stairs(_dungeon *d) {
  int xd1, xd2, yd1, yd2, xu1, xu2, xu3, yu1, yu2, yu3;
  xd1 = rand_range(d->rooms[1].x + 1, d->rooms[1].length - 2 + d->rooms[1].x);
  xd2 = rand_range(d->rooms[2].x + 1, d->rooms[2].length - 2 + d->rooms[2].x);
  yd1 = rand_range(d->rooms[1].y + 1, d->rooms[1].height - 2 + d->rooms[1].y);
  yd2 = rand_range(d->rooms[2].y + 1, d->rooms[2].height - 2 + d->rooms[2].y);
  xu1 = rand_range(d->rooms[3].x + 1, d->rooms[3].length - 2 + d->rooms[3].x);
  xu2 = rand_range(d->rooms[4].x + 1, d->rooms[4].length - 2 + d->rooms[4].x);
  yu1 = rand_range(d->rooms[3].y + 1, d->rooms[3].height - 2 + d->rooms[3].y);
  yu2 = rand_range(d->rooms[4].y + 1, d->rooms[4].height - 2 + d->rooms[4].y);
  xu3 = rand_range(d->rooms[5].x + 1, d->rooms[5].length - 2 + d->rooms[5].x);
  yu3 = rand_range(d->rooms[5].y + 1, d->rooms[5].height - 2 + d->rooms[5].y);

  mapxy(xd1, yd1) = ter_stairs_down;
  mapxy(xd2, yd2) = ter_stairs_down;
  mapxy(xu1, yu1) = ter_stairs_up;
  mapxy(xu2, yu2) = ter_stairs_up;
  mapxy(xu3, yu3) = ter_stairs_up;
  return 0;
}

/* --from Dr Sheaffer-- */
void print(_dungeon *d) {
  clear();

  mvprintw(0, 0, "%ld", d->seed);
  uint8_t x, y, x_l, y_l, x_h, y_h;
  uint8_t px, py;
  px = 0;
  py = 1;

  if(d->view_mode == LOOK_MODE) {
    x_l = d->lcoords[dim_x] - 39;
    x_h = d->lcoords[dim_x] + 41;
    y_l = d->lcoords[dim_y] - 9;
    y_h = d->lcoords[dim_y] + 12;
  }

  else {
    if(d->player.x >= 39) {
      if(d->player.x <= DUNGEON_X - 41) {
        x_l = d->player.x - 39;
        x_h = d->player.x + 41;
      }

      else {
        x_h = DUNGEON_X;
        x_l = 80;
      }
    }

    else {
      x_l = 0;
      x_h = 80;
    }

    if(d->player.y >= 9) {
      if(d->player.y <= DUNGEON_Y - 12) {
        y_l = d->player.y - 9;
        y_h = d->player.y + 12;
      }

      else {
        y_h = DUNGEON_Y;
        y_l = 84;
      }
    }

    else {
      y_l = 0;
      y_h = 21;
    }
  }

  mvprintw(0, 20, "PC at: (%d, %d)", d->player.x, d->player.y);
  mvprintw(0, 40, "viewing: (%d to %d) and (%d to %d)", x_l, x_h, y_l + 1, y_h);

  for(y = y_l; y < y_h; y++) {
    px = 0;
    for(x = x_l; x < x_h; x++) {

      if(x == d->player.x && y == d->player.y) {
        mvaddch(py, px, '@' | COLOR_PAIR(3));
      }

      else if(char_gridxy(x, y) != NULL && mapxy(x, y) != ter_wall_immutable) {
        attron(COLOR_PAIR(5));
        mvprintw(py, px, "%c", (char_gridxy(x, y)->type));
        attroff(COLOR_PAIR(5));
      }

      else {
        switch(mapxy(x, y)) {
          case ter_wall:
          case ter_wall_immutable:
            mvaddch(py, px, ' ' | COLOR_PAIR(7));
            break;
          case ter_floor:
          case ter_floor_room:
            mvaddch(py, px, '.' | COLOR_PAIR(7));
            break;
          case ter_floor_hall:
            mvaddch(py, px, '#' | COLOR_PAIR(7));
            break;
         case ter_stairs_down:
            mvaddch(py, px, '>' | COLOR_PAIR(1));
            break;
         case ter_stairs_up:
            mvaddch(py, px, '<' | COLOR_PAIR(1));
            break;
         case endgame_flag:
            mvaddch(py, px, 'X');
            break;
        }
      }
      px++;
    }
    py++;
    addch('\n');
  }
  attron(COLOR_PAIR(5));
  mvprintw(23, 0, "HEALTH: 100");
  attroff(COLOR_PAIR(5));
  refresh();
}

/* fills grid from loaded file */
void load_dungeon(_dungeon *d) {
  uint8_t x, y;
  for(y = 1; y < DUNGEON_Y - 1; y++) {
    for(x = 1; x < DUNGEON_X - 1; x++) {
      if(hardnessxy(x, y) == 0) {
        if(mapxy(x, y) == ter_floor_room) mapxy(x, y) = ter_floor_room;
        else mapxy(x, y) = ter_floor_hall;
      } else {
        mapxy(x, y) = ter_wall;
      }
    }
  }
}

int read_from_file(_dungeon *d, char *path) {
  d->num_rooms = 0;
  FILE *fp;

  if(!(fp = fopen(path, "rb"))) {
    fprintf(stderr, "no such file\n");
    return 1;
  }

  uint8_t f_marker[12];
  uint32_t f_version;
  uint32_t f_size;


  if(!fread(f_marker, sizeof(f_marker), 1, fp)) return 1;;
  if(!fread(&f_version, sizeof(int), 1, fp)) return 1;
  if(!fread(&f_size, sizeof(int), 1, fp)) return 1;
  if(!fread(d->hardness, sizeof(d->hardness), 1, fp)) return 1;

  /* converts to little-endian if needed */
  int e_test = 1;
  if(*(char *) &e_test == 1) {
    f_size = be32toh(f_size);
    f_version = be32toh(f_version);
  }

  /* reads 4 bytes at a time into rooms_temp, then is stored in d->rooms */
  uint8_t rooms_temp[4];
  uint32_t index = 0;
  while(fread(rooms_temp, sizeof(rooms_temp), 1, fp) == 1) {
    if(d->num_rooms >= MAX_ROOMS) {
      d->rooms = realloc(d->rooms, sizeof(d->rooms) * d->num_rooms);
    }
    d->rooms[index].x = rooms_temp[0];
    d->rooms[index].y = rooms_temp[1];
    d->rooms[index].length = rooms_temp[2];
    d->rooms[index].height = rooms_temp[3];
    index++;
    d->num_rooms++;
  }

  fclose(fp);
  printf("Loaded from %s\n", path);
  return 0;
}

void save_to_file(_dungeon *d, char* path) {
  FILE *fp;

  fp = fopen(path, "wb");
  unsigned char f_marker[12] = {'R','L','G','3','2','7','-','S','2','0','1','7'};
  uint32_t f_version = 0;
  uint32_t f_size = 12 + 4 + 4 + 160 * 105 + 4 * d->num_rooms;
  uint32_t i;

  /* check endianess */
  int e_test = 1;
  if(*(char *) &e_test == 1) {
    f_version = htobe32(f_version);
    f_size = htobe32(f_size);
  }

  fwrite(f_marker, sizeof(f_marker), 1, fp);
  fwrite(&f_version, sizeof(int32_t), 1, fp);
  fwrite(&f_size, sizeof(int32_t), 1, fp);
  fwrite(d->hardness, sizeof(uint8_t), 160 * 105, fp);

  for(i = 0; i < d->num_rooms; i++) {
    uint8_t to_mem[4];
    to_mem[0] = d->rooms[i].x;
    to_mem[1] = d->rooms[i].y;
    to_mem[2] = d->rooms[i].length;
    to_mem[3] = d->rooms[i].height;

    fwrite(to_mem, sizeof(to_mem), 1, fp);
  }

  fclose(fp);
}

void delete_dungeon(_dungeon *d) {
  int i, j;
  for(i = 0; i < d->nummon; i++) {
    d->npc_arr[i].curroom = 0;
    d->npc_arr[i].pc_lsp[dim_x] = 0;
    d->npc_arr[i].pc_lsp[dim_y] = 0;
    d->npc_arr[i].search_to[dim_x] = 0;
    d->npc_arr[i].search_to[dim_y] = 0;
    d->npc_arr[i].dead = 0;
    d->npc_arr[i].searching = 0;
    d->npc_arr[i].pc_los = 0;
    d->npc_arr[i].x = 0;
    d->npc_arr[i].y = 0;
    d->npc_arr[i].speed = 0;
    d->npc_arr[i].type = 0;
    d->npc_arr[i].trait = 0;
  }
  for(i = 0; i < d->num_rooms; i++) {
    d->rooms[i].x = d->rooms[i].y = 0;
    d->rooms[i].length = d->rooms[i].height = 0;
  }
  for(j = 0; j < DUNGEON_Y; j++) {
    for(i = 0; i < DUNGEON_X; i++) {
      d->char_grid[j][i] = NULL;
    }
  }

  free(d->npc_arr);
  free(d->rooms);
}

void init_dungeon(_dungeon *d, uint8_t load) {
  d->view_mode = CONTROL_MODE;
  if(load) {
    uint8_t x, y;
    for(y = 0; y < DUNGEON_Y; y++) {
      for(x = 0; x < DUNGEON_X; x++) {
        mapxy(x, y) = ter_wall;
      }
    }
  }
  empty_dungeon(d);
}

/* refreshes dungeon and moves characters */
uint8_t update_dungeon(_dungeon *d) {
  int32_t nxtcmd;
  uint32_t i;
  uint8_t initialized = 0;
  event_t *cur_node, *peek_node, *temp_node, event_array[d->nummon + 1];
  heap_t h;

  while(1) {

    d->lcoords[dim_x] = d->player.x;
    d->lcoords[dim_y] = d->player.y;
    if(!initialized) {
      heap_init(&h, event_cmp, NULL);
      event_array[0].time = 0;
      event_array[0].type = pc_type;
      event_array[0].u.pc = &d->player;
      event_array[0].sequence = 0;
      heap_insert(&h, &event_array[0]);

      for(i = 1; i < d->nummon + 1; i++) {
        event_array[i].time = 0;
        event_array[i].type = npc_type;
        event_array[i].u.npc = &d->npc_arr[i - 1];
        event_array[i].sequence = i;
        heap_insert(&h, &event_array[i]);
      }
      initialized = 1;
    }

    if(h.size != 1) {
      cur_node = heap_remove_min(&h);
    }

    else {
      print(d);
      heap_delete(&h);
      return end_game(d, PC_MODE);
    }

    while((peek_node = heap_peek_min(&h))) {
      if(peek_node->time == cur_node->time && peek_node->sequence < cur_node->sequence) {
        temp_node = heap_remove_min(&h);
        heap_insert(&h, cur_node);
        cur_node = temp_node;
      }
      else {
        break;
      }
    }

    if(cur_node->type == pc_type) {
      print(d);
      cur_node->time += (1000 / cur_node->u.pc->speed);
      while(move_pc(d, nxtcmd = getch())) {
        if(d->view_mode == LOOK_MODE) {
          print(d);
        }
      }
      heap_insert(&h, cur_node);
    }

    else {
      if(!cur_node->u.npc->dead) {
        cur_node->time += (1000 / cur_node->u.npc->speed);
        if(move_npc(d, cur_node->u.npc)) {
          heap_delete(&h);
          return end_game(d, NPC_MODE);
        }
        heap_insert(&h, cur_node);
      }
    }

  }
  return 0;
}

int create_dungeon(_dungeon *d, uint8_t load,
   uint8_t save, char *rpath, char* spath, uint8_t pc_loaded, pair_t pc_loc) {

  if(load) {
    init_dungeon(d, load);
    if(read_from_file(d, rpath)) {
      return 1;
    }

    fill_rooms(d);
    load_dungeon(d);
    place_stairs(d);
    place_pc(d, pc_loaded, pc_loc);
    pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
    pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
    init_monsters(d);
    print(d);
    print_maps(d, 0);
    return update_dungeon(d);
  }

  else {
    d->num_rooms = rand_range(MIN_ROOMS, MAX_ROOMS);
    d->rooms = malloc(sizeof(*d->rooms) * d->num_rooms);
    d->npc_arr = malloc(sizeof(_npc) * d->nummon);
    pc_loaded = 0;
    init_dungeon(d, load);
    make_rooms(d);
    fill_rooms(d);
    connect_rooms(d);
    place_stairs(d);
    place_pc(d, pc_loaded, pc_loc);
    pathfinding(d, d->player.x, d->player.y, d->tunnel_map, TUNNEL_MODE);
    pathfinding(d, d->player.x, d->player.y, d->non_tunnel_map, NON_TUNNEL_MODE);
    init_monsters(d);
    print(d);
    print_maps(d, 0);
    return update_dungeon(d);
  }

  if(save) {
    save_to_file(d, spath);
   //printf("Saved to: %s\n", spath);
  }
  return 0;
}

int main(int argc, char *argv[]) {
  _dungeon d;
  d.seed = time(NULL);
  uint8_t _load, _save, isn_empty, pc_loaded;
  d.nummon = rand_range(18, 25);
  char file_name[255];
  pair_t pc_loc;
  _load = _save = isn_empty = pc_loaded = 0;
  pc_loc[dim_y] = pc_loc[dim_x] = 0;

  /* handles command line args */
  if(argc > 10) {
    fprintf(stderr, "wrong parameters\n");
    return 1;
  }

  for(uint8_t i = 1; i < argc; i++) {
    if(argv[i][0] == '-') {
      if(!(strcmp(argv[i], "--load")) || argv[i][1] == 'l') _load = 1;
      else if(!(strcmp(argv[i], "--save")) || argv[i][1] == 's') _save = 1;
      else if(!(strcmp(argv[i], "--pc")) || argv[i][1] == 'p') {
        if(i < argc - 1) {
          pc_loaded = 1;
          pc_loc[dim_y] = atoi(argv[++i]);
          pc_loc[dim_x] = atoi(argv[++i]);
        }
      }
      else if(!(strcmp(argv[i], "--rand")) || argv[i][1] == 'r') {
        if(argc < ++i + 1 || !sscanf(argv[i], "%ld", &d.seed)) {}
      }
      else if(!(strcmp(argv[i], "--nummon")) || argv[i][1] == 'm') {
        if(i < argc) {
          d.nummon = atoi(argv[++i]);
        }
      }
    }
    else {
        strcpy(file_name, argv[i]);
        isn_empty = 1;
    }
  }

  srand(d.seed);
  printf("Using seed: %ld\n", d.seed);

  /* file concatination */
  char r_path[255];
  strcpy(r_path, getenv("HOME"));
  strcat(r_path, "/.rlg327/");

  char s_path[255];
  strcpy(s_path, getenv("HOME"));
  strcat(s_path, "/.rlg327/dungeon");

  if(isn_empty == 1 && _load == 1) {
    strcat(r_path, file_name);
    printf("FILENAME %s\n", file_name);
  }
  else if(isn_empty == 1 && _load == 0) {
   strcat(r_path, "dungeon");
   printf("Didn't call load!");
  }
  else {
    strcat(r_path, "dungeon");
  }

  mount_ncurses();
  create_dungeon(&d, _load, _save, r_path, s_path, pc_loaded, pc_loc);
  delete_dungeon(&d);
  return 0;
}
