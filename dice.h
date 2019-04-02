#ifndef DICE_H
#define DICE_H

#include <string>

class dice
{
public:
  int base;
  int num;
  int sides;
  std::string form;

  dice();
  dice(int base, int num, int sides);
  void parse_dice(std::string s);
};
#endif