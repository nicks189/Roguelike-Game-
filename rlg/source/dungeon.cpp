#include <vector>

#include "../include/dungeon.h"
#include "../include/pathfinding.h"
#include "../include/event.h"
#include "../include/factory.h"
#include "../include/shop_keeper.h"

using std::vector;
using std::stringstream;

int dungeon_init(_dungeon *d) { 
  static int initialized = 0;
  srand(d->seed);

  if(d->level == 2) {
    string path = getenv("HOME");
    path.append("/.rlg327/boss01.rlg327");
    return dungeon_init_load(d, path.c_str());
  }

  d->num_rooms = rand_range(MIN_ROOMS, MAX_ROOMS);
  d->rooms = (room_t *) malloc(sizeof(*d->rooms) * d->num_rooms);

  init_dungeon(d, 0);

  make_rooms(d);
  fill_rooms(d);

  if(!initialized) {
    d->player = new pc(d);
    setupDisplay(d);
    initialized = 1;
  }

  else {
    d->player->place(d);  
  }

  heap_insert(&d->event_heap, init_pc_event(d->player));

  connect_rooms(d);
  place_stairs(d);

  pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
  pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);

  create_monsters(d, d->nummon > 0 ? d->nummon : 20);
  createItems(d, d->numitems > 0 ? d->numitems : 20);

  d->player->updateMap(); 

  if(d->level < 6) {
    d->level_msg = "It's very dark... you wonder how you got here.";
  }
  else if(d->level < 10) {
    d->level_msg = "You begin to shiver from the cold."; 
  }
  else if(d->level < 15) {
    d->level_msg = "You hear a strange noise...";
  }

  d->disp_msg = d->level_msg;

  d->display->displayMap();

  return 0;
}

int dungeon_init_load(_dungeon *d, const char *path) {
  init_dungeon(d, 0);

  d->rooms = (room_t *) malloc(sizeof(*d->rooms) * MAX_ROOMS);

  if(read_from_file(d, path)) {
    return 1;
  }

  if(d->level == 2) {
    d->level_msg = "You hear a strange noise...";
    d->nummon = 5;
    d->numitems = 8;
  }
  //else if(d->level < 6) {
  //  d->level_msg = "You begin to shiver from the cold."; 
  //}
  else {
    d->level_msg = "It's very dark... you wonder how you got here.";
  }
  
  fill_rooms(d);
  load_dungeon(d);
  place_stairs(d);

  d->player->place(d);  
  heap_insert(&d->event_heap, init_pc_event(d->player));

  pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
  pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);

  create_monsters(d, d->nummon > 0 ? d->nummon : 20);
  createItems(d, d->numitems > 0 ? d->numitems : 20);

  d->player->updateMap(); 

  d->disp_msg = d->level_msg;

  d->display->displayMap();

  return 0;
}

/* refreshes dungeon and moves characters until 
 * the game ends or user quits */
uint8_t run_dungeon(_dungeon *d) {
  event_t *cur_node, *peek_node, *temp_node;

  while(true) {
    d->lcoords[dim_x] = d->player->getX();
    d->lcoords[dim_y] = d->player->getY();
    if(d->event_heap.size != 1) {
      cur_node = (event_t *) heap_remove_min(&d->event_heap);
    }

    else {
      d->display->displayMap();
      heap_delete(&d->event_heap);
      return end_game(d, PC_MODE);
    }

    while((peek_node = (event_t *) heap_peek_min(&d->event_heap))) {
      if(peek_node->time == cur_node->time 
                         && peek_node->sequence < cur_node->sequence) {
        temp_node = (event_t *) heap_remove_min(&d->event_heap);
        heap_insert(&d->event_heap, cur_node);
        cur_node = temp_node;
      }
      else {
        break;
      }
    }

    if(cur_node->type == pc_type) {
      d->display->displayMap();
      cur_node->time += (1000 / cur_node->c->getSpeed());

      /* __TODO - Make getting command independant__ */
      while(d->player->move(getch())) {
        d->display->displayMap();
      }
      pathfinding(d, d->player->getX(), d->player->getY(), TUNNEL_MODE);
      pathfinding(d, d->player->getX(), d->player->getY(), NON_TUNNEL_MODE);
      heap_insert(&d->event_heap, cur_node);
    }

    else {
      if(!(cur_node->c->isDead())) {
        cur_node->time += (1000 / cur_node->c->getSpeed());
        if(cur_node->c->move()) {
          heap_delete(&d->event_heap);
          return end_game(d, NPC_MODE);
        }
        heap_insert(&d->event_heap, cur_node);
      }
      else {
        delete_event(cur_node);
      }
    }
  }
  return 0;
}

