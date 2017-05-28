#ifndef PC_H
#define PC_H

#define DUNGEON_X 160
#define DUNGEON_Y 105

#include <stdint.h>

#include "character.h"
#include "dimensions.h"
#include "terrain.h"
#include "item.h"
#include "display.h"
#include "projectile.h"

#define INVENTORY_SIZE 10
#define DEFAULT_LIGHT_RADIUS 5
#define DEFAULT_HP 100

/* Controls */
#define MV_UP_LEFT_1 067 
#define MV_UP_LEFT_2 0171 
#define MV_UP_1 070
#define MV_UP_2 0153 
#define MV_UP_RIGHT_1 071
#define MV_UP_RIGHT_2 0165 
#define MV_RIGHT_1 066
#define MV_RIGHT_2 0154 
#define MV_DWN_RIGHT_1 063
#define MV_DWN_RIGHT_2 0156 
#define MV_DWN_1 062
#define MV_DWN_2 0152
#define MV_DWN_LEFT_1 061
#define MV_DWN_LEFT_2 0142 
#define MV_LEFT_1 064
#define MV_LEFT_2 0150 
#define REST_1 065
#define REST_2 040 
#define MV_UP_STAIRS 074
#define MV_DWN_STAIRS 076
#define LOOK_MODE 0114
#define CONTROL_MODE 033
#define QUIT_GAME 0121
#define INVENTORY_SCREEN 0151
#define EQUIPMENT_SCREEN 0145
#define WEAR_SCREEN 0167
#define TAKEOFF_SCREEN 0164
#define DROP_SCREEN 0144
#define EXPUNGE_SCREEN 0170
#define INSPECT_SCREEN 0111
#define STATS_SCREEN 0143
#define RANGED_SCREEN 0162

/* Inventory controls */
#define SLOT0 060
#define SLOT1 061
#define SLOT2 062
#define SLOT3 063
#define SLOT4 064
#define SLOT5 065
#define SLOT6 066
#define SLOT7 067
#define SLOT8 070
#define SLOT9 071
#define EQUIPA 0141
#define EQUIPB 0142
#define EQUIPC 0143
#define EQUIPD 0144
#define EQUIPE 0145
#define EQUIPF 0146
#define EQUIPG 0147
#define EQUIPH 0150
#define EQUIPI 0151
#define EQUIPJ 0152
#define EQUIPK 0153
#define EQUIPL 0154
#define EQUIPM 0155
#define EQUIPN 0156

typedef struct dungeon _dungeon;

struct equipment_t {
  equipment_t() : weapon(), offhand(), ranged(), 
                armor(), helmet(), cloak(),
                gloves(), boots(), amulet(),
                light(), ring_one(), ring_two(), 
                wand(), ammunition() {}

  item *weapon;
  item *offhand;
  item *ranged;
  item *armor;
  item *helmet;
  item *cloak;
  item *gloves;
  item *boots;
  item *amulet;
  item *light;
  item *ring_one;
  item *ring_two;
  item *wand;
  item *ammunition;
};

enum characterClass {
  WARRIOR, 
  WIZARD,
  ARCHER,
  ROGUE,
  NO_CLASS
};

class pc : public character {
  private:
    characterClass currentClass;
    /*         *
     *  Stats  *
     *         */
    int strength;
    int rangedAP; 
    int lightRadius;
    int cash;
    int weight;
    int score;
    /*                          *
     * Private member functions * 
     *                          */
    item *equip(item *t);
    int pickupItems();
    int attackPos();
    int rangedAttack(projectile *p, int cmd);
    bool getInventorySlot(int &s);
    inline int command(int cmd, displayMode mode);      
    inline int inventoryCommand(int cmd);
    inline int equipmentCommand(int cmd);
    inline int wearCommand(int cmd);
    inline int takeOffCommand(int cmd);
    inline int dropCommand(int cmd);
    inline int expungeCommand(int cmd);
    inline int inspectCommand(int cmd);
    inline int statsCommand(int cmd);
    inline int rangedCommand(int cmd);
    inline void updateDesc();

  public:
    pc(_dungeon *dun);
    pc(_dungeon *dun, int16_t *loc);
    ~pc() { clearInventories(); }
    void place(_dungeon *dun);
    equipment_t equipment;
    item *inventory[INVENTORY_SIZE];
    terrain_t pcmap[DUNGEON_Y][DUNGEON_X];
    bool fireball_is_learned;
    bool heal_is_learned;
    int move(int move);
    int move(); 
    int getCash() { return cash; }
    int getStrength() { return strength; }
    int getLightRadius() { return lightRadius; }
    int getWeight() { return weight; }
    int getRangedAP() { return rangedAP; }
    int getScore() { return score; }
    characterClass getClass() { return currentClass; }
    void setClass(characterClass c) { currentClass = c; }
    void setStrength(int s) { strength = s; }
    void setRangedAP(int n) { rangedAP = n; }
    void setScore(int n) { score = n; }
    void clearInventories();
    void updateMap();
    void initMap();
};

#endif
