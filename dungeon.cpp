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

#include "character_utils.hpp"
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#define ROOMS_MIN 6
#define ROOMS_MAX 8
#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 5
#define ROOM_MAX_Y 4
#define FILE_SEMANTIC "RLG327-S2019"
#define FILE_PATH "/.rlg327/dungeon"

typedef enum dungeon_terrain {
  room_ter,
  corridor_ter,
  water_ter,
  sand_ter,
  up_stair_ter,
  down_stair_ter,
  portal_ter
} dungeon_terrain_t;

const char *terrain_char = ".#~*<>^";

#define hardnesspair(pair) (hardness_map[pair[dim_y]][pair[dim_x]])

int32_t corridor_path_cmp(const void *key, const void *with) {
  return ((corridor_path_t *)key)->cost - ((corridor_path_t *)with)->cost;
}

int32_t monster_cmp(const void *key, const void *with) {
  return ((character *)key)->p - ((character *)with)->p;
}

dungeon::dungeon(int num_lives, action_t action) {
  point_t pc;
  this->pc = new character;

  int x, y;
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      item_map[y][x] = NULL;
      character_map[y][x] = NULL;
    }
  }

  if (action == save) {
    pc = generate_dungeon();
  } else if (action == load) {
    pc = load_dungeon();
  } else if (action == loadSave) {
    pc = load_save_dungeon();
  } else {
    fprintf(stderr, "Invalid action parameter: %d", action);
    return;
  }

  pc_init(this->pc, pc, num_lives);
  heap_init(&mh, monster_cmp, NULL);

  this->character_map[this->pc->y][this->pc->x] = this->pc;

  init_pc_map(pc);
  tunneling_path(this, pc.xpos, pc.ypos);
  non_tunneling_path(this, pc.xpos, pc.ypos);

  update_pc_map(pc.xpos, pc.ypos);
}

dungeon::~dungeon() {
  int x, y;

  delete this->pc;
  heap_delete(&mh);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (item_map[y][x]) {
        assert(item_map[y][x] != NULL);
        delete item_map[y][x];
      }

      item_map[y][x] = NULL;
      character_map[y][x] = NULL;
    }
  }
}

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
      if (x == 0 || x == 20 || y == 0 || y == 79) {
        this->hardness_map[x][y] = 255;
      } else {
        this->hardness_map[x][y] = rand() % 254 + 1;
      }
      this->terrain_map[x][y] = ' ';
    }
  }

  num_rooms = generate_rooms(rooms);
  generate_corridors(rooms, num_rooms);
  up_stairs[0] = place_stairs(terrain_char[up_stair_ter], upStairSpot);
  if (num_up_stairs == 2)
    up_stairs[1] = place_stairs(terrain_char[up_stair_ter], upStairSpot * 2);
  down_stairs[0] = place_stairs(terrain_char[down_stair_ter], downStairSpot);
  if (num_down_stairs == 2)
    down_stairs[1] =
        place_stairs(terrain_char[down_stair_ter], downStairSpot * 2);
  pc = place_PC(pcSpot);
  generate_water();
  generate_sand();
  /*generate_portal();*/
  save_dungeon(rooms, up_stairs, down_stairs, pc, num_rooms, num_up_stairs,
               num_down_stairs);

  return pc;
}

void dungeon::update_pc_map(int px, int py) {
  int x, y;

  for (y = py - PC_RADIUS; y <= py + PC_RADIUS; y++) {
    for (x = px - PC_RADIUS; x <= px + PC_RADIUS; x++) {
      if (x < DUNGEON_X && x >= 0 && y < DUNGEON_Y && y >= 0) {
        if (this->terrain_map[y][x]) {
          this->pc_map[y][x] = this->terrain_map[y][x];
        }
      }
    }
  }
}

