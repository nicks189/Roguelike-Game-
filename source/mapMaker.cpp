#include "../include/mapMaker.h"
#include "../include/dungeon.h"

mapMaker::mapMaker() {
  d = (_dungeon *) malloc(sizeof(*d));
  d->seed = time(NULL);
  d->numrooms = 0;
  d->nummon = 0;
  d->numitems = 0;
  d->level = 1;
  d->nofog = false;
  d->custom = false;
  d->rooms = (room_t *) malloc(sizeof(*d->rooms) * MAX_ROOMS);
  init_dungeon(d, 0);
}

mapMaker::~mapMaker() {
  delete_dungeon(d);
  free(d);
}

int mapMaker::addRoom(room_t r) {
  d->rooms[d->numrooms] = r;
  d->numrooms++;
  return 0;
}

int mapMaker::completeDungeon() {
  fill_rooms(d);
  d->player = new pc(d);
  connect_rooms(d);
  return 0;
}

int mapMaker::saveToFile(string path) {
  save_to_file(d, path.c_str());
  return 0;
}

int mapMaker::loadFromFile(string path) {
  read_from_file(d, path.c_str());
  fill_rooms(d);
  load_dungeon(d);
  return 0;
}

int mapMaker::printDungeon() {
  print_to_term(d);
  return 0;
}
