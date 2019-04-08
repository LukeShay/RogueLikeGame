#include "item.hpp"

item::item() {}
item::~item() {}

void item::pick_location(item *item_map[DUNGEON_Y][DUNGEON_X]) {
  int x;
  int y;

  int i = 1;

  while (i) {
    x = rand() % (DUNGEON_X - 3) + 1;
    y = rand() % (DUNGEON_Y - 3) + 1;

    if (!item_map[y][x]) {
      this->x = x;
      this->y = y;
      i = 0;
    }
  }
}