int dungeon::save_dungeon(room_t rooms[], stair_t up_stairs[],
                          stair_t down_stairs[], point_t pc, uint16_t num_rooms,
                          uint16_t num_up_stairs, uint16_t num_down_stairs) {
  FILE *f;
  char *directory = getenv("HOME");
  uint32_t temp32bit,
      fileSize =
          1708 + (num_rooms * 4) + ((num_up_stairs + num_down_stairs) * 2),
      version = 0;
  uint16_t temp16bit;
  uint8_t i, j;
  char *s = (char *)malloc(strlen(FILE_SEMANTIC) + 1);
  strcpy(s, FILE_SEMANTIC);

  char *path = (char *)malloc(strlen(directory) + strlen(FILE_PATH) + 1);
  strcpy(path, directory);
  strcat(path, FILE_PATH);

  if (!(f = fopen(path, "w"))) {
    fprintf(stderr, "Failed to open file");
    return -5;
  }

  free(path);

  // Writes file semantic.
  if (fwrite(s, 12, 1, f) != 1) {
    fprintf(stderr, "Failed to write semantic file-type.");
    return -6;
  }
  free(s);

  // Writes file version.
  if (fwrite(&version, sizeof(uint32_t), 1, f) != 1) {
    fprintf(stderr, "Failed to write version.");
    return -7;
  }

  temp32bit = htobe32(fileSize);
  // Writes fileSize.
  if (fwrite(&temp32bit, sizeof(uint32_t), 1, f) != 1) {
    fprintf(stderr, "Failed to write size.");
    return -8;
  }

  // Writes PC x position.
  if (fwrite(&pc.xpos, sizeof(pc.xpos), 1, f) != 1) {
    fprintf(stderr, "Failed to write pc xpos.");
    return -9;
  }

  // Writes PC y position.
  if (fwrite(&pc.ypos, sizeof(pc.ypos), 1, f) != 1) {
    fprintf(stderr, "Failed to write pc ypos.");
    return -10;
  }

  // Writes dungeon hardnesses.
  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      if (fwrite(&this->hardness_map[i][j], sizeof(this->hardness_map[i][j]), 1,
                 f) != 1) {
        fprintf(stderr, "Failed to write dungeon[%d][%d].", i, j);
        return -11;
      }
    }
  }

  // Writes the number of rooms.
  temp16bit = htobe16(num_rooms);
  if (fwrite(&temp16bit, sizeof(temp16bit), 1, f) != 1) {
    fprintf(stderr, "Failed to write number of rooms.");
    return -12;
  }

  // Writes the postion and sizes of each room.
  for (i = 0; i < num_rooms; i++) {
    if (fwrite(&rooms[i].xpos, sizeof(rooms[i].xpos), 1, f) != 1) {
      fprintf(stderr, "Failed to write room[%d].xpos.", i);
      return -13;
    }
    if (fwrite(&rooms[i].ypos, sizeof(rooms[i].ypos), 1, f) != 1) {
      fprintf(stderr, "Failed to write room[%d].ypos.", i);
      return -14;
    }
    if (fwrite(&rooms[i].xsize, sizeof(rooms[i].xsize), 1, f) != 1) {
      fprintf(stderr, "Failed to write room[%d].xsize.", i);
      return -15;
    }
    if (fwrite(&rooms[i].ysize, sizeof(rooms[i].ysize), 1, f) != 1) {
      fprintf(stderr, "Failed to write room[%d].ysize.", i);
      return -16;
    }
  }

  // Writes the number of up stairs.
  temp16bit = htobe16(num_up_stairs);
  if (fwrite(&temp16bit, sizeof(temp16bit), 1, f) != 1) {
    fprintf(stderr, "Failed to write number up_stairs.");
    return -17;
  }

  // Writes x and y of up stairs.
  for (i = 0; i < num_up_stairs; i++) {
    if (fwrite(&up_stairs[i].xpos, sizeof(up_stairs[i].xpos), 1, f) != 1) {
      fprintf(stderr, "Failed to write up-stair %d position.", i);
      return -18;
    }
    if (fwrite(&up_stairs[i].ypos, sizeof(up_stairs[i].ypos), 1, f) != 1) {
      fprintf(stderr, "Failed to write up-stair %d position.", i);
      return -18;
    }
  }

  // Writes the number of down stairs.
  temp16bit = htobe16(num_down_stairs);
  if (fwrite(&temp16bit, sizeof(temp16bit), 1, f) != 1) {
    fprintf(stderr, "Failed to write number down_stairs.");
    return -19;
  }

  // Writes x and y of down stairs.
  for (i = 0; i < num_down_stairs; i++) {
    if (fwrite(&down_stairs[i].xpos, sizeof(down_stairs[i].xpos), 1, f) != 1) {
      fprintf(stderr, "Failed to write down-stair %d position.", i);
      return -20;
    }
    if (fwrite(&down_stairs[i].ypos, sizeof(down_stairs[i].ypos), 1, f) != 1) {
      fprintf(stderr, "Failed to write up-stair %d position.", i);
      return -18;
    }
  }

  fclose(f);

  return 0;
}

