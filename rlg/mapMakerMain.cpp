#include <iostream>

#include "mapMaker.h"
#include "dungeon.h"

using namespace std;

int main(int argc, char **argv) {
  mapMaker mm;

  //room_t r;
  //r.x = 20;
  //r.y = 10;
  //r.length = 10;
  //r.height = 10;

  //room_t r1; 
  //r1.x = 80;
  //r1.y = 10;
  //r1.length = 10;
  //r1.height = 10;

  //room_t r2;
  //r2.x = 20;
  //r2.y = 60;
  //r2.length = 10;
  //r2.height = 10;

  //room_t r3;
  //r3.x = 70;
  //r3.y = 40;
  //r3.length = 40;
  //r3.height = 20;

  //mm.addRoom(r);
  //mm.addRoom(r1);
  //mm.addRoom(r2);
  //mm.addRoom(r3);

  //mm.completeDungeon();
  //mm.saveToFile("/home/nick/.rlg327/boss01.rlg327");

  mm.loadFromFile("/home/nick/.rlg327/boss01.rlg327");
  
  mm.printDungeon();

  return 0; 
}
