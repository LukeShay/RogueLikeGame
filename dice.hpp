#ifndef DICE_HPP
#define DICE_HPP

#include <string>

class dice {
public:
  int base;
  int num;
  int sides;
  std::string form;

  dice();
  dice(int base, int num, int sides);
  void parse_dice(std::string s);
  int roll_dice();
};
#endif
