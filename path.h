#ifndef PATH_H
# define PATH_H

#include "structs.h"
#include "priority_queue.h"

typedef struct neighbor {
	uint8_t x;
	uint8_t y;
	int c;
}neighbor_t;

void get_neighbors(uint8_t x, uint8_t y, dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], neighbor_t arr[]);
void non_tunneling_path(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], point_t pc);
void tunneling_path(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], point_t pc);

#endif
