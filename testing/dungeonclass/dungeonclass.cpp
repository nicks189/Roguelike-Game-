#include <vector>

#include "dungeonclass.h"
//#include "pathfinding.h"
//#include "event.h"
//#include "factory.h"

using std::vector;
using std::stringstream;

dungeonclass::dungeonclass() : nofog(false), custom(true), hardness(0), numrooms(0),
                     nummon(0), numitems(0), level(1), view_mode(CONTROL_MODE),
                     seed(time(NULL)), event_heap(), lcoords(0), rooms(), 
                     map(), char_grid(), item_grid(), tunnel_map(), non_tunnel_map(),
                     player(), display() {

  memset(&d->event_heap, 0, sizeof(d->event_heap));
  heap_init(&d->event_heap, event_cmp, delete_event);
  init();
}

dungeonclass::~dungeonclass() {
  if(player)
    delete player;
  if(display)
    delete display;
}

/* Sets the dungeon up, decoupled from the constructor because this
 * is also used to create new maps when the player goes up/down stairs */ 
int dungeonclass::init() { 
  static int initialized = 0;
  srand(seed);

  /* Load boss level instead */
  if(level == 2) {
    string path = getenv("HOME");
    path.append("/.rlg327/boss01.rlg327");
    init_load(path.c_str());
    return 0;
  }

  /* Make random number of rooms */
  numrooms = rand_range(MIN_ROOMS, MAX_ROOMS);
  rooms = (room_t *) malloc(sizeof(*rooms) * numrooms);

  /* clears the dungeon and sets all terrain to ter_wall */
  init_dungeon(0);

  make_rooms();
  fill_rooms();

  if(!initialized) {
    player = new pc(this);
    setupDisplay();
    initialized = 1;
  }

  else {
    player->place(this);  
  }

  heap_insert(&event_heap, init_pc_event(player));

  connect_rooms();
  place_stairs();

  /* Setup pathfinding maps for npcs */
  pathfinding(this, player->getX(), player->getY(), TUNNEL_MODE);
  pathfinding(this, player->getX(), player->getY(), NON_TUNNEL_MODE);

  create_monsters();
  createItems();

  /* Updates the map that is visible to the player */
  player->updateMap(); 

  if(level < 6) {
    level_msg = "It's very dark... you wonder how you got here.";
  }
  else if(level < 10) {
    level_msg = "You begin to shiver from the cold."; 
  }
  else if(level < 15) {
    level_msg = "You hear a strange noise...";
  }

  disp_msg = level_msg;
  display->displayMap();

  return 0;
}

int dungeonclass::init_load(const char *path) {
  init_dungeon(0);

  rooms = (room_t *) malloc(sizeof(*rooms) * MAX_ROOMS);

  if(read_from_file(path)) {
    return 1;
  }
  
  fill_rooms();
  load_dungeon();
  place_stairs();

  player->place();  
  heap_insert(&event_heap, init_pc_event(player));


  pathfinding(this, player->getX(), player->getY(), TUNNEL_MODE);
  pathfinding(this, player->getX(), player->getY(), NON_TUNNEL_MODE);

  create_monsters();
  createItems();

  player->updateMap(); 

  if(d->level != 6) {
    level_msg = "It's very dark... you wonder how you got here.";
  }
  //else if(d->level < 6) {
  //  d->level_msg = "You begin to shiver from the cold."; 
  //}
  else {
    level_msg = "You hear a strange noise...";
  }

  disp_msg = level_msg;
  display->displayMap();

  return 0;
}

/* Running loop - refreshes dungeon and 
 * moves characters until the game ends 
 * or user quits */
int dungeonclass::run() {
  /* Events */
  event_t *cur_node, *peek_node, *temp_node;

  while(true) {
    /* Look mode coords default to player position */
    lcoords[dim_x] = player->getX();
    lcoords[dim_y] = player->getY();

    if(event_heap.size != 1) {
      /* Selects the event at the top of the queue */
      cur_node = (event_t *) heap_remove_min(&event_heap);
    }

    /* Player is the only remaining event, pc wins 
     * This will be changed at some point */
    else {
      display->displayMap();
      heap_delete(&event_heap);
      return end_game(PC_MODE);
    }

    /* Compares current node with the node at the top, if the 
     * turn is the same, goes by sequence number instead. 
     * Continues doing so until the right event is selected. */
    while((peek_node = (event_t *) heap_peek_min(&event_heap))) {
      if(peek_node->time == cur_node->time 
                         && peek_node->sequence < cur_node->sequence) {
        /* The node at the top moves first. Remove the top and 
         * insert the current node back into the heap */
        temp_node = (event_t *) heap_remove_min(&event_heap);
        heap_insert(&event_heap, cur_node);
        cur_node = temp_node;
      }
      else {
        break;
      }
    }

    if(cur_node->type == pc_type) {
      display->displayMap();
      cur_node->time += (1000 / cur_node->c->getSpeed());

      /* __TODO - Make getting command independant__ 
       * Stay in this loop until input is correct */
      while(player->move(getch())) {
        display->displayMap();
      }
      /* Player has moved, we need to update pathfinding maps */
      pathfinding(this, player->getX(), player->getY(), TUNNEL_MODE);
      pathfinding(this, player->getX(), player->getY(), NON_TUNNEL_MODE);
      heap_insert(&event_heap, cur_node);
    }

    else {
      if(!(cur_node->c->isDead())) {
        cur_node->time += (1000 / cur_node->c->getSpeed());
        if(cur_node->c->move()) {
          /* Npc killed the player, must end the game */
          heap_delete(&event_heap);
          return end_game(NPC_MODE);
        }
        heap_insert(&event_heap, cur_node);
      }
      /* This npc is dead, so it can be deleted */
      else {
        delete_event(cur_node);
      }
    }
  }
  return 0;
}

