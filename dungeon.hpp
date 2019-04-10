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

typedef enum dim { dim_x, dim_y, num_dims } dim_t;

#define hardnesspair_inv(p)                                                    \
  (is_open_space(d, p[dim_y], p[dim_x])                                        \
       ? 127                                                                   \
       : (adjacent_to_room(d, p[dim_y], p[dim_x]) ? 191                        \
                                                  : (255 - hardnesspair(p))))

#define hardnesspair(pair) (d[pair[dim_y]][pair[dim_x]].hardness)

class dungeon {
public:
  char terrain_map[DUNGEON_Y][DUNGEON_X];
  uint8_t hardness_map[][DUNGEON_Y][DUNGEON_X];
  char *pc_map[DUNGEON_Y][DUNGEON_X];
  int cost_t_map[DUNGEON_Y][DUNGEON_X];
  int cost_nt_map[DUNGEON_Y][DUNGEON_X];

  dungeon(heap_t *mh, character_t *pc_char, int num_lives, int num_mon,
          action_t action);
  ~dungeon();
  void update_pc_map(int px, int py);

private:
  int save_dungeon(room_t rooms[], stair_t upStairs[], stair_t downStairs[],
                   point_t pc, uint16_t num_rooms, uint16_t numUpStairs,
                   uint16_t numDownStairs);
  point_t generate_dungeon();
  point_t load_dungeon();
  point_t load_save_dungeon();
  uint32_t adjacent_to_room(int16_t y, int16_t x);
  uint32_t is_open_space(int16_t y, int16_t x);
  uint16_t generate_rooms(room_t rooms[]);
  void dijkstra_corridor_inv(point_t from, point_t to);
  void generate_corridors(room_t rooms[], uint16_t num_rooms);
  stair_t place_stairs(char direction, uint8_t spot);
  point_t place_PC(uint8_t spot);
  void set_hardness();
  void add_dungeon_rooms(room_t rooms[], uint16_t numRooms);
  void add_dungeon_stairs(stair_t stairs[], uint16_t numStairs);
  void add_dungeon_corridor();
  void add_PC(point_t pc);
  void init_pc_map(point_t pc);
};

#endif
