#include "dice.hpp"
#include "parser.hpp"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

#define MONSTER_FILE_PATH "/.rlg327/monster_desc.txt"
#define MONSTER_FILE_SEMANTIC "RLG327 MONSTER DESCRIPTION 1"
#define ITEM_FILE_PATH "/.rlg327/object_desc.txt"
#define ITEM_FILE_SEMANTIC "RLG327 OBJECT DESCRIPTION 1"
#define BEGIN_MONSTER "BEGIN MONSTER"
#define BEGIN_ITEM "BEGIN OBJECT"
#define NAME "NAME"
#define DESCRIPTION "DESC"
#define COLOR "COLOR"
#define SPEED "SPEED"
#define ABILITIES "ABIL"
#define HITPOINTS "HP"
#define DAMAGE "DAM"
#define SYMBOL "SYMB"
#define RARITY "RRTY"
#define END "END"
#define TYPE "TYPE"
#define HIT_BONUS "HIT"
#define DAMAGE_BONUS "DAM"
#define DODGE_BONUS "DODGE"
#define DEFENSE_BONUS "DEF"
#define WEIGHT "WEIGHT"
#define VALUE "VAL"
#define SPEED_BONUS "SPEED"
#define SPECIAL_ATTR "ATTR"
#define ARTIFACT_STATUS "ART"

void parse_monsters(std::vector<character_desc> *mv) {
  int valid = 1, monster = 1;
  char *directory = getenv("HOME");
  char *path =
      (char *)malloc(strlen(directory) + strlen(MONSTER_FILE_PATH) + 1);
  character_desc npc;

  strcpy(path, directory);
  strcat(path, MONSTER_FILE_PATH);

  std::ifstream f(path);

  free(path);

  std::string str;
  getline(f, str);

  valid = str.compare(MONSTER_FILE_SEMANTIC);

  while (!valid && getline(f, str)) {

    monster = str.compare(BEGIN_MONSTER);

    while (!monster) {
      f >> str;
      f.get();
      if (!str.compare(NAME)) {

        getline(f, str);

        npc.name = str;
      } else if (!str.compare(SYMBOL)) {
        getline(f, str);
        npc.symbol = str[0];

      } else if (!str.compare(COLOR)) {

        getline(f, str);

        npc.color = str;
      } else if (!str.compare(DESCRIPTION)) {

        getline(f, str);

        while (str.compare(".")) {

          npc.desc += "\n" + str;
          getline(f, str);
        }
      } else if (!str.compare(SPEED)) {

        getline(f, str);

        npc.speed.parse_dice(str);
      } else if (!str.compare(DAMAGE)) {

        getline(f, str);

        npc.ad.parse_dice(str);
      } else if (!str.compare(HITPOINTS)) {

        getline(f, str);

        npc.hp.parse_dice(str);
      } else if (!str.compare(RARITY)) {

        getline(f, str);

        npc.rarity = std::stoi(str);
      } else if (!str.compare(ABILITIES)) {

        getline(f, str);
        npc.abilities = str;
      } else if (!str.compare(END)) {

        getline(f, str);

        monster = 1;
        npc.placed = 0;
        mv->push_back(npc);
      }
    }
  }
  mv->shrink_to_fit();
  f.close();
}

void parse_items(std::vector<item_desc> *iv) {
  int valid = 1, object = 1;
  char *directory = getenv("HOME");
  char *path = (char *)malloc(strlen(directory) + strlen(ITEM_FILE_PATH) + 1);
  item_desc obj;

  strcpy(path, directory);
  strcat(path, ITEM_FILE_PATH);

  std::ifstream f(path);

  free(path);

  std::string str;
  getline(f, str);

  valid = str.compare(ITEM_FILE_SEMANTIC);

  while (!valid && getline(f, str)) {

    object = str.compare(BEGIN_ITEM);

    while (!object) {
      f >> str;
      f.get();
      if (!str.compare(NAME)) {

        getline(f, str);

        obj.name = str;
      } else if (!str.compare(TYPE)) {

        getline(f, str);

        obj.type = str;
      } else if (!str.compare(COLOR)) {

        getline(f, str);

        obj.color = str;
      } else if (!str.compare(DESCRIPTION)) {

        getline(f, str);

        while (str.compare(".")) {

          obj.desc += "\n" + str;
          getline(f, str);
        }
      } else if (!str.compare(WEIGHT)) {

        getline(f, str);
        obj.weight.parse_dice(str);

      } else if (!str.compare(HIT_BONUS)) {

        getline(f, str);
        obj.hit_bonus.parse_dice(str);

      } else if (!str.compare(DAMAGE_BONUS)) {

        getline(f, str);
        obj.damage_bonus.parse_dice(str);

      } else if (!str.compare(RARITY)) {

        getline(f, str);

        obj.rarity = std::stoi(str);

      } else if (!str.compare(SPECIAL_ATTR)) {

        getline(f, str);

        obj.special_attr.parse_dice(str);

      } else if (!str.compare(VALUE)) {

        getline(f, str);

        obj.value.parse_dice(str);

      } else if (!str.compare(DODGE_BONUS)) {

        getline(f, str);
        obj.dodge_bonus.parse_dice(str);

      } else if (!str.compare(WEIGHT)) {

        getline(f, str);
        obj.weight.parse_dice(str);

      } else if (!str.compare(DEFENSE_BONUS)) {

        getline(f, str);
        obj.defense_bonus.parse_dice(str);

      } else if (!str.compare(SPEED_BONUS)) {

        getline(f, str);
        obj.speed_bonus.parse_dice(str);

      } else if (!str.compare(ARTIFACT_STATUS)) {

        getline(f, str);

        obj.artifact_status = str;

      } else if (!str.compare(END)) {

        getline(f, str);
        object = 1;

        iv->push_back(obj);
      }
    }
  }
  iv->shrink_to_fit();
  f.close();
}

void parse(std::vector<character_desc> *mv, std::vector<item_desc> *iv) {
  parse_monsters(mv);
  parse_items(iv);
}
