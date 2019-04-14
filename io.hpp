#ifndef IO_HPP
#define IO_HPP

#include "character.hpp"
#include "dungeon.hpp"

#define MOVE_INVALID 0
#define MOVE_VALID 1
#define MOVE_STAIR 2
#define QUIT 3
#define WIN 4
#define LOSE 5
#define INVALID_KEY 6
#define LIST_MONSTERS 7
#define TUNNELING_MAP 8
#define NON_TUNNELING_MAP 9
#define DEFAULT_MAP 10
#define FOG_TOGGLE 11
#define TELEPORT 12
#define ITEM_PICKUP 13

void io_init_terminal(void);
int move_pc(dungeon *d, character *pc, heap_t *mh, int fog);
void render_dungeon(dungeon *d, character *pc, heap_t *mh, int fog);
void render_dungeon_first(dungeon *d, character *pc, heap_t *mh, int fog);
void display_tunneling_map(dungeon *d);
void display_non_tunneling_map(dungeon *d);
void display_default_map(dungeon *d);
void game_over(int result);
void invalid_move();
void invalid_key();

#endif
