#ifndef NPC_H
#define NPC_H

#include <string>
#include "dice.h"

class npc
{

public:
  std::string name;
  std::string color;
  std::string desc;
  int abilities;
  char symbol;
  dice speed; //Going to be dice.
  dice hp;
  dice ad;
  int rarity;

  npc();
  ~npc();
  void print_fields();
};
#endif