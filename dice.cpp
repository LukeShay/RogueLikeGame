#include "dice.hpp"

dice::dice() {}

dice::dice(int base, int num, int sides) {
  this->base = base;
  this->num = num;
  this->sides = sides;

  this->form = "" + std::to_string(base) + '+' + std::to_string(num) + 'd' +
               std::to_string(sides);
}

void dice::parse_dice(std::string s) {
  int i, plus, d;

  for (i = 0; i < (int)s.length(); i++) {
    if (s.at(i) == '+') {
      plus = i;
    } else if (s.at(i) == 'd') {
      d = i;
    }
  }

  this->base = std::stoi(s.substr(0, plus));
  this->num = std::stoi(s.substr(plus + 1, d));
  this->sides = std::stoi(s.substr(d + 1));

  this->form = s;
}

int dice::roll_dice() {
  int num = this->base, i;

  for (i = 0; i < this->num; i++) {
    num += rand() % this->sides + 1;
  }

  return num;
}
