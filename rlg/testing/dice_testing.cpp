#include <iostream>

#include "../dice.h"

using namespace std;

int main() {
  
  dice test(500, 20, 6);
  dice test1(100, 21, 8);
  dice test2(8, 6, 20);
  dice test3(2, 3, 2);

  cout << test.roll() << endl;
  cout << test1.roll() << endl;
  cout << test2.roll() << endl;
  cout << test3.roll() << endl;

  return 0;
}