void setupDisplay(_dungeon *d) {
  d->display = new cursesDisplay(d);
}

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

void create_monsters(_dungeon *d, int num) {
  // if(d->nummon == 0) {
  //   d->nummon = 20;
  // }

  int i = 0;

  vector<character *> v;

  character *cp;
  characterFactory *factory = new characterFactory(d);

  /* Bad implementation but it assures monsters are random */
  while((cp = (character *) factory->generateNext())) {
    v.push_back(cp);
  }

  while(i < num) { 
    npc *n = (npc *) v[rand_range(0, v.size() - 1)];

    if(d->level > n->getLevel()) {
      cp = new npc(*n); 

      char_gridxy(cp->getX(), cp->getY()) = cp;
      heap_insert(&d->event_heap, init_npc_event(cp, i));
      i++;
    }
  }

  while(v.size() != 0) {
    cp = v.back();
    delete cp;
    v.pop_back();
  }

  delete factory;
}

void printMonster(character *cp) {
  cout << endl;
  cout << cp->getName() << endl;
  cout << cp->getDesc() << endl;
  cout << cp->getSymbol() << endl;
  cout << "Color: " << cp->getColor() << endl;
  cout << "Speed: " << cp->getSpeed() << endl;
  cout << "Traits: " << cp->getTraits() << endl;
  cout << "HP: " << cp->getHp() << endl;
  dice *d = cp->getDamage();
  cout << "Damage: " << d->toString() << endl;
}

void createItems(_dungeon *d, int num) {
  // if(d->numitems == 0) {
  //   d->numitems = 20;
  // }
  
  int i = 0;

  vector<item *> v;

  item *ip;
  itemFactory *factory = new itemFactory(d);

  /* Bad implementation but it assures items are random */
  while((ip = (item *) factory->generateNext())) {
    v.push_back(ip);
  }

  while(i < num) { 
    item *n = v[rand_range(0, v.size() - 1)];
    if(d->level > n->getLevel()) {
      ip = new item(*n); 
      d->item_grid[ip->getY()][ip->getX()] = ip;
      i++;
    }
  }

  while(v.size() != 0) {
    ip = v.back();
    delete ip;
    v.pop_back();
  }

  delete factory;
}

void printItem(item *ip) {
  dice *d = ip->getDamage();
  cout << endl;
  cout << ip->getName() << endl;
  cout << ip->getDesc() << endl;
  cout << "Color: " << ip->getColor() << endl;
  cout << "Hit bonus: " << ip->getHit() << endl;
  cout << "Damage bonus: " << d->toString() << endl;
  cout << "Dodge bonus: " << ip->getDodge() << endl;
  cout << "Defense bonus: " << ip->getDefense() << endl;
  cout << "Weight: " << ip->getWeight() << endl;
  cout << "Speed bonus: " << ip->getSpeed() << endl;
  cout << "Attribute: " << ip->getAttribute() << endl;
  cout << "Value: " << ip->getValue() << endl;
}

int mv_up_stairs(_dungeon *d) {
  d->level--;
  if(mapxy(d->player->getX(), d->player->getY()) == ter_stairs_up) {
    d->seed++;
    delete_dungeon(d);
    if(dungeon_init(d)) {
      end_game(d, ERROR_MODE);
    }
    d->display->displayMap();
    run_dungeon(d);
  }
  return 1;
}

