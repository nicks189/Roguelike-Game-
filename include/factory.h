#ifndef FACTORY_H
#define FACTORY_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>

#include "character.h"
#include "item.h"

typedef struct dungeon _dungeon;

using std::string;
using std::getline;
using std::stringstream;
using std::vector;
using std::ifstream;
using std::cout;
using std::endl;

class abstractFactory {
  protected:
    vector<stringstream *> cvector;
    _dungeon *d;
  public:
    virtual ~abstractFactory() {
      while(cvector.size() != 0) {
        cvector.pop_back(); 
      }
    }
    virtual void *generateNext() = 0;
};

class characterFactory : public abstractFactory {
  private:
    character *c;
  public:
    characterFactory(_dungeon *dun);
    ~characterFactory() {}
    void *generateNext(); 
};

class itemFactory : public abstractFactory {
  private:
    item *c;
  public:
    itemFactory(_dungeon *dun); 
    ~itemFactory() {}
    void *generateNext();
};

#endif
