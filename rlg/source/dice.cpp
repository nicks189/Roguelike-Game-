#include "../include/dice.h"

int dice::roll() const {
  int total;
  int i;

  total = base;

  if(sides) {
    for(i = 0; i < number; i++) {
      total += rand_range(1, sides);
    }
  }

  return total;
}

string dice::toString() {
  string ret;
  ret += std::to_string(base) + "+";
  ret += std::to_string(number) + "d";
  ret += std::to_string(sides);
  return ret;
}

dice &dice::operator=(dice &d) {
  base = d.getBase();
  number = d.getNumber();
  sides = d.getSides();
  return d;
}