point_t dungeon::load_dungeon() {
  FILE *f;
  char *directory = getenv("HOME");
  char *path =
      (char *)malloc(strlen(directory) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, directory);
  strcat(path, "/.rlg327/dungeon");
  point_t pc;
  pc.xpos = pc.ypos = 0;

  uint32_t version, fileSize;
  uint16_t num_rooms, num_up_stairs, num_down_stairs;
  uint16_t i, j;
  char s[12];

  if (!(f = fopen(path, "r"))) {
    fprintf(stderr, "Failed to open %s for read.", path);
    return pc;
  }

  free(path);

  // Reads semantic marker.
  if (!(fread(&s, sizeof(s), 1, f))) {
    fprintf(stderr, "Failed to read semantic marker from %s.\n", path);
    return pc;
  }

  if (!strcmp(s, "RLG327-S2190")) {
    fprintf(stderr,
            "Invalid file. This file has a different semantic mark: %s.\n", s);
    return pc;
  }

  // Reads file version.
  uint32_t temp32bit;
  if (!(fread(&temp32bit, sizeof(temp32bit), 1, f))) {
    fprintf(stderr, "Failed to read version from %s.\n", path);
    return pc;
  }
  version = be32toh(temp32bit);

  // Reads file size.
  if (!(fread(&temp32bit, sizeof(temp32bit), 1, f))) {
    fprintf(stderr, "Failed to read size from %s.\n", path);
    return pc;
  }
  fileSize = be32toh(temp32bit);
  // Reads position of PC.
  if (!(fread(&pc.xpos, sizeof(pc.xpos), 1, f))) {
    fprintf(stderr, "Failed to read pc.xpos from %s.\n", path);
    return pc;
  }

  if (!(fread(&pc.ypos, sizeof(pc.ypos), 1, f))) {
    fprintf(stderr, "Failed to read pc.ypos from %s.\n", path);
    return pc;
  }

  // Reads dungeon hardnesses.
  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      if (!(fread(&this->hardness_map[i][j], sizeof(pc.ypos), 1, f))) {
        fprintf(stderr, "Failed to read dungeon terrain [%d][%d] from %s.\n", i,
                j, path);
        return pc;
      }
    }
  }

  // Reads number of rooms.
  uint16_t temp16bit;
  if (!(fread(&temp16bit, sizeof(temp16bit), 1, f))) {
    fprintf(stderr, "Failed to read number of rooms from %s.\n", path);
    return pc;
  }
  num_rooms = be16toh(temp16bit);

  // Creates array of rooms.
  room_t rooms[num_rooms];

  // Reads room positions and sizes.
  for (i = 0; i < num_rooms; i++) {
    if (!(fread(&rooms[i].xpos, sizeof(rooms[i].xpos), 1, f))) {
      fprintf(stderr, "Failed to read rooms[%d].xpos from.\n", i);
      return pc;
    }
    if (!(fread(&rooms[i].ypos, sizeof(rooms[i].ypos), 1, f))) {
      fprintf(stderr, "Failed to read rooms[%d].ypos from.\n", i);
      return pc;
    }
    if (!(fread(&rooms[i].xsize, sizeof(rooms[i].xsize), 1, f))) {
      fprintf(stderr, "Failed to read rooms[%d].xsize from.\n", i);
      return pc;
    }
    if (!(fread(&rooms[i].ysize, sizeof(rooms[i].ysize), 1, f))) {
      fprintf(stderr, "Failed to read rooms[%d].ysize from.\n", i);
      return pc;
    }
  }

  // Reads number of up staircases
  if (!(fread(&temp16bit, sizeof(temp16bit), 1, f))) {
    fprintf(stderr, "Failed to read num_up_stairs from %s.\n", path);
    return pc;
  }
  num_up_stairs = be16toh(temp16bit);

  // Creates array for up stairs.
  struct stair up_stairs[num_up_stairs];

  // Reads positions of up stairs.
  for (i = 0; i < num_up_stairs; i++) {
    up_stairs[i].direction = '<';

    if (!(fread(&up_stairs[i].xpos, sizeof(up_stairs[i].xpos), 1, f))) {
      fprintf(stderr, "Failed to read up_stairs[%d].xpos from %s.\n", i, path);
      return pc;
    }
    if (!(fread(&up_stairs[i].ypos, sizeof(up_stairs[i].ypos), 1, f))) {
      fprintf(stderr, "Failed to read up_stairs[%d].ypos from %s.\n", i, path);
      return pc;
    }
  }

  // Reads number of down staircases.
  if (!(fread(&temp16bit, sizeof(temp16bit), 1, f))) {
    fprintf(stderr, "Failed to read num_down_stairs from %s.\n", path);
    return pc;
  }
  num_down_stairs = be16toh(temp16bit);
  // Creates array for up stairs.
  struct stair down_stairs[num_down_stairs];

  // Reads positions of up stairs.
  for (i = 0; i < num_down_stairs; i++) {
    down_stairs[i].direction = '>';

    if (!(fread(&down_stairs[i].xpos, sizeof(down_stairs[i].xpos), 1, f))) {
      fprintf(stderr, "Failed to read up_stairs[i].xpos from %s.\n", path);
      return pc;
    }
    if (!(fread(&down_stairs[i].ypos, sizeof(down_stairs[i].ypos), 1, f))) {
      fprintf(stderr, "Failed to read up_stairs[i].ypos from %s.\n", path);
      return pc;
    }
  }

  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      this->terrain_map[i][j] = ' ';
    }
  }

  add_dungeon_rooms(rooms, num_rooms);
  add_dungeon_stairs(up_stairs, num_up_stairs);
  add_dungeon_stairs(down_stairs, num_down_stairs);
  add_dungeon_corridor();

  return pc;
}

