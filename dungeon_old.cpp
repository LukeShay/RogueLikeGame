#include "dungeon.h"
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

#include "priority_queue.h"

#define ROOMS_MIN 6
#define ROOMS_MAX 8
#define ROOM_MIN_X 4
#define ROOM_MIN_Y 3
#define ROOM_MAX_X 5
#define ROOM_MAX_Y 4
#define FILE_SEMANTIC "RLG327-S2019"
#define FILE_PATH "/.rlg327/dungeon"

typedef struct corridor_path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} corridor_path_t;

typedef enum dim { dim_x, dim_y, num_dims } dim_t;

#define has_characteristic(character, bit) (character & C_##bit)

// From Dr. Sheaffer.
#define hardnesspair(pair) (d[pair[dim_y]][pair[dim_x]].hardness)

int32_t monster_cmp(const void *key, const void *with) {
  return ((character_t *)key)->p - ((character_t *)with)->p;
}

// From Dr. Sheaffer.
int32_t corridor_path_cmp(const void *key, const void *with) {
  return ((corridor_path_t *)key)->cost - ((corridor_path_t *)with)->cost;
}

// From Dr. Sheaffer.
uint32_t adjacent_to_room(dungeon_space_t d[][DUNGEON_X], int16_t y,
                          int16_t x) {
  return (d[y][x - 1].terrain == '.' || d[y][x + 1].terrain == '.' ||
          d[y - 1][x].terrain == '.' || d[y + 1][x].terrain == '.' ||
          d[y][x - 1].terrain == '#' || d[y][x + 1].terrain == '#' ||
          d[y - 1][x].terrain == '#' || d[y + 1][x].terrain == '#');
}

// From Dr. Sheaffer.
uint32_t is_open_space(dungeon_space_t d[][DUNGEON_X], int16_t y, int16_t x) {
  return !d[y][x].hardness;
}

uint16_t generate_rooms(room_t rooms[],
                        dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X]) {
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
      for (y = rooms[x].ypos; y < rooms[x].ypos + rooms[x].ysize; y++)
        for (z = rooms[x].xpos; z < rooms[x].xpos + rooms[x].xsize; z++)
          if (y < 20 && z < 79) {
            dungeon[y][z].terrain = '.';
            dungeon[y][z].hardness = 0;
          }
      numRedos = 0;
      num_rooms++;
    }
  } // End loop for creating rooms.
  return num_rooms;
}

// From Dr. Sheaffer.
void dijkstra_corridor_inv(dungeon_space_t d[][DUNGEON_X], point_t from,
                           point_t to) {
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
      if (d[y][x].hardness != 255) {
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
        if (d[y][x].terrain != '.') {
          d[y][x].terrain = '#';
          d[y][x].hardness = 0;
        }
      }
      heap_delete(&h);
      return;
    }

