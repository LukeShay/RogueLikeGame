#ifndef CHARACTER_UTILS_HPP
#define CHARACTER_UTILS_HPP

#include "dungeon.hpp"
#include <vector>

void generate_monsters(dungeon *d, heap_t *mh, std::vector<character_desc> *mv);
void pc_init(character *pc_char, point_t pc, int num_lives);
void add_to_character_map(dungeon *d, character *characters);
void move_monster(dungeon *d, character *to_move, character *pc);

#endif
