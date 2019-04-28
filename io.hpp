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
#define GAME_OVER 14
#define MOVE_TRAPPED 15

void io_init_terminal(void);
int move_pc(dungeon *d, character *pc, heap_t *mh, std::vector<item_desc> *iv,
            int fog);
void render_dungeon(dungeon *d, character *pc, int fog, std::string message);
void game_over(int result);
void display_message(int x, int y, char *s);

#endif
