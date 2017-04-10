#ifndef DISPLAY_H
#define DISPLAY_H

#include "item.h"

typedef struct dungeon _dungeon;

enum displayMode {
  MAP_MODE, 
  INVENTORY_MODE,
  EQUIPMENT_MODE,
  WEAR_MODE,
  TAKEOFF_MODE,
  DROP_MODE,
  EXPUNGE_MODE,
  INSPECT_MODE
};

class abstractDisplay {
  protected:
    _dungeon *d;
  public:
    abstractDisplay(_dungeon *dun) {
      d = dun;
    }
    virtual ~abstractDisplay() {}
    virtual void displayMap() = 0;
    virtual void displayInventory(displayMode mode) = 0;
    virtual void displayItemDescription(item *i) = 0;
    virtual void displayCharacterStats() = 0;
    virtual void endGameScreen(int mode) = 0;
};

class cursesDisplay : public abstractDisplay {
  private:
    inline int inLosRange(int x, int y);
  public:
    cursesDisplay(_dungeon *dun);
    ~cursesDisplay();
    void displayMap();
    void displayInventory(displayMode mode); 
    void displayItemDescription(item *i);
    void displayCharacterStats();
    void endGameScreen(int mode);
};

#endif
