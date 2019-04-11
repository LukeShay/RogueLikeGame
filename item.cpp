#include "item.hpp"
#include <cstdlib>
#include <vector>

item::item() {}
item::~item() {}

void item::pick_location(uint8_t hardness_map[DUNGEON_Y][DUNGEON_X],
                         item *item_map[DUNGEON_Y][DUNGEON_X]) {
  uint8_t x, y;

  int i = 1;

  while (i) {
    x = rand() % (DUNGEON_X - 3) + 1;
    y = rand() % (DUNGEON_Y - 3) + 1;

    if (!item_map[y][x] && !hardness_map[y][x]) {
      this->x = x;
      this->y = y;
      i = 0;
    }
  }

  item_map[y][x] = this;
}