#define hardnesspair_inv(p)                                                    \
  (is_open_space(d, p[dim_y], p[dim_x])                                        \
       ? 127                                                                   \
       : (adjacent_to_room(d, p[dim_y], p[dim_x]) ? 191                        \
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

void generate_corridors(room_t rooms[],
                        dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                        uint16_t num_rooms) {
  int i;
  point_t start_point, end_point;

  for (i = 0; i < num_rooms - 1; i++) {
    start_point.xpos = rand() % rooms[i].xsize + rooms[i].xpos;
    start_point.ypos = rand() % rooms[i].ysize + rooms[i].ypos;

    end_point.xpos = rand() % rooms[i + 1].xsize + rooms[i + 1].xpos;
    end_point.ypos = rand() % rooms[i + 1].ysize + rooms[i + 1].ypos;

    dijkstra_corridor_inv(dungeon, start_point, end_point);
  }
}

stair_t place_stairs(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                     char direction, uint8_t spot) {
  uint8_t x, y, z = 0;
  stair_t staircase;

  for (x = 1; x < DUNGEON_Y; x++) {
    for (y = 1; y < DUNGEON_X; y++) {

      if ((dungeon[x][y].terrain == '.' || dungeon[x][y].terrain == '#') &&
          dungeon[x][y].terrain != '>' && dungeon[x][y].terrain != '<')
        z++;

      if (z == spot) {
        dungeon[x][y].terrain = direction;
        staircase.ypos = x;
        staircase.xpos = y;
        staircase.direction = direction;
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

  return staircase;
}

point_t place_PC(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], uint8_t spot) {
  uint8_t x, y, z = 0;
  point_t pc;

  for (x = 1; x < DUNGEON_Y; x++) {
    for (y = 1; y < DUNGEON_X; y++) {

      if (dungeon[x][y].terrain == '.' || dungeon[x][y].terrain == '#')
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

void set_hardness(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X]) {
  uint8_t x, y;
  // Sets hardness for the array.
  for (x = 1; x < 20; x++) {
    for (y = 1; y < 79; y++) {
      dungeon[x][y].hardness = rand() % 254 + 1;
    }
  }
}

void add_PC(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], point_t pc) {
  dungeon[pc.ypos][pc.xpos].terrain = '@';
}

void add_dungeon_rooms(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                       room_t rooms[], uint16_t num_rooms) {
  int i, j, k;
  for (i = 0; i < num_rooms; i++) {
    for (j = rooms[i].xpos; j < rooms[i].xpos + rooms[i].xsize; j++) {
      for (k = rooms[i].ypos; k < rooms[i].ypos + rooms[i].ysize; k++) {
        if (k != 0 && k < 20 && j != 0 && j < 79)
          dungeon[k][j].terrain = '.';
      }
    }
  }
}

void add_dungeon_stairs(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                        stair_t stairs[], uint16_t num_stairs) {
  int i;
  for (i = 0; i < num_stairs; i++) {
    dungeon[stairs[i].ypos][stairs[i].xpos].terrain = stairs[i].direction;
  }
}

void add_dungeon_corridor(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X]) {
  int i, j;
  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      if (dungeon[i][j].hardness == 0 && dungeon[i][j].terrain == ' ')
        dungeon[i][j].terrain = '#';
    }
  }
}

void init_pc_map(dungeon_space_t dungeon[][DUNGEON_X], point_t pc) {
  int x, y;

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      dungeon[y][x].pc_map = ' ';
    }
  }
}

void update_pc_map(dungeon_space_t dungeon[][DUNGEON_X], point_t pc) {
  int x, y;

  for (y = pc.ypos - PC_RADIUS; y <= pc.ypos + PC_RADIUS; y++) {
    for (x = pc.xpos - PC_RADIUS; x <= pc.xpos + PC_RADIUS; x++) {
      if (x < DUNGEON_X && x >= 0 && y < DUNGEON_Y && y >= 0) {
        if (dungeon[y][x].terrain) {
          dungeon[y][x].pc_map = dungeon[y][x].terrain;
        }
      }
    }
  }
}

static int count_num_spaces(dungeon_space_t dungeon[][DUNGEON_X]) {
  int i, j, num = 0;

  for (i = 1; i < DUNGEON_Y - 1; i++) {
    for (j = 1; j < DUNGEON_X - 1; j++) {
      if (dungeon[i][j].terrain != ' ')
        num++;
    }
  }
  return num;
}

void clear_dungeon(character_t *character_map[][DUNGEON_X]) {
  int x, y;
  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      // if(character_map[y][x]) free(character_map[y][x]);
      character_map[y][x] = NULL;
    }
  }
}

void first_dungeon(dungeon_space_t dungeon[][DUNGEON_X],
                   character_t *character_map[][DUNGEON_X], heap_t *mh,
                   character_t *pc_char, int num_lives, int num_mon,
                   action_t action) {
  point_t pc;
  int num_spaces, num;

  clear_dungeon(character_map);
  heap_init(mh, monster_cmp, NULL);

  if (action == save) {
    pc = generate_dungeon(dungeon);
    num = rand() % 5 + 8;
  } else if (action == savenummon) {
    pc = generate_dungeon(dungeon);
    num_spaces = count_num_spaces(dungeon);
    num = num_mon < num_spaces ? num_mon : num_spaces;
  } else if (action == load) {
    pc = load_dungeon(dungeon);
    num = rand() % 5 + 8;
  } else if (action == loadnummon) {
    pc = load_dungeon(dungeon);
    num_spaces = count_num_spaces(dungeon);
    num = num_mon < num_spaces ? num_mon : num_spaces;
  } else if (action == loadSavenummon) {
    pc = load_save_dungeon(dungeon);
    num_spaces = count_num_spaces(dungeon);
    num = num_mon < num_spaces ? num_mon : num_spaces;
  } else {
    pc = load_save_dungeon(dungeon);
    num = rand() % 5 + 8;
  }

  pc_init(pc_char, pc, character_map, dungeon, mh, num_lives);
  generate_monsters(pc, character_map, dungeon, count_num_spaces(dungeon), num,
                    mh);

  non_tunneling_path(dungeon, pc);
  tunneling_path(dungeon, pc);

  init_pc_map(dungeon, pc);
  update_pc_map(dungeon, pc);
}