point_t dungeon::load_save_dungeon() {
  FILE *f;
  char *directory = getenv("HOME");
  char *path =
      (char *)malloc(strlen(directory) + strlen("/.rlg327/dungeon") + 1);
  strcpy(path, directory);
  strcat(path, "/.rlg327/dungeon");

  uint32_t version, fileSize;
  uint16_t num_rooms, num_up_stairs, num_down_stairs;
  uint16_t i, j;
  char s[12];
  point_t pc;
  pc.xpos = pc.ypos = 0;

  if (!(f = fopen(path, "r"))) {
    fprintf(stderr, "Failed to open %s for read.", path);
    return pc;
  }

  free(path);

  // Reads semantic marker.
  if (!(fread(&s, sizeof(s), 1, f))) {
    fprintf(stderr, "Failed to read semantic marker from %s.\n", path);
    return pc;
  }

  if (!strcmp(s, "RLG327-S2190")) {
    fprintf(stderr,
            "Invalid file. This file has a different semantic mark: %s.\n", s);
    return pc;
  }

  // Reads file version.
  uint32_t temp32bit;
  if (!(fread(&temp32bit, sizeof(temp32bit), 1, f))) {
    fprintf(stderr, "Failed to read version from %s.\n", path);
    return pc;
  }
  version = be32toh(temp32bit);

  // Reads file size.
  if (!(fread(&temp32bit, sizeof(temp32bit), 1, f))) {
    fprintf(stderr, "Failed to read size from %s.\n", path);
    return pc;
  }
  fileSize = be32toh(temp32bit);
  // Reads position of PC.
  if (!(fread(&pc.xpos, sizeof(pc.xpos), 1, f))) {
    fprintf(stderr, "Failed to read pc.xpos from %s.\n", path);
    return pc;
  }

  if (!(fread(&pc.ypos, sizeof(pc.ypos), 1, f))) {
    fprintf(stderr, "Failed to read pc.ypos from %s.\n", path);
    return pc;
  }

  // Reads dungeon hardnesses.
  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      if (!(fread(&this->hardness_map[i][j], sizeof(pc.ypos), 1, f))) {
        fprintf(stderr, "Failed to read dungeon terrain [%d][%d] from %s.\n", i,
                j, path);
        return pc;
      }
    }
  }

  // Reads number of rooms.
  uint16_t temp16bit;
  if (!(fread(&temp16bit, sizeof(temp16bit), 1, f))) {
    fprintf(stderr, "Failed to read number of rooms from %s.\n", path);
    return pc;
  }
  num_rooms = be16toh(temp16bit);

  // Creates array of rooms.
  room_t rooms[num_rooms];

  // Reads room positions and sizes.
  for (i = 0; i < num_rooms; i++) {
    if (!(fread(&rooms[i].xpos, sizeof(rooms[i].xpos), 1, f))) {
      fprintf(stderr, "Failed to read rooms[%d].xpos from.\n", i);
      return pc;
    }
    if (!(fread(&rooms[i].ypos, sizeof(rooms[i].ypos), 1, f))) {
      fprintf(stderr, "Failed to read rooms[%d].ypos from.\n", i);
      return pc;
    }
    if (!(fread(&rooms[i].xsize, sizeof(rooms[i].xsize), 1, f))) {
      fprintf(stderr, "Failed to read rooms[%d].xsize from.\n", i);
      return pc;
    }
    if (!(fread(&rooms[i].ysize, sizeof(rooms[i].ysize), 1, f))) {
      fprintf(stderr, "Failed to read rooms[%d].ysize from.\n", i);
      return pc;
    }
  }

  // Reads number of up staircases
  if (!(fread(&temp16bit, sizeof(temp16bit), 1, f))) {
    fprintf(stderr, "Failed to read num_up_stairs from %s.\n", path);
    return pc;
  }
  num_up_stairs = be16toh(temp16bit);

  // Creates array for up stairs.
  stair_t up_stairs[num_up_stairs];

  // Reads positions of up stairs.
  for (i = 0; i < num_up_stairs; i++) {
    up_stairs[i].direction = '<';

    if (!(fread(&up_stairs[i].xpos, sizeof(up_stairs[i].xpos), 1, f))) {
      fprintf(stderr, "Failed to read up_stairs[%d].xpos from %s.\n", i, path);
      return pc;
    }
    if (!(fread(&up_stairs[i].ypos, sizeof(up_stairs[i].ypos), 1, f))) {
      fprintf(stderr, "Failed to read up_stairs[%d].ypos from %s.\n", i, path);
      return pc;
    }
  }

  // Reads number of down staircases.
  if (!(fread(&temp16bit, sizeof(temp16bit), 1, f))) {
    fprintf(stderr, "Failed to read num_down_stairs from %s.\n", path);
    return pc;
  }
  num_down_stairs = be16toh(temp16bit);
  // Creates array for up stairs.
  stair_t down_stairs[num_down_stairs];

  // Reads positions of up stairs.
  for (i = 0; i < num_down_stairs; i++) {
    down_stairs[i].direction = '>';

    if (!(fread(&down_stairs[i].xpos, sizeof(down_stairs[i].xpos), 1, f))) {
      fprintf(stderr, "Failed to read up_stairs[i].xpos from %s.\n", path);
      return pc;
    }
    if (!(fread(&down_stairs[i].ypos, sizeof(down_stairs[i].ypos), 1, f))) {
      fprintf(stderr, "Failed to read up_stairs[i].ypos from %s.\n", path);
      return pc;
    }
  }

  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      this->terrain_map[i][j] = ' ';
    }
  }

  // Fills in the dungeon.
  add_dungeon_rooms(rooms, num_rooms);
  add_dungeon_stairs(up_stairs, num_up_stairs);
  add_dungeon_stairs(down_stairs, num_down_stairs);
  add_dungeon_corridor();
  save_dungeon(rooms, up_stairs, down_stairs, pc, num_rooms, num_up_stairs,
               num_down_stairs);

  return pc;
}

