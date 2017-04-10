#include "dungeon.h"
#include "factory.h"
#include "dice.h"

inline int setcolor(string color) {
  if(color == "RED")
    return 1;  
  else if(color == "GREEN")
    return 2;  
  else if(color == "YELLOW")
    return 3;  
  else if(color == "BLUE")
    return 4;  
  else if(color == "CYAN")
    return 5;  
  else if(color == "MAGENTA")
    return 6;  
  else 
    return 7;  
}

inline int getabils(string a) {
  int t = 0;

  if(a == "ERRATIC")
    t += 8;
  if(a == "TUNNEL")
    t += 4;
  if(a == "TELE")
    t += 2;
  if(a == "SMART")
    t += 1;

  return t;
}

inline objectType settype(string t) {
  if(t == "WEAPON")
    return WEAPON_TYPE;
  else if(t == "OFFHAND")
      return OFFHAND_TYPE;
  else if(t == "RANGED")
    return RANGED_TYPE;
  else if(t == "ARMOR")
      return ARMOR_TYPE;
  else if(t == "HELMET")
    return HELMET_TYPE;
  else if(t == "CLOAK")
      return CLOAK_TYPE;
  else if(t == "GLOVES")
    return GLOVE_TYPE;
  else if(t == "BOOTS")
      return BOOT_TYPE;
  else if(t == "RING")
    return RING_TYPE;
  else if(t == "AMULET")
      return AMULET_TYPE;
  else if(t == "LIGHT")
    return LIGHT_TYPE;
  else if(t == "SCROLL")
      return SCROLL_TYPE;
  else if(t == "BOOK")
    return BOOK_TYPE;
  else if(t == "FLASK")
      return FLASK_TYPE;
  else if(t == "GOLD")
    return GOLD_TYPE;
  else if(t == "AMMUNITION")
      return AMMUNITION_TYPE;
  else if(t == "FOOD")
    return FOOD_TYPE;
  else if(t == "WAND")
      return WAND_TYPE;
  else if(t == "CONTAINER")
    return CONTAINER_TYPE;
  else 
    return INVALID_TYPE;
}

characterFactory::characterFactory(_dungeon *dun) {
  d = dun;

  string path = getenv("HOME");
  path += "/.rlg327/monster_desc.txt";

  ifstream f(path); 
  stringstream *ss;
  string buf;

  /* check valid file */
  getline(f, buf);
  if(buf != "RLG327 MONSTER DESCRIPTION 1") {
    fprintf(stderr, "File mismatch -- missing \"monster_desc.txt\"\n");
  }
  buf = "";

  ss = new stringstream();
  while(getline(f, buf)) {
    if(buf == "BEGIN MONSTER") {} 
    else if(buf == "") {}
    else if(buf == "END") {
      //cout << endl;
      cvector.push_back(ss);
      ss = new stringstream();
    }
    else {
      *ss << buf << "\n";
    }
    buf = "";
  }

  delete ss;

  f.close();
}

void *characterFactory::generateNext() {
  if(cvector.size() == 0) {
    return nullptr;
  }

  c = new npc(d);
  stringstream *ss = cvector.back();

  string buf;
  string cbuf;
  bool nameset = false;
  bool descset = false;
  bool colorset = false;
  bool speedset = false;
  bool abilset = false;
  bool hpset = false;
  bool damset = false;
  bool symbset = false; 
  bool failed = false;

  while(*ss >> buf) {
    /* Name */
    if(buf == "NAME") { 
      if(!nameset) {
        nameset = true;
        ss->get();
        getline(*ss, cbuf);
        c->setName(cbuf);
      }
      else {
        failed = true;
      }
    }

    /* Symbol */
    else if(buf == "SYMB") {
      if(!symbset) {
        symbset = true;
        ss->get();
        c->setSymbol(ss->get());
      }
      else {
        failed = true;
      }
    }

    /* Color */
    else if(buf == "COLOR") {
      if(!colorset) {
        colorset = true;
        *ss >> cbuf;
        c->setColor(setcolor(cbuf));
        getline(*ss, cbuf);
      }
      else {
        failed = true;
      }
    }

    /* Speed */
    else if(buf == "SPEED") {
      if(!speedset) {
        speedset = true;
        dice d;
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d.setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d.setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d.setSides(atoi(cbuf.c_str()));
        c->setSpeed(d.roll());
      }
      else {
        failed = true;
      }
    }

    /* Damage */
    else if(buf == "DAM") {
      if(!damset) {
        damset = true;
        dice *d = new dice();
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d->setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d->setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d->setSides(atoi(cbuf.c_str()));
        c->setDamage(d);
      }
      else {
        failed = true;
      }
    }

    /* Hitpoints */
    else if(buf == "HP") {
      if(!hpset) {
        hpset = true;
        dice d;
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d.setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d.setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d.setSides(atoi(cbuf.c_str()));
        c->setHp(d.roll());
        c->setMaxHp(c->getHp());
      }
      else {
        failed = true;
      }
    }

    /* Abilities */
    else if(buf == "ABIL") {
      if(!abilset) {
        abilset = true;
        int a = 0;
        string t;
        stringstream sst;
        ss->get();

        getline(*ss, t);

        sst << t;

        while(sst >> cbuf) {
          a += getabils(cbuf);
        }

        c->setTraits(a);

      }
      else {
        failed = true;
      }
    }

    /* Description */
    else if(buf == "DESC") {
      if(!descset) {
        descset = true;
        string t;
        ss->get();
        while(ss->peek() != '.') {
          getline(*ss, cbuf);
          t += cbuf + '\n';
        }
        t.pop_back();
        ss->get();
        c->setDesc(t);
      }

      else {
        failed = true;
      }
    }

    cbuf = "";
    buf = "";
  }

  delete ss;
  cvector.pop_back();

  if(nameset && descset && colorset && speedset && abilset 
                         && hpset && damset && symbset && !failed) {
    return c;
  }

  else {
    delete c;
    return nullptr;
  }
}

