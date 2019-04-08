#ifndef PATH_HPP
#define PATH_HPP

#include "../priority_queue.h"
#include "dungeon.hpp"

typedef struct neighbor {
  uint8_t x;
  uint8_t y;
  int c;
} neighbor_t;

void get_neighbors(uint8_t x, uint8_t y, neighbor_t arr[]);
void non_tunneling_path(dungeon *d, point_t pc);
void tunneling_path(dungeon *d, point_t pc);

#endif