int mv_dwn_stairs(_dungeon *d) {
  d->level++;
  if(mapxy(d->player->getX(), d->player->getY()) == ter_stairs_down) {
    d->seed++;
    delete_dungeon(d);
    if(dungeon_init(d)) {
      end_game(d, ERROR_MODE);
    }
    d->display->displayMap();
    run_dungeon(d);
  }
  return 1;
}

/* --from Dr sheaffer */
int smooth_hardness(_dungeon *d) {
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  uint8_t hardness[DUNGEON_Y][DUNGEON_X];
  tail = head = nullptr;

  memset(&hardness, 0, sizeof (hardness));

  /* Seed with some values */
  for (i = 1; i < 255; i += 5) {
    do {
      x = rand() % DUNGEON_X;
      y = rand() % DUNGEON_Y;
    } while (hardness[y][x]);
    hardness[y][x] = i;
    if (i == 1) {
      head = tail = (queue_node_t *) malloc(sizeof (*tail));
    } else {
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = nullptr;
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
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !hardness[y][x - 1]) {
      hardness[y][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < DUNGEON_Y && !hardness[y + 1][x - 1]) {
      hardness[y + 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !hardness[y - 1][x]) {
      hardness[y - 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < DUNGEON_Y && !hardness[y + 1][x]) {
      hardness[y + 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < DUNGEON_X && y - 1 >= 0 && !hardness[y - 1][x + 1]) {
      hardness[y - 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < DUNGEON_X && !hardness[y][x + 1]) {
      hardness[y][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < DUNGEON_X && y + 1 < DUNGEON_Y && !hardness[y + 1][x + 1]) {
      hardness[y + 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
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
  int32_t i;

  for(i = 0; i < d->num_rooms; i++) {
    while(1) {
      uint8_t bool_tmp = 1;
      room_t r = {rand_range(1, DUNGEON_X - 9), rand_range(1, DUNGEON_Y - 7),
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
    } 
  }
}

void fill_rooms(_dungeon *d) {
  int32_t i;
  int x, y;

  for(i = 0; i < d->num_rooms; i++) {
    uint8_t y1 = d->rooms[i].y;
    uint8_t x1 = d->rooms[i].x;
    uint8_t y2 = d->rooms[i].y + d->rooms[i].height;
    uint8_t x2 = d->rooms[i].x + d->rooms[i].length;

    for(y = y1; y < y2; y++) {
      if(mapxy(x1 - 1, y) != ter_wall_immutable)
        mapxy(x1 - 1, y) = ter_border_west;
      if(mapxy(x2, y) != ter_wall_immutable)
        mapxy(x2, y) = ter_border_east;
    }

    for(x = x1 - 1; x < x2 + 1; x++) {
      if(mapxy(x, y1 - 1) != ter_wall_immutable)
        mapxy(x, y1 - 1) = ter_border_north;
      if(mapxy(x, y2) != ter_wall_immutable)
        mapxy(x, y2) = ter_border_south;
    }

    for(y = y1; y < y2; y++) {
      for(x = x1; x < x2; x++) {
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
  int32_t i;
  for(i = 0; i < d->num_rooms - 1; i++) {
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

  if(d->level > 1) {
    mapxy(xd1, yd1) = ter_stairs_down;
    mapxy(xd2, yd2) = ter_stairs_down;
    mapxy(xu1, yu1) = ter_stairs_down;
    mapxy(xu2, yu2) = ter_stairs_up;
    mapxy(xu3, yu3) = ter_stairs_up;
  }
  else {
    mapxy(xd1, yd1) = ter_stairs_down;
    mapxy(xd2, yd2) = ter_stairs_down;
    mapxy(xu1, yu1) = ter_stairs_down;
    mapxy(xu2, yu2) = ter_stairs_down;
    mapxy(xu3, yu3) = ter_stairs_down;
  }

  return 0;
}

inline uint8_t in_los_range(_dungeon *d, int x, int y) {
  if(d->nofog) {
    return 1;
  }
  else if(abs(x - d->player->getX()) <= 5 && 
          (abs(y - d->player->getY()) <= 5)) {
    return 1;
  }
  return 0;
}

/* fills grid from loaded file */
void load_dungeon(_dungeon *d) {
  uint8_t x, y;

  for(y = 1; y < DUNGEON_Y - 1; y++) {
    for(x = 1; x < DUNGEON_X - 1; x++) {
      if(hardnessxy(x, y) == 0) {
        if(mapxy(x, y) == ter_floor_room);
        else mapxy(x, y) = ter_floor_hall;
      } 
      else if(mapxy(x, y) == ter_border_south);
      else if(mapxy(x, y) == ter_border_north);
      else if(mapxy(x, y) == ter_border_west);
      else if(mapxy(x, y) == ter_border_east);
      else {
        mapxy(x, y) = ter_wall;
      }
    }
  }
}

int read_from_file(_dungeon *d, const char *path) {
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
      d->rooms = (room_t *) realloc(d->rooms, sizeof(d->rooms) * d->num_rooms);
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

void save_to_file(_dungeon *d, const char* path) {
  FILE *fp;

  fp = fopen(path, "wb");
  unsigned char f_marker[12] = {'R','L','G','3','2','7','-','S','2','0','1','7'};
  uint32_t f_version = 0;
  uint32_t f_size = 12 + 4 + 4 + 160 * 105 + 4 * d->num_rooms;
  int32_t i;

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

  for(i = 0; i < d->num_rooms; i++) {
    d->rooms[i].x = 0;
    d->rooms[i].y = 0;
    d->rooms[i].length = 0;
    d->rooms[i].height = 0;
  }

  for(j = 0; j < DUNGEON_Y; j++) {
    for(i = 0; i < DUNGEON_X; i++) {
      if(d->item_grid[j][i] != nullptr) {
        delete d->item_grid[j][i];
        d->item_grid[j][i] = nullptr;
      }
    }
  }
   
  for(j = 0; j < DUNGEON_Y; j++) {
    for(i = 0; i < DUNGEON_X; i++) {
      if(d->char_grid[j][i] != nullptr) {
        d->char_grid[j][i] = nullptr;
      }
    }
  }

  heap_delete(&d->event_heap);

  free(d->rooms);
  d->rooms = nullptr;
}

void init_dungeon(_dungeon *d, uint8_t load) {
  memset(&d->event_heap, 0, sizeof(d->event_heap));
  heap_init(&d->event_heap, event_cmp, delete_event);
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

int end_game(_dungeon *d, int mode) {
  d->display->endGameScreen(mode);
  delete_dungeon(d);

  delete d->player;
  d->player = nullptr;

  delete d->display;
  d->display = nullptr;

  exit(0);
  return 1;
}

void print_to_term(_dungeon *d) {
  uint8_t x, y;
  for(y = 0; y < DUNGEON_Y; y++) {
    for(x = 0; x < DUNGEON_X; x++) {
      if(d->player && x == d->player->getX() && y == d->player->getY()) {
        putchar('@');
      }
      else {
        switch(mapxy(x ,y)) {
          case ter_wall:
          case ter_wall_immutable:
            putchar(' ');
            break;
          case ter_border_north:
            putchar('-');
            break;
          case ter_border_south:
            putchar('-');
            break;
          case ter_border_west:
            putchar('|');
            break;
          case ter_border_east:
            putchar('|');
            break;
          case ter_floor:
          case ter_floor_room:
            putchar('.');
            break;
          case ter_floor_hall:
            putchar('#');
            break;
          case ter_stairs_down:
            putchar('>');
            break;
          case ter_stairs_up:
            putchar('<');
            break;
          case endgame_flag:
            putchar('X');
            break;
        } 
      }
    }
    putchar('\n');
  }
}
