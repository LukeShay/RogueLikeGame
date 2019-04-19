#ifndef CHARACTER_UTILS_HPP
#define CHARACTER_UTILS_HPP

#include "dungeon.hpp"
#include "heap.h"
#include <vector>

typedef enum equiped {
  weapon,
  offhand,
  ranged,
  armor,
  helmet,
  cloak,
  gloves,
  boots,
  amulet,
  light,
  ring_1,
  ring_2
} equiped_t;

void generate_monsters(dungeon *d, heap_t *mh, std::vector<character_desc> *mv);
void pc_init(character *pc_char, point_t pc, int num_lives);
void move_monster(dungeon *d, character *to_move);
int item_slot(std::string type);
int empty_inventory_slot(item *arr[]);

#endif
