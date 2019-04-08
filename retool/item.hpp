#ifndef ITEM_HPP
#define ITEM_HPP
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
  std::string artifact_status;
  int rarity;
  uint8_t x;
  uint8_t y;

  item();
  ~item();
  void pick_location(item *item_map[DUNGEON_Y][DUNGEON_X]);
};

#endif
