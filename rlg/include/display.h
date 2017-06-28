#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>

#include "item.h"
#include "projectile.h"
#include "effect.h"

typedef struct dungeon _dungeon;

using std::string;

enum displayMode {
  MAP_MODE,
  INVENTORY_MODE,
  EQUIPMENT_MODE,
  WEAR_MODE,
  TAKEOFF_MODE,
  DROP_MODE,
  EXPUNGE_MODE,
  INSPECT_MODE,
  RANGED_SELECT_MODE,
  RANGED_ATTACK_MODE
};

class abstractDisplay {
  protected:
    _dungeon *d;
    int pcX, pcY;
  public:
    abstractDisplay(_dungeon *dun) {
      d = dun;
      pcX = 0;
      pcY = 0;
    }
    virtual ~abstractDisplay() {}
    virtual int displayMenu() = 0;
    virtual void displayMap() = 0;
    virtual void displayInventory(displayMode mode) = 0;
    virtual void displayItemDescription(item *i) = 0;
    virtual void displayCharacterStats() = 0;
    virtual void displayRangedAttack(displayMode) = 0;
    virtual void displayProjectile(projectile *p) = 0;
    virtual void displayEffect(effect *e, int x, int y) = 0;
    virtual void displayNextMessage(string s) = 0;
    virtual void endGameScreen(int mode) = 0;
};

class cursesDisplay : public abstractDisplay {
  private:
    inline int inLosRange(int x, int y);
  public:
    cursesDisplay(_dungeon *dun);
    ~cursesDisplay();
    int displayMenu();
    void displayMap();
    void displayInventory(displayMode mode);
    void displayItemDescription(item *i);
    void displayCharacterStats();
    void displayRangedAttack(displayMode);
    void displayProjectile(projectile *p);
    void displayEffect(effect *e, int x, int y);
    void displayNextMessage(string s);
    void endGameScreen(int mode);
};

/* Not currently implemented. Will be added to
 * use the SFML 2D graphics library to added 
 * 8-bit graphics overlay
 */
class sfmlDisplay : public abstractDisplay {
  private:
    inline int inLosRange(int x, int y);
  public:
    sfmlDisplay(_dungeon *dun);
    ~sfmlDisplay();
    int displayMenu();
    void displayMap();
    void displayInventory(displayMode mode);
    void displayItemDescription(item *i);
    void displayCharacterStats();
    void displayRangedAttack(displayMode);
    void displayProjectile(projectile *p);
    void displayEffect(effect *e, int x, int y);
    void displayNextMessage(string s);
    void endGameScreen(int mode);
};

#endif
