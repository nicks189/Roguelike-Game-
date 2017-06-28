#ifndef EFFECT_H
#define EFFECT_H

#include <string>

#include "terrain.h"
#include "dimensions.h"

#define RENDER_HEIGHT 3
#define RENDER_WIDTH 5

using std::string;

typedef struct dungeon _dungeon;

class effect {
  private:
    _dungeon *d;
    string name;
    int x, y;
    char **renderArr;
    int damage;

    // Only allowing one color per effect 
    int color;
  public:
    effect(_dungeon *dun);
    ~effect();
    char **getRenderArray() { return renderArr; }
    string getName() { return name; }
    int getDamage() { return damage; }
    int getColor() { return color; }
    int getX() { return x; }
    int getY() { return y; }
    void setDamage(int n) { damage = n; }
    void setName(string n) { name = n; }
    void setColor(int t) { color = t; }
    void setX(int t) { x = t; }
    void setY(int t) { y = t; }
    void trigger(int, int);
};

#endif
