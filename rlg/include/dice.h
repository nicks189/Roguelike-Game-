#ifndef DICE_H
#define DICE_H

#include <string>

#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

using std::string;

class dice {
  private:
    int base;
    int number;
    int sides;  

  public: 
    dice() : base(0), number(0), sides(0) {} 
    dice(int b, int n, int s) : base(b), number(n), sides(s) {}
    ~dice() {}
    void setDice(int b, int n, int s) {
      base = b;
      number = n;
      sides = s;
    }
    void setBase(int b) { base = b; }
    void setNumber(int n) { number = n; }
    void setSides(int s) { sides = s; }
    int roll() const;
    int getBase() { return base; }
    int getNumber() { return number; }
    int getSides() { return sides; }
    dice &operator=(dice &d); 
    string toString();
};

#endif