uint32_t dungeon::adjacent_to_room(int16_t y, int16_t x) {
  return (this->terrain_map[y][x - 1] == '.' ||
          this->terrain_map[y][x + 1] == '.' ||
          this->terrain_map[y - 1][x] == '.' ||
          this->terrain_map[y + 1][x] == '.' ||
          this->terrain_map[y][x - 1] == '#' ||
          this->terrain_map[y][x + 1] == '#' ||
          this->terrain_map[y - 1][x] == '#' ||
          this->terrain_map[y + 1][x] == '#');
}

uint32_t dungeon::is_open_space(int16_t y, int16_t x) {
  return !this->hardness_map[y][x];
}

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
            this->terrain_map[y][z] = terrain_char[room_ter];
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

void dungeon::dijkstra_corridor_inv(point_t from, point_t to) {
  static corridor_path_t path[DUNGEON_Y][DUNGEON_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      path[y][x].cost = INT_MAX;
    }
  }

  path[from.ypos][from.xpos].cost = 0;

  heap_init(&h, corridor_path_cmp, NULL);

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (this->hardness_map[y][x] != 255) {
        path[y][x].hn = heap_insert(&h, &path[y][x]);
      } else {
        path[y][x].hn = NULL;
      }
    }
  }

  while ((p = (corridor_path_t *)heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to.ypos) && p->pos[dim_x] == to.xpos) {
      for (x = to.xpos, y = to.ypos; (x != from.xpos) || (y != from.ypos);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        if (this->terrain_map[y][x] != terrain_char[room_ter]) {
          this->terrain_map[y][x] = terrain_char[corridor_ter];
          this->hardness_map[y][x] = 0;
        }
      }
      heap_delete(&h);
      return;
    }

