#ifndef MAPMAKER_H
#define MAPMAKER_H

#include <string>

typedef struct dungeon _dungeon;
typedef struct room room_t;

using std::string;

class mapMaker {
  private:
    _dungeon *d;
  public:
    mapMaker();
    ~mapMaker();
    int addRoom(room_t room);
    int completeDungeon();
    int saveToFile(string path);
    int loadFromFile(string path);
    int printDungeon();
};

#endif
