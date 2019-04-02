#ifndef MONSTER_PARSER_H
#define MONSTER_PARSER_H

#include "heap.h"

#define NAME "NAME"
#define DESCRIPTION "DESC"
#define COLOR "COLOR"
#define SPEED "SPEED"
#define ABILITIES "ABIL"
#define HITPOINTS "HP"
#define DAMAGE "DAM"
#define SYMBOL "SYMB"
#define RARITY "RRTY"

#define SMART 0x00000001
#define TELE 0x00000002
#define TUNNEL 0x00000004
#define ERRATIC 0x00000008

#define has_characteristic(character, bit) \
  (character.abilities & ##bit)

void parse_monsters(const char *file_name, heap_t *mh);

#endif