void new_dungeon(dungeon_space_t dungeon[][DUNGEON_X],
                 character_t *character_map[][DUNGEON_X], heap_t *mh,
                 character_t *pc_char, int num_lives, int num_mon,
                 action_t action) {
  int num_spaces = 0, num;

  clear_dungeon(character_map);
  heap_delete(mh);
  heap_init(mh, monster_cmp, NULL);

  point_t pc = generate_dungeon(dungeon);

  if (action == savenummon || action == loadnummon ||
      action == loadSavenummon) {
    num_spaces = count_num_spaces(dungeon);
    num = num_mon < num_spaces ? num_mon : num_spaces;
  } else {
    num = rand() % 5 + 8;
  }

  pc_init(pc_char, pc, character_map, dungeon, mh, num_lives);
  generate_monsters(pc, character_map, dungeon, num_spaces, num, mh);

  non_tunneling_path(dungeon, pc);
  tunneling_path(dungeon, pc);

  init_pc_map(dungeon, pc);
  update_pc_map(dungeon, pc);
}

int save_dungeon(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], room_t rooms[],
                 stair_t up_stairs[], stair_t down_stairs[], point_t pc,
                 uint16_t num_rooms, uint16_t num_up_stairs,
                 uint16_t num_down_stairs) {
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
      if (fwrite(&dungeon[i][j].hardness, sizeof(dungeon[i][j].hardness), 1,
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

point_t generate_dungeon(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X]) {
  room_t rooms[ROOMS_MAX];
  uint16_t num_rooms = 0;
  point_t pc;

  uint8_t x, y, upStairSpot = rand() % 200, downStairSpot = rand() % 200,
                pcSpot = rand() % 200, num_up_stairs = rand() % 2 + 1,
                num_down_stairs = rand() % 2 + 1;
  stair_t up_stairs[num_up_stairs], down_stairs[num_down_stairs];

  while (upStairSpot == downStairSpot)
    downStairSpot = rand() % 200;

  // Adds the boundaries to the dungeon array.
  for (x = 0; x < DUNGEON_Y; x++) {
    for (y = 0; y < DUNGEON_X; y++) {
      if (x == 0 || x == 20)
        dungeon[x][y].hardness = 255;
      else if (y == 0 || y == 79)
        dungeon[x][y].hardness = 255;

      dungeon[x][y].terrain = ' ';
    }
  }
  set_hardness(dungeon);

  num_rooms = generate_rooms(rooms, dungeon);
  generate_corridors(rooms, dungeon, num_rooms);
  up_stairs[0] = place_stairs(dungeon, '<', upStairSpot);
  if (num_up_stairs == 2)
    up_stairs[1] = place_stairs(dungeon, '<', upStairSpot * 2);
  down_stairs[0] = place_stairs(dungeon, '>', downStairSpot);
  if (num_down_stairs == 2)
    down_stairs[1] = place_stairs(dungeon, '>', downStairSpot * 2);
  pc = place_PC(dungeon, pcSpot);
  save_dungeon(dungeon, rooms, up_stairs, down_stairs, pc, num_rooms,
               num_up_stairs, num_down_stairs);

  return pc;
}

point_t load_dungeon(dungeon_space_t dungeon[][DUNGEON_X]) {
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
      if (!(fread(&dungeon[i][j].hardness, sizeof(pc.ypos), 1, f))) {
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
      dungeon[i][j].terrain = ' ';
    }
  }

  add_dungeon_rooms(dungeon, rooms, num_rooms);
  add_dungeon_stairs(dungeon, up_stairs, num_up_stairs);
  add_dungeon_stairs(dungeon, down_stairs, num_down_stairs);
  add_dungeon_corridor(dungeon);

  return pc;
}

point_t load_save_dungeon(dungeon_space_t dungeon[][DUNGEON_X]) {
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
      if (!(fread(&dungeon[i][j].hardness, sizeof(pc.ypos), 1, f))) {
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
      dungeon[i][j].terrain = ' ';
    }
  }

  // Fills in the dungeon.
  add_dungeon_rooms(dungeon, rooms, num_rooms);
  add_dungeon_stairs(dungeon, up_stairs, num_up_stairs);
  add_dungeon_stairs(dungeon, down_stairs, num_down_stairs);
  add_dungeon_corridor(dungeon);
  save_dungeon(dungeon, rooms, up_stairs, down_stairs, pc, num_rooms,
               num_up_stairs, num_down_stairs);

  return pc;
}
