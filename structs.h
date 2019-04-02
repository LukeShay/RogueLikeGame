#ifndef STRUCTS_H
#define STRUCTS_H

#include "heap.h"
#include "priority_queue.h"
#include <cstring>

#define DUNGEON_X 80
#define DUNGEON_Y 21

#define C_PC 0x00000010
#define PC_RADIUS 4

typedef enum action
{
  save,
  load,
  loadSave,
  savenummon,
  loadnummon,
  loadSavenummon
} action_t;

class dungeon_space_t
{
public:
  char terrain;
  uint8_t hardness;
  char pc_map;
  int cost_t;
  int cost_nt;
};

class point_t
{
public:
  uint8_t xpos, ypos;
};

class character_t
{
public:
  int characteristic;
  int speed;
  int direction; // 0 up, 1 right, 2 down, 3 left.
  int lives;
  point_t location;
  int p;
};

#define has_characteristic(character, bit) (character & C_##bit)

#endif
