#include "character_utils.hpp"
#include "dungeon.hpp"
#include "heap.hpp"
#include "item_utils.hpp"
#include "parser.hpp"

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

typedef enum item_type {
  not_valid,
  weapon,
  offhand,
  ranged,
  armor,
  helmet,
  cloack,
  gloves,
  boots,
  ring,
  amulet,
  light,
  scroll,
  book,
  flask,
  gold,
  ammunition,
  food,
  wand,
  container,
  stack
} item_type_t;

int32_t monster_cmp(const void *key, const void *with) {
  return ((character *)key)->p - ((character *)with)->p;
}

const std::string item_symbol = "*|)}[]({\\=\"_~?!$/,-\%&";

int main(void) {
  srand(time(NULL));

  character *pc = new character;
  pc->name = "PC";
  pc->abilities = PC;
  pc->symbol = '@';
  pc->speed = 10;

  dungeon *d = new dungeon(pc, 3, 10, save);
  vector<character_desc> mv;
  vector<item_desc> iv;
  heap_t mh;
  int x, y;

  heap_init(&mh, monster_cmp, NULL);

  heap_insert(&mh, pc);

  parse(&mv, &iv);

  generate_monsters(d, &mh, &mv);
  generate_items(d, &iv);

  /*  for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (d->character_map[y][x]) {
          cout << x << ", " << y << " " << d->character_map[y][x]->name << endl;
        }
      }
    }

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (d->item_map[y][x]) {
          cout << x << ", " << y << " " << d->item_map[y][x]->name << endl;
        }
      }
    }*/

  character *mon;

  while ((mon = (character *)heap_remove_min(&mh))) {
    if (mon != pc) {
      move_monster(d, mon, pc);
    } else if (!mon->hp) {
      break;
    }

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (d->character_map[y][x]) {
          cout << d->character_map[y][x]->symbol;
        } else if (d->item_map[y][x]) {
          cout << item_symbol[stack];
        } else {
          cout << d->terrain_map[y][x];
        }
      }
      cout << endl;
    }

    if (mon->hp != 0) {
      mon->p += 1000 / mon->speed;
      heap_insert(&mh, mon);
    } else {
      delete mon;
    }

    usleep(1000000);
  }

  // delete pc;
  delete d;

  return 0;
}
