#include "characters.h"
#include "heap.h"
#include "path.h"
#include "structs.h"

#ifndef PATHFINDER_H
#define PATHFINDER_H

typedef struct stair {
  char direction;
  uint8_t xpos, ypos;
} stair_t;

typedef struct room {
  uint8_t xpos, ypos, xsize, ysize;
} room_t;

#define hardnesspair(pair) (d[pair[dim_y]][pair[dim_x]].hardness)

// From Dr. Sheaffer.
int32_t corridor_path_cmp(const void *key, const void *with);
// From Dr. Sheaffer.
uint32_t adjacent_to_room(dungeon_space_t d[][DUNGEON_X], int16_t y, int16_t x);
// From Dr. Sheaffer.
uint32_t is_open_space(dungeon_space_t d[][DUNGEON_X], int16_t y, int16_t x);
uint16_t generate_rooms(room_t rooms[],
                        dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X]);
// From Dr. Sheaffer.
void dijkstra_corridor_inv(dungeon_space_t d[][DUNGEON_X], point_t from,
                           point_t to);

#define hardnesspair_inv(p)                                                    \
  (is_open_space(d, p[dim_y], p[dim_x])                                        \
       ? 127                                                                   \
       : (adjacent_to_room(d, p[dim_y], p[dim_x]) ? 191                        \
                                                  : (255 - hardnesspair(p))))
void generate_corridors(room_t rooms[],
                        dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                        uint16_t num_rooms);
stair_t place_stairs(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                     char direction, uint8_t spot);
point_t place_PC(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], uint8_t spot);
void set_hardness(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X]);
void add_dungeon_rooms(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                       room_t rooms[], uint16_t numRooms);
void add_dungeon_stairs(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                        stair_t stairs[], uint16_t numStairs);
void add_dungeon_corridor(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X]);
void add_PC(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], point_t pc);
void update_pc_map(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], point_t pc);
void new_dungeon(dungeon_space_t dungeon[][DUNGEON_X],
                 character_t *character_map[][DUNGEON_X], heap_t *mh,
                 character_t *pc_char, int num_lives, int num_mon,
                 action_t action);
void first_dungeon(dungeon_space_t dungeon[][DUNGEON_X],
                   character_t *character_map[][DUNGEON_X], heap_t *mh,
                   character_t *pc_char, int num_lives, int num_mon,
                   action_t action);
int save_dungeon(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], room_t rooms[],
                 stair_t upStairs[], stair_t downStairs[], point_t pc,
                 uint16_t num_rooms, uint16_t numUpStairs,
                 uint16_t numDownStairs);
point_t generate_dungeon(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X]);
point_t load_dungeon(dungeon_space_t dungeon[][DUNGEON_X]);
point_t load_save_dungeon(dungeon_space_t dungeon[][DUNGEON_X]);

#endif
