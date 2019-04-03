#include <fstream>
#include "monster_parser.h"
#include "npc.h"
#include "dice.h"
#include <iostream>

#define FILE_PATH "/.rlg327/test.txt"
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

#define SMART 0x00000001
#define TELE 0x00000002
#define TUNNEL 0x00000004
#define ERRATIC 0x00000008

void parse_monsters(const char *file_name, heap_t *mh)
{
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

  while (!valid)
  {
    np = (npc *)malloc(sizeof(np));

    getline(f, str);

    if (str.compare("") == 0)
    {
      break;
    }
    monster = str.compare(BEGIN_MONSTER);

    std::cout << monster << std::endl;
    std::cout << str << std::endl;

    while (!monster)
    {
      getline(f, str, ' ');
      std::cout << str;
      if (!str.compare(NAME))
      {
        getline(f, str);
        std::cout << str << std::endl;
        np->name = str;
      }
      else if (!str.compare(SYMBOL))
      {
        np->symbol = f.get();
        std::cout << np->symbol << std::endl;
      }
      else if (!str.compare("COLOR"))
      {
        getline(f, str);
        std::cout << str << std::endl;
        np->color = str;
      }
      else if (!str.compare(DESCRIPTION))
      {
        getline(f, str);

        while (str.compare("."))
        {
          std::cout << str << std::endl;
          np->desc += str + "";
          getline(f, str);
        }
      }
      else if (!str.compare(SPEED))
      {
        getline(f, str);
        std::cout << str << std::endl;
        np->speed.parse_dice(str);
      }
      else if (!str.compare(DAMAGE))
      {
        getline(f, str);
        std::cout << str << std::endl;
        np->ad.parse_dice(str);
      }
      else if (!str.compare(HITPOINTS))
      {
        getline(f, str);
        std::cout << str << std::endl;
        np->hp.parse_dice(str);
      }
      else if (!str.compare(RARITY))
      {
        getline(f, str);
        std::cout << str << std::endl;
        np->rarity = std::stoi(str);
      }
      else if (!str.compare(ABILITIES))
      {
        getline(f, str);
        std::cout << str << std::endl;
      }
      else if (!str.compare(END))
      {
        getline(f, str);
        std::cout << str << std::endl;
        monster = 1;
      }
      else
      {
        return;
      }
    }
    heap_insert(mh, np);
  }
}