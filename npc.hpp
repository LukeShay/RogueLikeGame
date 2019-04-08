#ifndef NPC_H
#define NPC_H

#include "dice.h"
#include <string>

#define SMART 0x00000001
#define TELE 0x00000002
#define TUNNEL 0x00000004
#define ERRATIC 0x00000008

class npc {

public:
  std::string name;
  std::string color;
  std::string desc;
  int abilities;
  char symbol;
  dice speed;
  dice hp;
  dice ad;
  int rarity;
  int p;

  npc();
  ~npc();
  void print_fields();
};
#endif
