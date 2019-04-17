#ifndef PATHFINDER_HPP
#define PATHFINDER_HPP

#include "character.hpp"
#include "heap.h"
#include "item.hpp"
#include "priority_queue.hpp"
#include <cstdint>
#include <vector>

#define DUNGEON_X 80
#define DUNGEON_Y 21
#define PC_RADIUS 4

typedef struct stair {
  char direction;
  uint8_t xpos, ypos;
} stair_t;

typedef struct point {
  uint8_t xpos, ypos;
} point_t;

typedef struct room {
  uint8_t xpos, ypos, xsize, ysize;
} room_t;

typedef struct corridor_path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} corridor_path_t;

typedef enum action {
  save,
  load,
  loadSave,
  savenummon,
  loadnummon,
  loadSavenummon
} action_t;

typedef enum dim { dim_x, dim_y, num_dims } dim_t;

class dungeon {
public:
  char terrain_map[DUNGEON_Y][DUNGEON_X];
  uint8_t hardness_map[DUNGEON_Y][DUNGEON_X];
  character *character_map[DUNGEON_Y][DUNGEON_X];
  item *item_map[DUNGEON_Y][DUNGEON_X];
  char pc_map[DUNGEON_Y][DUNGEON_X];
  int cost_t_map[DUNGEON_Y][DUNGEON_X];
  int cost_nt_map[DUNGEON_Y][DUNGEON_X];
  std::vector<item_desc> iv;
  std::vector<character_desc> mv;

  dungeon(character *pc_char, int num_lives, int num_mon, action_t action);
  ~dungeon();
  void update_pc_map(int px, int py);
  void clear();

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
  void add_dungeon_rooms(room_t rooms[], uint16_t numRooms);
  void add_dungeon_stairs(stair_t stairs[], uint16_t numStairs);
  void add_dungeon_corridor();
  void add_PC(point_t pc);
  void init_pc_map(point_t pc);
};

typedef struct neighbor {
  uint8_t x;
  uint8_t y;
  int c;
} neighbor_t;

void get_neighbors(uint8_t x, uint8_t y, neighbor_t arr[]);
void non_tunneling_path(dungeon *d, int xp, int yp);
void tunneling_path(dungeon *d, int xp, int yp);

#endif
