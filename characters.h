#include "structs.h"

#ifndef CHARACTERS_H
#define CHARACTERS_H

void generate_monsters(point_t pc,
                       character_t *character_map[DUNGEON_Y][DUNGEON_X],
                       dungeon_space_t dungeon[][DUNGEON_X], int num_spaces,
                       int num_mon, heap_t *mh);
void pc_init(character_t *pc_char, point_t pc,
             character_t *character_map[DUNGEON_Y][DUNGEON_X],
             dungeon_space_t dungeon[][DUNGEON_X], heap_t *mh, int num_lives);
void place_monster(character_t *character, dungeon_space_t dungeon[][DUNGEON_X],
                   character_t *character_map[][DUNGEON_X], int num_spaces,
                   int num_mon);
void add_to_character_map(character_t *character_map[][DUNGEON_X],
                          character_t *characters);
void queue_monster(heap_t *mh, character_t *character);
void move_character(dungeon_space_t dungeon[][DUNGEON_X],
                    character_t *character_map[][DUNGEON_X],
                    character_t *to_move, character_t *pc);

#endif
