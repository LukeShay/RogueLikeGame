#ifndef IO_HPP
#define IO_HPP

#include "character.hpp"
#include "dungeon.hpp"

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