#define hardnesspair_inv(p)                                                    \
  (is_open_space(p[dim_y], p[dim_x])                                           \
       ? 127                                                                   \
       : (adjacent_to_room(p[dim_y], p[dim_x]) ? 191                           \
                                               : (255 - hardnesspair(p))))

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]].cost =
          p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
          p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
          p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
         p->cost + hardnesspair_inv(p->pos))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]].cost =
          p->cost + hardnesspair_inv(p->pos);
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h,
                                   path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
    }
  }
}

void dungeon::generate_corridors(room_t rooms[], uint16_t num_rooms) {
  int i;
  point_t start_point, end_point;

  for (i = 0; i < num_rooms - 1; i++) {
    start_point.xpos = rand() % rooms[i].xsize + rooms[i].xpos;
    start_point.ypos = rand() % rooms[i].ysize + rooms[i].ypos;

    end_point.xpos = rand() % rooms[i + 1].xsize + rooms[i + 1].xpos;
    end_point.ypos = rand() % rooms[i + 1].ysize + rooms[i + 1].ypos;

    dijkstra_corridor_inv(start_point, end_point);
  }
}

stair_t dungeon::place_stairs(char direction, uint8_t spot) {
  uint8_t x, y, z = 0;
  stair_t staircase;

  for (x = 1; x < DUNGEON_Y; x++) {
    for (y = 1; y < DUNGEON_X; y++) {

      if ((this->terrain_map[x][y] == terrain_char[room_ter] ||
           this->terrain_map[x][y] == terrain_char[corridor_ter]) &&
          this->terrain_map[x][y] != terrain_char[down_stair_ter] &&
          this->terrain_map[x][y] != terrain_char[up_stair_ter])
        z++;

      if (z == spot) {
        this->terrain_map[x][y] = direction;
        staircase.ypos = x;
        staircase.xpos = y;
        staircase.direction = direction;
        break;
      }
    }
    if (x == 20) {
      x = 1;
    }
    if (y == 79) {
      y = 1;
    }
    if (z == spot) {
      break;
    }
  }

  return staircase;
}

