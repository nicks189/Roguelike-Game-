#include "effect.h"
#include "dungeon.h"

effect::effect(_dungeon *dun) : d(dun) {
  damage = 0;
  color = COLOR_RED;
  renderArr = 0;
  name = "effect";
  x = 0;
  y = 0;

  int i;
  
  /* Initialize renderArr 
   *
   *  /   \
   *       
   *  \   /
   *                 */
  renderArr = new char*[RENDER_HEIGHT];

  for(i = 0; i < RENDER_HEIGHT; i++) {
    renderArr[i] = new char[RENDER_WIDTH];
  } 

  renderArr[0][0] = '/';
  renderArr[0][1] = ' ';
  renderArr[0][2] = ' ';
  renderArr[0][3] = ' ';
  renderArr[0][4] = '\\';
  renderArr[1][0] = ' ';
  renderArr[1][1] = ' ';
  renderArr[1][2] = ' ';
  renderArr[1][3] = ' ';
  renderArr[1][4] = ' ';
  renderArr[2][0] = '\\';
  renderArr[2][1] = ' ';
  renderArr[2][2] = ' ';
  renderArr[2][3] = ' ';
  renderArr[2][4] = '/';
} 

effect::~effect() {
  int i;

  for(i = 0; i < RENDER_HEIGHT; i++) {
      delete [] renderArr[i];
  }
  delete [] renderArr;
  
  renderArr = 0;
}

void effect::trigger(int f, int s) {
  int x_l, x_h, y_l, y_h;
 
  x = f;
  y = s;

  x_h = x + 2;
  y_h = y + 1;
  
  for(y_l = y - 1; y_l <= y_h; y_l++) {
    for(x_l = x - 2; x_l <= x_h; x_l++) {
      if(char_gridxy(x_l, y_l) != nullptr) {
        character *c = char_gridxy(x_l, y_l);
        c->setHp(c->getHp() - damage); 

        d->disp_msg.append("-");
        d->disp_msg.append(name);
        d->disp_msg.append(" did ");
        d->disp_msg.append(std::to_string(damage));
        d->disp_msg.append(" damage");
        
        if(c->getHp() < 1) {
          c->setHp(0);
          c->kill();

          d->player->setScore(d->player->getScore() + 100 * (c->getLevel() + 1));

          char_gridxy(x_l, y_l) = nullptr;
        }

      } 
    }
  }
}
