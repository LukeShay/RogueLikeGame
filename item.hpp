#ifndef ITEM_HPP
#define ITEM_HPP

#include "character.hpp"
#include "dice.hpp"
#include <string>

#define DUNGEON_X 80
#define DUNGEON_Y 21

class item {
public:
  std::string name;
  std::string desc;
  std::string type;
  std::string color;
  int hit_bonus;
  int damage_bonus;
  int dodge_bonus;
  int defense_bonus;
  int weight;
  int speed_bonus;
  int special_attr;
  int value;
  uint8_t x;
  uint8_t y;

  item();
  ~item();
  void pick_location(uint8_t hardness_map[DUNGEON_Y][DUNGEON_X],
                     item *item_map[DUNGEON_Y][DUNGEON_X]);
};

class item_desc {
public:
  std::string name;
  std::string desc;
  std::string type;
  std::string color;
  dice hit_bonus;
  dice damage_bonus;
  dice dodge_bonus;
  dice defense_bonus;
  dice weight;
  dice speed_bonus;
  dice special_attr;
  dice value;
  int rarity;
  std::string artifact_status;
  int placed; // 0 = no, 1 = yes Only used for artifacts
};

#endif