point_t dungeon::place_PC(uint8_t spot) {
  uint8_t x, y, z = 0;
  point_t pc;

  for (x = 1; x < DUNGEON_Y; x++) {
    for (y = 1; y < DUNGEON_X; y++) {

      if (this->terrain_map[x][y] == '.' || this->terrain_map[x][y] == '#')
        z++;

      if (z == spot) {
        pc.ypos = x;
        pc.xpos = y;
        break;
      }
    }
    if (x == 20)
      x = 1;
    if (y == 79)
      y = 1;
    if (z == spot)
      break;
  }

  return pc;
}

void dungeon::add_dungeon_rooms(room_t rooms[], uint16_t num_rooms) {
  int i, j, k;
  for (i = 0; i < num_rooms; i++) {
    for (j = rooms[i].xpos; j < rooms[i].xpos + rooms[i].xsize; j++) {
      for (k = rooms[i].ypos; k < rooms[i].ypos + rooms[i].ysize; k++) {
        if (k != 0 && k < 20 && j != 0 && j < 79)
          this->terrain_map[k][j] = '.';
      }
    }
  }
}

void dungeon::add_dungeon_stairs(stair_t stairs[], uint16_t num_stairs) {
  int i;
  for (i = 0; i < num_stairs; i++) {
    this->terrain_map[stairs[i].ypos][stairs[i].xpos] = stairs[i].direction;
  }
}

void dungeon::add_dungeon_corridor() {
  int i, j;
  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      if (this->hardness_map[i][j] == 0 && this->terrain_map[i][j] == ' ')
        this->terrain_map[i][j] = '#';
    }
  }
}

void dungeon::add_PC(point_t pc) {
  int i, j;
  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      if (this->hardness_map[i][j] == 0 && this->terrain_map[i][j] == ' ')
        this->terrain_map[i][j] = '#';
    }
  }
}

void dungeon::init_pc_map(point_t pc) {
  int x, y;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      this->pc_map[y][x] = ' ';
    }
  }
}

void get_neighbors(uint8_t x, uint8_t y, neighbor_t arr[]) {
  uint8_t i, j, k = 0;

  for (i = y - 1; i <= y + 1; i++) {
    for (j = x - 1; j <= x + 1; j++) {
      if (!(i == y && j == x)) {
        arr[k].x = j;
        arr[k].y = i;
        k++;
      }
    }
  }
}

