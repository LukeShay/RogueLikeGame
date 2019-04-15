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
#include <unistd.h>
#include <vector>

using namespace std;

int32_t monster_cmp(const void *key, const void *with) {
  return ((character *)key)->p - ((character *)with)->p;
}

int main(void) {
  srand(time(NULL));

  vector<character_desc> mv;
  vector<item_desc> iv;
  int fog = 0, move;
  character *pc;
  dungeon *d;
  heap_t mh;
  character *mon;

  move = 0;
  pc = new character;
  d = new dungeon(pc, 1000, 10, save);

  parse(&mv, &iv);

  heap_init(&mh, monster_cmp, NULL);

  io_init_terminal();

new_dung:
  heap_insert(&mh, pc);

  generate_monsters(d, &mh, &mv);
  generate_items(d, &iv);

  render_dungeon_first(d, pc, &mh, fog);

  while ((mon = (character *)heap_remove_min(&mh))) {
    if (pc->hp <= 0) {
      getch();
      game_over(LOSE);
      goto over;
    }
    if (has_characteristic(mon->abilities, PC)) {
      render_dungeon(d, pc, &mh, fog);

      if (mh.size == 0) {
        game_over(WIN);
        goto over;
      } else if (pc->hp <= 0) {
        getch();
        game_over(LOSE);
        goto over;
      }

      while (move == MOVE_INVALID || move >= INVALID_KEY) {
        move = move_pc(d, mon, &mh, &iv, fog);
        d->update_pc_map(mon->x, mon->y);

        if (move == MOVE_INVALID) {
          invalid_move();

        } else if (move == INVALID_KEY) {
          invalid_key();

        } else if (move == MOVE_STAIR) {

          heap_reset(&mh);

          d->~dungeon();

          move = 0;
          d = new dungeon(pc, pc->hp, 10, save);

          goto new_dung;

        } else if (move == QUIT) {
          game_over(QUIT);
          goto over;

        } else if (move == FOG_TOGGLE) {
          fog = fog == 1 ? 0 : 1;

          render_dungeon(d, pc, &mh, fog);

        } else if (move == TELEPORT) {
          render_dungeon(d, pc, &mh, fog);
        }
      }

    } else if (mon->hp > 0) {
      move_monster(d, mon, pc);
    }

    mon->p += (1000 / mon->speed);

    if (mon->hp > 0) {
      heap_insert(&mh, mon);

    } else {
      delete mon;
    }

    move = 0;

    non_tunneling_path(d, pc->x, pc->y);
    tunneling_path(d, pc->x, pc->y);
  }

over:

  // delete pc;
  delete d;

  return 0;
}