/* TODO */
void dungeonclass::setupDisplay() {
  display = new cursesDisplay(d);
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

void dungeonclass::create_monsters() {
  /* Create 20 monsters by default */
  if(nummon == 0) {
    nummon = 20;
  }

  int i = 0;

  /* Load all monsters from monster_desc.txt into
   * vector, then select 20 randomly to create */
  vector<character *> v;

  /* factory object that parses and instantiates npcs 
   * Probably change this at some point */
  character *cp;
  characterFactory *factory = new characterFactory(this);

  /* Bad implementation but it assures monsters are random */
  while((cp = (character *) factory->generateNext())) {
    v.push_back(cp);
  }

  while(i < nummon) { 
    npc *n = (npc *) v[rand_range(0, v.size() - 1)];

    if(level > n->getLevel()) {
      cp = new npc(*n); 

      char_gridxy(cp->getX(), cp->getY()) = cp;
      heap_insert(&event_heap, init_npc_event(cp, nummon));
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

void dungeonclass::printMonster(character *cp) {
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

void dungeonclass::createItems() {
  /* Identical to create_monsters() */
  int i = 0;
  if(numitems == 0) {
    numitems = 20;
  }

  vector<item *> v;

  item *ip;
  itemFactory *factory = new itemFactory(this);

  /* Bad implementation but it assures items are random */
  while((ip = (item *) factory->generateNext())) {
    v.push_back(ip);
  }

  while(i < numitems) { 
    item *n = v[rand_range(0, v.size() - 1)];
    if(level > n->getLevel()) {
      ip = new item(*n); 
      item_grid[ip->getY()][ip->getX()] = ip;
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

void dungeonclass::printItem(item *ip) {
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

int dungeonclass::mv_up_stairs() {
  level--;
  if(mapxy(player->getX(), player->getY()) == ter_stairs_up) {
    seed++;
    delete_dungeon();
    init();
    display->displayMap();
    run_dungeon();
  }
  return 1;
}

int dungeonclass::mv_dwn_stairs() {
  level++;
  if(mapxy(player->getX(), player->getY()) == ter_stairs_down) {
    seed++;
    delete_dungeon();
    init();
    display->displayMap();
    run_dungeon();
  }
  return 1;
}

/* --from Dr sheaffer */
int dungeonclass::smooth_hardness() {
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  uint8_t temphardness[DUNGEON_Y][DUNGEON_X];
  tail = head = nullptr;

  memset(&temphardness, 0, sizeof (temphardness));

  /* Seed with some values */
  for (i = 1; i < 255; i += 5) {
    do {
      x = rand() % DUNGEON_X;
      y = rand() % DUNGEON_Y;
    } while (temphardness[y][x]);
    temphardness[y][x] = i;
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
    i = temphardness[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !temphardness[y - 1][x - 1]) {
      temphardness[y - 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !temphardness[y][x - 1]) {
      temphardness[y][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < DUNGEON_Y && !temphardness[y + 1][x - 1]) {
      temphardness[y + 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !temphardness[y - 1][x]) {
      temphardness[y - 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < DUNGEON_Y && !temphardness[y + 1][x]) {
      temphardness[y + 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < DUNGEON_X && y - 1 >= 0 && !temphardness[y - 1][x + 1]) {
      temphardness[y - 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < DUNGEON_X && !temphardness[y][x + 1]) {
      temphardness[y][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = nullptr;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < DUNGEON_X && y + 1 < DUNGEON_Y && !temphardness[y + 1][x + 1]) {
      temphardness[y + 1][x + 1] = i;
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
            t += temphardness[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      hardness[y][x] = t / s;
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
            t += temphardness[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      hardness[y][x] = t / s;
    }
  }

  return 0;
}

/* --from Dr Sheaffer-- */
void dungeonclass::empty_dungeon() {
  uint8_t x, y;
  smooth_hardness();
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
int dungeonclass:in_room(int16_t y, int16_t x)
{
  int i;
  for (i = 0; i < numrooms; i++) {
    if ((x >= rooms[i].x) &&
        (x < (rooms[i].y + d->rooms[i].length)) &&
        (y >= rooms[i].y) &&
        (y < (rooms[i].y + d->rooms[i].height))) {
          return 1;
    }
  }
  return 0;
}

void dungeonclass::make_rooms() {
  int i;

  for(i = 0; i < numrooms; i++) {
    while(true) {
      bool bool_tmp = true;

      room_t r = {rand_range(1, DUNGEON_X - 9), rand_range(1, DUNGEON_Y - 7),
           rand_range(ROOM_MIN_X, ROOM_MAX_X), rand_range(ROOM_MIN_Y, ROOM_MAX_Y)};

      /* end points of new room */
      if(r.x + r.length >= DUNGEON_X) r.length = (DUNGEON_X - 2) - r.x;
      uint8_t r_x2 = r.x + r.length;

      if(r.y + r.height >= DUNGEON_Y) r.height = (DUNGEON_Y - 2) - r.y;
      uint8_t r_y2 = r.y + r.height;

      /* Don't allow square rooms */
      if(r.length == r.height) bool_tmp = false;

      for(int j = 0; j < i; j++) {
        uint8_t a_x2 = rooms[j].x + rooms[j].length;
        uint8_t a_y2 = rooms[j].y + rooms[j].height;
        uint8_t a_x1 = rooms[j].x;
        uint8_t a_y1 = rooms[j].y;

        /* compare locations of rooms */
        if((r.x <= a_x2 && r_x2 >= a_x1) || (r_x2 <= a_x2 && r.x >= a_x1)) {
          if((r.y <= a_y2 && r_y2 >= a_y1) || (r_y2 <= a_y2 && r.y >= a_y1)) {
            bool_tmp = false;
            break;
          }
        }
      }
      if(bool_tmp) {
        rooms[i] = r;
        break;
      }
    } 
  }
}

void dungeonclass::fill_rooms() {
  int i;
  int x, y;

  for(i = 0; i < d->numrooms; i++) {
    uint8_t y1 = rooms[i].y;
    uint8_t x1 = rooms[i].x;
    uint8_t y2 = rooms[i].y + rooms[i].height;
    uint8_t x2 = rooms[i].x + rooms[i].length;

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
int dungeonclass::create_cycle()
{
  /* Find the (approximately) farthest two rooms, then connect *
   * them by the shortest path using inverted hardnesses.      */

  int max, tmp, i, j, p, q;
  pair_t e1, e2;
  p = q = 0;

  for (i = max = 0; i < numrooms - 1; i++) {
    for (j = i + 1; j < numrooms; j++) {
      tmp = (((rooms[i].x - rooms[j].x)  *
              (rooms[i].x - rooms[j].x)) +
             ((rooms[i].y - rooms[j].y)  *
              (rooms[i].y - rooms[j].y)));
      if (tmp > max) {
        max = tmp;
        p = i;
        q = j;
      }
    }
  }

  e1[dim_y] = rand_range(rooms[p].y,
                         (rooms[p].y +
                          rooms[p].height - 1));
  e1[dim_x] = rand_range(rooms[p].x,
                         (rooms[p].x +
                          rooms[p].length - 1));
  e2[dim_y] = rand_range(d->rooms[q].y,
                         (rooms[q].y +
                          rooms[q].height - 1));
  e2[dim_x] = rand_range(rooms[q].x,
                         (rooms[q].x +
                          rooms[q].length - 1));

  dijkstra_corridor_inv(this, e1, e2);

  return 0;
}

void dungeonclass::connect_rooms() {
  int i;
  for(i = 0; i < numrooms - 1; i++) {
    pair_t e1, e2;

    e1[dim_x] = rand_range(d->rooms[i].x, rooms[i].length - 1 + rooms[i].x);
    e1[dim_y] = rand_range(d->rooms[i].y, rooms[i].height - 1 + rooms[i].y);
    e2[dim_x] = rand_range(d->rooms[i+1].x, rooms[i+1].length - 1 + rooms[i+1].x);
    e2[dim_y] = rand_range(d->rooms[i+1].y, rooms[i+1].height - 1 + rooms[i+1].y);

    dijkstra_corridor(this, e1, e2);
  }
  create_cycle();
}

int dungeonclass::place_stairs() {
  int xd1, xd2, yd1, yd2, xu1, xu2, xu3, yu1, yu2, yu3;
  xd1 = rand_range(rooms[1].x + 1, rooms[1].length - 2 + rooms[1].x);
  xd2 = rand_range(rooms[2].x + 1, rooms[2].length - 2 + rooms[2].x);
  yd1 = rand_range(rooms[1].y + 1, rooms[1].height - 2 + rooms[1].y);
  yd2 = rand_range(rooms[2].y + 1, rooms[2].height - 2 + rooms[2].y);
  xu1 = rand_range(rooms[3].x + 1, rooms[3].length - 2 + rooms[3].x);
  xu2 = rand_range(rooms[4].x + 1, rooms[4].length - 2 + rooms[4].x);
  yu1 = rand_range(rooms[3].y + 1, rooms[3].height - 2 + rooms[3].y);
  yu2 = rand_range(rooms[4].y + 1, rooms[4].height - 2 + rooms[4].y);
  xu3 = rand_range(rooms[5].x + 1, rooms[5].length - 2 + rooms[5].x);
  yu3 = rand_range(rooms[5].y + 1, rooms[5].height - 2 + rooms[5].y);

  if(level > 1) {
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

inline int dungeonclass::in_los_range(int x, int y) {
  if(nofog) {
    return 1;
  }
  else if(abs(x - player->getX()) <= 5 && 
          (abs(y - player->getY()) <= 5)) {
    return 1;
  }
  return 0;
}

/* fills grid from loaded file */
void dungeonclass::load_dungeon() {
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

int dungeonclass::read_from_file(const char *path) {
  numrooms = 0;
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
    if(numrooms >= MAX_ROOMS) {
      rooms = (room_t *) realloc(rooms, sizeof(rooms) * numrooms);
    }
    rooms[index].x = rooms_temp[0];
    rooms[index].y = rooms_temp[1];
    rooms[index].length = rooms_temp[2];
    rooms[index].height = rooms_temp[3];
    index++;
    numrooms++;
  }

  fclose(fp);
  printf("Loaded from %s\n", path);
  return 0;
}

void dungeonclass::save_to_file(const char* path) {
  FILE *fp;

  fp = fopen(path, "wb");
  unsigned char f_marker[12] = {'R','L','G','3','2','7','-','S','2','0','1','7'};
  uint32_t f_version = 0;
  uint32_t f_size = 12 + 4 + 4 + 160 * 105 + 4 * numrooms;
  int i;

  /* check endianess */
  int e_test = 1;
  if(*(char *) &e_test == 1) {
    f_version = htobe32(f_version);
    f_size = htobe32(f_size);
  }

  fwrite(f_marker, sizeof(f_marker), 1, fp);
  fwrite(&f_version, sizeof(int32_t), 1, fp);
  fwrite(&f_size, sizeof(int32_t), 1, fp);
  fwrite(hardness, sizeof(uint8_t), 160 * 105, fp);

  for(i = 0; i < numrooms; i++) {
    uint8_t to_mem[4];
    to_mem[0] = rooms[i].x;
    to_mem[1] = rooms[i].y;
    to_mem[2] = rooms[i].length;
    to_mem[3] = rooms[i].height;

    fwrite(to_mem, sizeof(to_mem), 1, fp);
  }

  fclose(fp);
}

void dungeonclass::delete_dungeon() {
  int i, j;

  for(i = 0; i < numrooms; i++) {
    rooms[i].x = 0;
    rooms[i].y = 0;
    rooms[i].length = 0;
    rooms[i].height = 0;
  }

  for(j = 0; j < DUNGEON_Y; j++) {
    for(i = 0; i < DUNGEON_X; i++) {
      if(item_grid[j][i] != nullptr) {
        delete item_grid[j][i];
        item_grid[j][i] = nullptr;
      }
    }
  }
   
  for(j = 0; j < DUNGEON_Y; j++) {
    for(i = 0; i < DUNGEON_X; i++) {
      if(char_grid[j][i] != nullptr) {
        char_grid[j][i] = nullptr;
      }
    }
  }

  heap_delete(&event_heap);

  free(d->rooms);
  rooms = nullptr;
}

void dungeonclass::init_dungeon(uint8_t load) {
  view_mode = CONTROL_MODE;

  if(load) {
    uint8_t x, y;
    for(y = 0; y < DUNGEON_Y; y++) {
      for(x = 0; x < DUNGEON_X; x++) {
        mapxy(x, y) = ter_wall;
      }
    }
  }
  empty_dungeon();
}

int dungeonclass::end_game(int mode) {
  display->endGameScreen(mode);
  delete_dungeon();

  delete player;
  player = nullptr;

  delete display;
  display = nullptr;

  exit(0);
  return 1;
}

void dungeonclass::print_to_term() {
  int x, y;

  for(y = 0; y < DUNGEON_Y; y++) {
    for(x = 0; x < DUNGEON_X; x++) {
      if(player && x == player->getX() && y == player->getY()) {
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
