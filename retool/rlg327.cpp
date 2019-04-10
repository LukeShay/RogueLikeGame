#include "character_utils.hpp"
#include "dungeon.hpp"
#include "heap.hpp"
#include "io.hpp"
#include "item_utils.hpp"
#include "parser.hpp"

#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <vector>

#define MOVE_INVALID 0
#define MOVE_VALID 1
#define MOVE_STAIR 2
#define QUIT 3
#define WIN 4
#define LOSE 5
#define INVALID_KEY 6
#define LIST_MONSTERS 7
#define TUNNELING_MAP 8
#define NON_TUNNELING_MAP 9
#define DEFAULT_MAP 10
#define FOG_TOGGLE 11
#define TELEPORT 12

using namespace std;

int32_t monster_cmp(const void *key, const void *with) {
  return ((character *)key)->p - ((character *)with)->p;
}

int main(void) {
  srand(time(NULL));

  character *pc;
  dungeon *d;
  vector<character_desc> mv;
  vector<item_desc> iv;
  heap_t mh;
  int fog = 0, move;
  character *mon;

  pc = new character;
  d = new dungeon(pc, 10, 10, save);
  heap_init(&mh, monster_cmp, NULL);

  heap_insert(&mh, pc);

  pc->abilities = PC;

  parse(&mv, &iv);

  generate_monsters(d, &mh, &mv);
  generate_items(d, &iv);

  io_init_terminal();
  render_dungeon_first(d, pc, &mh, fog);

  while ((mon = (character *)heap_remove_min(&mh))) {
    if (has_characteristic(mon->abilities, PC)) {
      if (mh.size == 0) {
        game_over(WIN);
        break;
      }

      while (move == MOVE_INVALID || move >= INVALID_KEY) {
        move = move_pc(d, mon, &mh, fog);
        d->update_pc_map(mon->x, mon->y);

        if (move == MOVE_INVALID) {
          invalid_move();

        } else if (move == INVALID_KEY) {
          invalid_key();

        } else if (move == MOVE_STAIR) {
          delete d;
          d = new dungeon(pc, pc->hp, 10, save);

          heap_reset(&mh);
          heap_insert(&mh, pc);

          mv.clear();
          iv.clear();
          parse(&mv, &iv);

          generate_monsters(d, &mh, &mv);
          generate_items(d, &iv);

          render_dungeon_first(d, pc, &mh, fog);
          goto new_dung;

        } else if (move == QUIT) {
          game_over(QUIT);
          goto over;

        } else if (move == FOG_TOGGLE) {
          fog = fog == 1 ? 0 : 1;

          render_dungeon(d, pc, &mh, fog);

        } else {
          render_dungeon(d, pc, &mh, fog);
        }
      }

    } else if (mon->hp != 0) {
      move_monster(d, mon, pc);
    }

    if (pc->hp == 0) {
      game_over(LOSE);
      goto over;
    }

    mon->p += (1000 / mon->speed);

    if (mon->hp > 0) {
      heap_insert(&mh, mon);

    } else {
      delete mon;
    }

  new_dung:
    move = 0;

    non_tunneling_path(d, pc->x, pc->y);
    tunneling_path(d, pc->x, pc->y);
  }

over:

  // delete pc;
  delete d;

  return 0;
}
