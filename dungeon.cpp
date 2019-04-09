#include "dungeon.hpp"
#ifdef __APPLE__
#include <libkern/OSByteOrder.h>
#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)
#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)
#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)
#else
#include <endian.h>
#endif

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdint.h>

#define ROOMS_MIN 6
#define ROOMS_MAX 8
#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 5
#define ROOM_MAX_Y 4
#define FILE_SEMANTIC "RLG327-S2019"
#define FILE_PATH "/.rlg327/dungeon"

int32_t corridor_path_cmp(const void *key, const void *with) {}

static int count_num_spaces(dungeon *d) {
  int i, j, num = 0;

  for (i = 1; i < DUNGEON_Y - 1; i++) {
    for (j = 1; j < DUNGEON_X - 1; j++) {
      if (d->terrain_map[i][j] != ' ')
        num++;
    }
  }
  return num;
}

dungeon::dungeon(heap_t *mh, character_t *pc_char, int num_lives, int num_mon,
                 action_t action) {
  int num_spaces = 0, num;
  point_t pc;

  heap_delete(mh);
  heap_init(mh, monster_cmp, NULL);

  if (action == save || action == savenummon || action == num_mon) {
    pc = generate_dungeon(); // TODO: Change to be named save_dungeon.
  } else if (action == load || action == loadnummon) {
    pc = load_dungeon();
  } else if (action == loadSave || action == loadSavenummon) {
    pc = load_save_dungeon();
  } else {
    fprintf(stderr, "Invalid action parameter: %d", action);
    return;
  }

  if (action == savenummon || action == loadnummon ||
      action == loadSavenummon) {
    num_spaces = count_num_spaces(this);
    num = num_mon < num_spaces ? num_mon : num_spaces;
  } else {
    num = rand() % 5 + 8;
  }

  pc_init(pc_char, pc, mh, num_lives);
  generate_monsters(pc, num_spaces, num, mh);

  /*non_tunneling_path(pc);
  tunneling_path(pc);*/

  init_pc_map(pc);
  update_pc_map(pc);
}

dungeon::~dungeon() {}

point_t dungeon::generate_dungeon() {
  room_t rooms[ROOMS_MAX];
  uint16_t num_rooms = 0;
  point_t pc;

  uint8_t x, y, upStairSpot = rand() % 200, downStairSpot = rand() % 200,
                pcSpot = rand() % 200, num_up_stairs = rand() % 2 + 1,
                num_down_stairs = rand() % 2 + 1;
  stair_t up_stairs[num_up_stairs], down_stairs[num_down_stairs];

  while (upStairSpot == downStairSpot)
    downStairSpot = rand() % 200;

  // Adds the boundaries and hardness to the dungeon arrays.
  for (x = 0; x < DUNGEON_Y; x++) {
    for (y = 0; y < DUNGEON_X; y++) {
      if (x == 0 || x == 20) {
        this->hardness_map[x][y] = 255;
      } else if (y == 0 || y == 79) {
        this->hardness_map[x][y] = 255;
      } else {
        this->hardness_map[x][y] = rand() % 254 + 1;
      }
      this->terrain_map[x][y] = ' ';
    }
  }

  num_rooms = generate_rooms(rooms);
  generate_corridors(rooms, num_rooms);
  up_stairs[0] = place_stairs('<', upStairSpot);
  if (num_up_stairs == 2)
    up_stairs[1] = place_stairs('<', upStairSpot * 2);
  down_stairs[0] = place_stairs('>', downStairSpot);
  if (num_down_stairs == 2)
    down_stairs[1] = place_stairs('>', downStairSpot * 2);
  pc = place_PC(pcSpot);
  // save_dungeon(rooms, up_stairs, down_stairs, pc, num_rooms, num_up_stairs,
  //             num_down_stairs);

  return pc;
}

void dungeon::update_pc_map(point_t pc) {}
int dungeon::save_dungeon(room_t rooms[], stair_t upStairs[],
                          stair_t downStairs[], point_t pc, uint16_t num_rooms,
                          uint16_t numUpStairs, uint16_t numDownStairs) {}
point_t dungeon::load_dungeon() {}
point_t dungeon::load_save_dungeon() {}
uint32_t dungeon::adjacent_to_room(int16_t y, int16_t x) {}
uint32_t dungeon::is_open_space(int16_t y, int16_t x) {}
uint16_t dungeon::generate_rooms(room_t rooms[]) {
  uint8_t x, y, z, l;
  uint16_t num_rooms = 0;
  int numRedos = 0;
  bool yMatch = false, xMatch = false;
  // Generates the random location and size of the rooms.
  for (x = 0; x < ROOMS_MAX; x++) {
    rooms[x].xpos = rand() % 69 + 1;
    rooms[x].ypos = rand() % 13 + 1;
    rooms[x].ysize = rand() % ROOM_MAX_Y + ROOM_MIN_Y;
    rooms[x].xsize = rand() % ROOM_MAX_X + ROOM_MIN_X;

    // Checks to see if the current room intersects with any previous rooms.
    for (z = 0; z < x; z++) {
      // Checks y-axis points.
      for (y = rooms[z].ypos - 1; y <= rooms[z].ysize + rooms[z].ypos + 1;
           y++) {
        for (l = rooms[x].ypos; l <= rooms[x].ysize + rooms[x].ypos; l++) {
          yMatch = y == l;
          if (yMatch)
            break;
        }
        if (yMatch)
          break;
      }

      // Checks x-axis points.
      for (y = rooms[z].xpos - 1; y <= rooms[z].xsize + rooms[z].xpos + 1;
           y++) {
        for (l = rooms[x].xpos; l <= rooms[x].xsize + rooms[x].xpos; l++) {
          xMatch = y == l;
          if (xMatch)
            break;
        }
        if (xMatch)
          break;
      }

      if (xMatch && yMatch)
        break;
    }

    // If they match on both axis, the current room is remade, else it gets
    // added to the dungeon array.
    if (xMatch && yMatch) {
      numRedos++;
      if (numRedos >= 2000 && num_rooms >= ROOMS_MIN) {
        rooms[x].xpos = 0;
        rooms[x].ypos = 0;
        rooms[x].xsize = 0;
        rooms[x].ysize = 0;
        break;
      }
      x--;
    } else {
      for (y = rooms[x].ypos; y < rooms[x].ypos + rooms[x].ysize; y++) {
        for (z = rooms[x].xpos; z < rooms[x].xpos + rooms[x].xsize; z++) {
          if (y < 20 && z < 79) {
            this->terrain_map[y][z] = '.';
            this->hardness_map[y][z] = 0;
          }
        }
      }
      numRedos = 0;
      num_rooms++;
    }
  } // End loop for creating rooms.
  return num_rooms;
}
void dungeon::dijkstra_corridor_inv(point_t from, point_t to) {}
void dungeon::generate_corridors(room_t rooms[], uint16_t num_rooms) {}
stair_t dungeon::place_stairs(char direction, uint8_t spot) {}
point_t dungeon::place_PC(uint8_t spot) {}
void dungeon::set_hardness() {}
void dungeon::add_dungeon_rooms(room_t rooms[], uint16_t numRooms) {}
void dungeon::add_dungeon_stairs(stair_t stairs[], uint16_t numStairs) {}
void dungeon::add_dungeon_corridor() {}
void dungeon::add_PC(point_t pc) {}
void init_pc_map(point_t pc) {}