void non_tunneling_path(dungeon *d, int xp, int yp) {
  uint8_t i, j;

  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      d->cost_nt_map[i][j] = INT_MAX;
    }
  }

  d->cost_nt_map[yp][xp] = 0;
  uint8_t x, y;
  int c;
  neighbor_t neighbors[8];

  queue_t pq;
  queue_init(&pq);
  queue_push(&pq, xp, yp, 0);
  while (queue_size(&pq)) {
    queue_pop(&pq, &x, &y, &c);
    get_neighbors(x, y, neighbors);

    for (i = 0; i < 8; i++) {
      if (d->hardness_map[neighbors[i].y][neighbors[i].x] == 0 &&
          c + 1 < d->cost_nt_map[neighbors[i].y][neighbors[i].x]) {
        d->cost_nt_map[neighbors[i].y][neighbors[i].x] = c + 1;
        queue_push(&pq, neighbors[i].x, neighbors[i].y, c + 1);
      }
    }
  }

  queue_delete(&pq);
}

void tunneling_path(dungeon *d, int xp, int yp) {
  uint8_t i, j;
  uint8_t x, y;
  int c;
  int temp_c;
  neighbor_t neighbors[8];
  queue_t pq;

  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      d->cost_t_map[i][j] = INT_MAX;
    }
  }

  d->cost_t_map[yp][xp] = 0;

  queue_init(&pq);
  queue_push(&pq, xp, yp, 0);

  while (queue_size(&pq) != 0) {
    queue_pop(&pq, &x, &y, &c);
    get_neighbors(x, y, neighbors);

    for (i = 0; i < 8; i++) {
      temp_c = c + (d->hardness_map[y][x] / 85) + 1;

      if (neighbors[i].x != 0 && neighbors[i].x != DUNGEON_X &&
          neighbors[i].y != 0 && neighbors[i].y != DUNGEON_Y &&
          temp_c < d->cost_t_map[neighbors[i].y][neighbors[i].x] &&
          d->hardness_map[neighbors[i].y][neighbors[i].x] != 255) {
        d->cost_t_map[neighbors[i].y][neighbors[i].x] = temp_c;
        queue_push(&pq, neighbors[i].x, neighbors[i].y, temp_c);
      }
    }
  }
  queue_delete(&pq);
}

void clear() {
  for (int y = 0; y < DUNGEON_Y; y++) {
    for (int x = 0; x < DUNGEON_X; x++) {
    }
  }
}

void dungeon::generate_water() {
  int water_x, water_y, water_start = rand() % 1000, v, w, x, y, i = 0;

  for (y = 3; y < DUNGEON_Y - 3; y++) {
    for (x = 6; x < DUNGEON_X - 6; x++) {
      i++;

      if (i == water_start) {
        i = 0;
        water_x = rand() % 2 + 4;
        water_y = rand() % 2 + 2;

        for (v = y; v < y + water_y && v < DUNGEON_Y; v++) {
          for (w = x; w < x + water_x && w < DUNGEON_X; w++) {
            if (terrain_map[v][w] != terrain_char[up_stair_ter] &&
                terrain_map[v][w] != terrain_char[down_stair_ter]) {
              terrain_map[v][w] = terrain_char[water_ter];
              hardness_map[v][w] = 0;
            }
          }
        }
        goto done_placing_water;
      }
    }
  }
done_placing_water:;
}

void dungeon::generate_sand() {
  int sand_x, sand_y, sand_start = rand() % 1000, v, w, x, y, i = 0;

  for (y = 3; y < DUNGEON_Y - 3; y++) {
    for (x = 6; x < DUNGEON_X - 6; x++) {
      if (terrain_map[y][x] != terrain_char[water_ter]) {
        i++;
      }

      if (i == sand_start) {
        i = 0;
        sand_x = rand() % 2 + 4;
        sand_y = rand() % 2 + 2;

        for (v = y; v < y + sand_y && v < DUNGEON_Y; v++) {
          for (w = x; w < x + sand_x && w < DUNGEON_X; w++) {
            if (terrain_map[v][w] != terrain_char[up_stair_ter] &&
                terrain_map[v][w] != terrain_char[down_stair_ter]) {
              terrain_map[v][w] = terrain_char[sand_ter];
              hardness_map[v][w] = 0;
            }
          }
        }
        goto done_placing_water;
      }
    }
  }
done_placing_water:;
}
