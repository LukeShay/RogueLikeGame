#ifndef MONSTER_PARSER_H
#define MONSTER_PARSER_H

#include "heap.h"

#define characteristic(character, bit) \
  (character.abilities & ##bit)

void parse_monsters(heap_t *mv);

#endif