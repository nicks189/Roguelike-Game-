#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include "dungeon.h"
#include "factory.h"
#include "item.h"

using namespace std;

int main(int argc, char *argv[]) {
  _dungeon d;
  d.seed = time(NULL);
  d.nummon = 0;
  d.numitems = 0;
  d.level = 1;
  d.nofog = false;

  /* handles command line args */
  if(argc > 11) {
    fprintf(stderr, "wrong parameters\n");
    return 1;
  }

  for(uint8_t i = 1; i < argc; i++) {
    if(argv[i][0] == '-') {
      if(!(strcmp(argv[i], "--load")) || argv[i][1] == 'l') 
        cout << "Not curently implemented" << endl; //_load = 1;
      else if(!(strcmp(argv[i], "--save")) || argv[i][1] == 's') 
        cout << "Not currently implemented" << endl; //_save = 1;
      else if(!(strcmp(argv[i], "--pc")) || argv[i][1] == 'p') {
        cout << "Not currently implemented" << endl;
        //if(i < argc - 1) {
        //  pc_loaded = 1;
        //  pc_loc[dim_y] = atoi(argv[++i]);
        //  pc_loc[dim_x] = atoi(argv[++i]);
        //}
      }
      else if(!(strcmp(argv[i], "--nofog")) || argv[i][1] == 'g') {
        d.nofog = true;
      }
      else if(!(strcmp(argv[i], "--rand")) || argv[i][1] == 'r') {
        if(argc < ++i + 1 || !sscanf(argv[i], "%ld", &d.seed)) {}
      }
      else if(!(strcmp(argv[i], "--nummon")) || argv[i][1] == 'm') {
        if(i < argc) {
          d.nummon = atoi(argv[++i]);
        }
      }
      else if(!(strcmp(argv[i], "--items")) || argv[i][1] == 'i') {
        if(i < argc) {
          d.numitems = atoi(argv[++i]);
        }
      }
    }
    else {}
  }

  srand(d.seed);
  cout << "Using seed: " << d.seed << endl;
    
  dungeon_init(&d);
  run_dungeon(&d);

  return 0;
}
