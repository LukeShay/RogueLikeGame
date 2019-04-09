#include "character_utils.hpp"
#include "dungeon.hpp"
#include "heap.hpp"
#include "item_utils.hpp"
#include "parser.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

int main(void) {
  srand(time(NULL));

  character *pc = new character;
  pc->name = "PC";

  dungeon *d = new dungeon(pc, 3, 10, save);
  vector<character_desc> mv;
  vector<item_desc> iv;
  heap_t mh;
  int x, y;

  parse(&mv, &iv);

  generate_monsters(d, &mh, &mv);
  generate_items(d, &iv);

  for (y = 0; y < DUNGEON_Y; y++) {
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
  }

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (d->character_map[y][x]) {
        cout << d->character_map[y][x]->symbol;
      } else if (d->item_map[y][x]) {
        cout << d->item_map[y][x]->type[0];
      } else {
        cout << d->terrain_map[y][x];
      }
    }
    cout << endl;
  }

  delete pc;
  delete d;

  return 0;
}
