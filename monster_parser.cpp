#include "dice.h"
#include "monster_parser.h"
#include "npc.h"
#include <fstream>
#include <iostream>

#define FILE_PATH "/.rlg327/monster_desc.txt"
#define FILE_SEMANTIC "RLG327 MONSTER DESCRIPTION 1"
#define BEGIN_MONSTER "BEGIN MONSTER"
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

void parse_monsters(heap_t *mh) {
  int valid = 1, monster = 1;
  char *directory = getenv("HOME");
  char *path = (char *)malloc(strlen(directory) + strlen(FILE_PATH) + 1);
  npc *np;

  strcpy(path, directory);
  strcat(path, FILE_PATH);

  std::ifstream f(path);

  free(path);

  std::string str;
  getline(f, str);

  valid = str.compare(FILE_SEMANTIC);

  while (!valid && getline(f, str)) {
    np = new npc;

    monster = str.compare(BEGIN_MONSTER);

    while (!monster) {
      f >> str;
      if (!str.compare(NAME)) {
        f.get();
        getline(f, str);
        std::cout << str << std::endl;
        np->name = str;
      } else if (!str.compare(SYMBOL)) {
        getline(f, str);
        np->symbol = str[1];
        std::cout << np->symbol << std::endl;
      } else if (!str.compare(COLOR)) {
        f.get();
        getline(f, str);
        std::cout << str << std::endl;
        np->color = str;
      } else if (!str.compare(DESCRIPTION)) {
        f.get();
        getline(f, str);

        while (str.compare(".")) {
          std::cout << str << std::endl;
          np->desc += str + "";
          getline(f, str);
        }
      } else if (!str.compare(SPEED)) {
        f.get();
        getline(f, str);
        std::cout << str << std::endl;
        np->speed.parse_dice(str);
      } else if (!str.compare(DAMAGE)) {
        f.get();
        getline(f, str);
        std::cout << str << std::endl;
        np->ad.parse_dice(str);
      } else if (!str.compare(HITPOINTS)) {
        f.get();
        getline(f, str);
        std::cout << str << std::endl;
        np->hp.parse_dice(str);
      } else if (!str.compare(RARITY)) {
        f.get();
        getline(f, str);
        std::cout << str << std::endl;
        np->rarity = std::stoi(str);
      } else if (!str.compare(ABILITIES)) {
        f.get();
        getline(f, str);
        std::cout << str << std::endl;
      } else if (!str.compare(END)) {
        f.get();
        getline(f, str);
        std::cout << str << std::endl;
        monster = 1;
      } else {
        return;
      }
    }
    heap_insert(mh, np);
  }
  heap_delete(mh);
}
