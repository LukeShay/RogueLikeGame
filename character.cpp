#include "character.hpp"

#include <cstdlib>
#include <iostream>

character::character() {
  int i;

  for (i = 0; i < 12; i++) {
    if (equiped[i]) {
      equiped[i] = NULL;
    }

    if (i < 10 && inventory[i]) {
      inventory[i] = NULL;
    }
  }
}

character::~character() {
  int i;

  for (i = 0; i < 12; i++) {
    if (equiped[i]) {
      delete equiped[i];
    }

    if (i < 10 && inventory[i]) {
      delete inventory[i];
    }
  }
}

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

int character_desc::parse_abilities() {
  int ret_ability = 0, start, end;
  std::string temp;

  for (start = end = 0; end <= (int)this->abilities.length(); end++) {
    if (end == (int)this->abilities.length() || abilities[end] == ' ') {
      temp = abilities.substr(start, end - start);
      start = end + 1;

      if (!temp.compare("SMART")) {
        ret_ability += SMART;
      } else if (!temp.compare("TELE")) {
        ret_ability += TELE;
      } else if (!temp.compare("TUNNEL")) {
        ret_ability += TUNNEL;
      } else if (!temp.compare("ERRATIC")) {
        ret_ability += ERRATIC;
      } else if (!temp.compare("PASS")) {
        ret_ability += PASS;
      } else if (!temp.compare("PICKUP")) {
        ret_ability += PICKUP;
      } else if (!temp.compare("DESTROY")) {
        ret_ability += DESTROY;
      } else if (!temp.compare("UNIQ")) {
        ret_ability += UNIQ;
      } else if (!temp.compare("BOSS")) {
        ret_ability += BOSS;
      }
    }
  }

  return ret_ability;
}

int character::get_speed() {
  int i, ret_speed = speed;

  for (i = 0; i < 12; i++) {
    if (equiped[i] && equiped[i]->speed_bonus > 0) {
      ret_speed += equiped[i]->speed_bonus;
    }
  }

  return ret_speed < speed ? speed : ret_speed;
}

int character::get_damage() {
  int i, ret_damage = ad.roll_dice();

  for (i = 0; i < 12; i++) {
    if (equiped[i]) {
      ret_damage += equiped[i]->damage_bonus.roll_dice();
    }
  }

  return ret_damage < 0 ? ad.roll_dice() : ret_damage;
}

void character::take_damage(int damage) {
  int i, defense = 0;

  for (i = 0; i < 12; i++) {
    if (equiped[i]) {
      defense += equiped[i]->defense_bonus;
    }
  }

  if (damage > defense) {
    hp -= damage - defense;
  }
}
