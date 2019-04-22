#include "character_utils.hpp"
#include "dungeon.hpp"
#include "heap.h"
#include "io.hpp"
#include "item_utils.hpp"
#include "parser.hpp"

#include <cstdlib>
#include <curses.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <unistd.h>
#include <vector>

using namespace std;

int main(void) {
  srand(time(NULL));

  int fog = 0, move;
  dungeon *d;
  character *mon;

  move = 0;
  d = new dungeon(1000, save);

  io_init_terminal();

new_dung:
  heap_insert(&d->mh, d->pc);

  parse(&d->mv, &d->iv);

  generate_monsters(d, &d->mh, &d->mv);
  generate_items(d, &d->iv);

  render_dungeon(d, d->pc, fog);

  while ((mon = (character *)heap_remove_min(&d->mh))) {
    if (d->pc->hp <= 0) {
      getch();
      game_over(LOSE);
      goto over;
    }
    if (has_characteristic(mon->abilities, PC)) {
      render_dungeon(d, d->pc, fog);

      while (move == MOVE_INVALID || move >= INVALID_KEY) {
        move = move_pc(d, mon, &d->mh, &d->iv, fog);
        d->update_pc_map(mon->x, mon->y);

        if (move == MOVE_STAIR) {

          heap_reset(&d->mh);

          d->~dungeon();

          move = 0;
          d = new dungeon(d->pc->hp, save);

          goto new_dung;

        } else if (move == QUIT) {
          game_over(QUIT);
          goto over;

        } else if (move == FOG_TOGGLE) {
          fog = fog == 1 ? 0 : 1;

          render_dungeon(d, d->pc, fog);

        } else if (move == TELEPORT) {
          render_dungeon(d, d->pc, fog);
        }
      }

    } else if (mon->hp > 0) {
      move_monster(d, mon);
    }

    mon->p += 1000 / mon->get_speed();

    if (mon->hp > 0) {
      heap_insert(&d->mh, mon);

    } else {
      delete mon;
    }

    move = 0;

    non_tunneling_path(d, d->pc->x, d->pc->y);
    tunneling_path(d, d->pc->x, d->pc->y);
  }

over:

  // delete pc;
  heap_delete(&d->mh);
  d->~dungeon();
  delete d;

  return 0;
}