itemFactory::itemFactory(_dungeon *dun) {
  d = dun;

  string path = getenv("HOME");
  path += "/.rlg327/object_desc.txt";

  ifstream f(path); 
  stringstream *ss;
  string buf;

  /* check valid file */
  getline(f, buf);
  if(buf != "RLG327 OBJECT DESCRIPTION 1") {
    fprintf(stderr, "File mismatch -- missing \"object_desc.txt\"\n");
  }
  buf = "";

  ss = new stringstream();
  while(getline(f, buf)) {
    if(buf == "BEGIN OBJECT") {} 
    else if(buf == "") {}
    else if(buf == "END") {
      //cout << endl;
      cvector.push_back(ss);
      ss = new stringstream();
    }
    else {
      *ss << buf << "\n";
    }
    buf = "";
  }

  delete ss;

  f.close();
}

void *itemFactory::generateNext() {
  if(cvector.size() == 0) {
    return nullptr;
  }

  c = new item(d);
  stringstream *ss = cvector.back();

  string buf;
  string cbuf;
  bool nameset = false;
  bool descset = false;
  bool typeset = false;
  bool colorset = false;
  bool hitset = false; 
  bool damset = false;
  bool dodgeset = false;
  bool defset = false;
  bool weightset = false;
  bool speedset = false;
  bool attrset = false;
  bool valset = false;
  bool failed = false;

  while(*ss >> buf) {

    /* Name */
    if(buf == "NAME") { 
      if(!nameset) {
        nameset = true;
        ss->get();
        getline(*ss, cbuf);
        c->setName(cbuf);
      }
      else {
        failed = true;
      }
    }

    /* Description */
    else if(buf == "DESC") {
      if(!descset) {
        descset = true;
        string t;
        ss->get();
        while(ss->peek() != '.') {
          getline(*ss, cbuf);
          t += cbuf + '\n';
        }
        t.pop_back();
        ss->get();
        c->setDesc(t);
      }

      else {
        failed = true;
      }
    }

    /* Type */
    else if(buf == "TYPE") {
      if(!typeset) {
        typeset = true;
        *ss >> cbuf;
        c->setType(settype(cbuf));
        c->setSymbol(c->getType());
        getline(*ss, cbuf);
      }
      else {
        failed = true;
      }
    }

    /* Color */
    else if(buf == "COLOR") {
      if(!colorset) {
        colorset = true;
        *ss >> cbuf;
        c->setColor(setcolor(cbuf));
        getline(*ss, cbuf);
      }
      else {
        failed = true;
      }
    }

    /* Hit */
    else if(buf == "HIT") {
      if(!hitset) {
        hitset = true;
        dice d;
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d.setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d.setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d.setSides(atoi(cbuf.c_str()));
        c->setHit(d.roll());
      }
      else {
        failed = true;
      }
    }

    /* Damage */
    else if(buf == "DAM") {
      if(!damset) {
        damset = true;
        dice *d = new dice();
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d->setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d->setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d->setSides(atoi(cbuf.c_str()));
        c->setDamage(d);
      }
      else {
        failed = true;
      }
    }

    /* Dodge */
    else if(buf == "DODGE") {
      if(!dodgeset) {
        dodgeset = true;
        dice d;
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d.setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d.setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d.setSides(atoi(cbuf.c_str()));
        c->setDodge(d.roll());
      }
      else {
        failed = true;
      }
    }

    /* Defense */
    else if(buf == "DEF") {
      if(!defset) {
        defset = true;
        dice d;
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d.setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d.setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d.setSides(atoi(cbuf.c_str()));
        c->setDefense(d.roll());
      }
      else {
        failed = true;
      }
    }

    /* Weight */
    else if(buf == "WEIGHT") {
      if(!weightset) {
        weightset = true;
        dice d;
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d.setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d.setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d.setSides(atoi(cbuf.c_str()));
        c->setWeight(d.roll());
      }
      else {
        failed = true;
      }
    }

    /* Speed */
    else if(buf == "SPEED") {
      if(!speedset) {
        speedset = true;
        dice d;
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d.setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d.setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d.setSides(atoi(cbuf.c_str()));
        c->setSpeed(d.roll());
      }
      else {
        failed = true;
      }
    }

    /* Attributes */
    else if(buf == "ATTR") {
      if(!attrset) {
        attrset = true;
              
        dice d;
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d.setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d.setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d.setSides(atoi(cbuf.c_str()));
        c->setAttribute(d.roll());

      }
      else {
        failed = true;
      }
    }

    /* Value */ 
    else if(buf == "VAL") { 
      if(!valset) {
        valset = true;
              
        dice d;
        char p;

        ss->get();
        while((p = ss->get()) != '\n') {
          if(p == '+') {
            d.setBase(atoi(cbuf.c_str()));  
            cbuf = "";
          } 
          else if(p == 'd') {
            d.setNumber(atoi(cbuf.c_str())); 
            cbuf = "";
          }
          else {
            cbuf += p;
          }
        }
        d.setSides(atoi(cbuf.c_str()));
        c->setValue(d.roll());

      }
      else {
        failed = true;
      }
    }

    cbuf = "";
    buf = "";
  }

  delete ss;
  cvector.pop_back();

  if(nameset && descset && typeset && colorset && hitset 
       && damset && dodgeset && defset 
       && weightset && speedset 
       && attrset && valset && !failed) {
                                                    
    return c;
  }

  else {
    delete c;
    return nullptr;
  }
  
  return nullptr;
}

