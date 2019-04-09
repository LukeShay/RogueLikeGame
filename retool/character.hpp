#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "dice.hpp"
#include <string>

#define SMART 0x00000001
#define TELE 0x00000002
#define TUNNEL 0x00000004
#define ERRATIC 0x00000008
#define PASS 0x00000010
#define PICKUP 0x00000020
#define DESTROY 0x00000040
#define UNIQ 0x00000080
#define PC 0x00000100
#define DUNGEON_X 80
#define DUNGEON_Y 21

#define has_characteristic(characte, bit) (characte & bit)

class character {
public:
  std::string name;
  std::string color;
  std::string desc;
  uint8_t x, y;
  int abilities;
  char symbol;
  int speed;
  int hp;
  dice ad;
  int p;
  int direction;

  character();
  ~character();
  void pick_location(character *character_map[DUNGEON_Y][DUNGEON_X],
                     uint8_t hardness_map[DUNGEON_Y][DUNGEON_X]);
};

class character_desc {
public:
  std::string name;
  std::string color;
  std::string desc;
  std::string abilities;
  char symbol;
  dice speed;
  dice hp;
  dice ad;
  int rarity;

  int parse_abilities();
};

#endif
