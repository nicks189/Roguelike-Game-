#include "dungeon.h"

int main(int argc, char *argv[]) {
  _dungeon d;
  d.seed = time(NULL);
  uint8_t _load, _save, isn_empty, pc_loaded;
  srand(d.seed);
  d.nummon = rand_range(18, 25);
  char file_name[255];
  pair_t pc_loc;
  _load = 0;
  _save = 0;
  isn_empty = 0;
  pc_loaded = 0;
  pc_loc[dim_y] = 0;
  pc_loc[dim_x] = 0;

  /* handles command line args */
  if(argc > 10) {
    fprintf(stderr, "wrong parameters\n");
    return 1;
  }

  for(uint8_t i = 1; i < argc; i++) {
    if(argv[i][0] == '-') {
      if(!(strcmp(argv[i], "--load")) || argv[i][1] == 'l') _load = 1;
      else if(!(strcmp(argv[i], "--save")) || argv[i][1] == 's') _save = 1;
      else if(!(strcmp(argv[i], "--pc")) || argv[i][1] == 'p') {
        if(i < argc - 1) {
          pc_loaded = 1;
          pc_loc[dim_y] = atoi(argv[++i]);
          pc_loc[dim_x] = atoi(argv[++i]);
        }
      }
      else if(!(strcmp(argv[i], "--rand")) || argv[i][1] == 'r') {
        if(argc < ++i + 1 || !sscanf(argv[i], "%ld", &d.seed)) {}
      }
      else if(!(strcmp(argv[i], "--nummon")) || argv[i][1] == 'm') {
        if(i < argc) {
          d.nummon = atoi(argv[++i]);
        }
      }
    }
    else {
        strcpy(file_name, argv[i]);
        isn_empty = 1;
    }
  }

  srand(d.seed);
  printf("Using seed: %ld\n", d.seed);

  /* file concatination */
  char r_path[255];
  strcpy(r_path, getenv("HOME"));
  strcat(r_path, "/.rlg327/");

  char s_path[255];
  strcpy(s_path, getenv("HOME"));
  strcat(s_path, "/.rlg327/dungeon");

  if(isn_empty == 1 && _load == 1) {
    strcat(r_path, file_name);
    printf("FILENAME %s\n", file_name);
  }
  else if(isn_empty == 1 && _load == 0) {
   strcat(r_path, "dungeon");
   printf("Didn't call load!");
  }
  else {
    strcat(r_path, "dungeon");
  }

  dungeon_init(&d, _load, _save, r_path, s_path, pc_loaded, pc_loc);
  print(&d);
  run_dungeon(&d);
  return 0;
}
