#include "character.hpp"

character::character() {}
character::~character() {}

void character::pick_location(character *character_map[DUNGEON_Y][DUNGEON_X],
                              uint8_t hardness_map[DUNGEON_Y][DUNGEON_X]) {
  int i = 1;
  uint8_t x, y;

  while (i) {
    x = rand() % (DUNGEON_X - 3) + 1;
    y = rand() % (DUNGEON_Y - 3) + 1;

    if (!character_map[y][x] && !hardness_map[y][x]) {
      this->x = x;
      this->y = y;
      i = 0;
    }
  }

  character_map[y][x] = this;
}

int character_desc::parse_abilities() { return 0; }
