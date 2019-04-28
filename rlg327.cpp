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

#define START_DROWNING 5
#define SAND_TELEPORT 5

using namespace std;

int main(void) {
  srand(time(NULL));

  int fog = 0, move, water_counter = 0, sand_counter = 0;
  dungeon *d;
  character *mon;

  move = 0;
  d = new dungeon(100000, save);

  io_init_terminal();

new_dung:
  heap_insert(&d->mh, d->pc);

  parse(&d->mv, &d->iv);

  generate_monsters(d, &d->mh, &d->mv);
  generate_items(d, &d->iv);

  render_dungeon(d, d->pc, fog,
                 d->moves_trapped > 0 ? "You are trapped!"
                                      : "Make your move summoner.");

  while ((mon = (character *)heap_remove_min(&d->mh))) {
    assert(mon);
    assert(d);

    if (d->pc->hp <= 0) {
      getch();
      game_over(LOSE);
      goto over;
    }

    if (has_characteristic(mon->abilities, PC)) {
      if (d->terrain_map[d->pc->y][d->pc->x] == '~') {
        water_counter++;

        if (water_counter >= START_DROWNING) {
          d->pc->hp -= 50;
        }
      } else {
        water_counter = 0;
      }

      if (d->terrain_map[d->pc->y][d->pc->x] == '*') {
        sand_counter++;

        if (sand_counter == SAND_TELEPORT) {
          goto fall_through_quicksand;
        }
      } else {
        sand_counter = 0;
      }

      d->moves_trapped = d->moves_trapped >= 2 ? 0 : d->moves_trapped;

      render_dungeon(d, d->pc, fog,
                     d->moves_trapped > 0 ? "You are trapped!"
                                          : "Make your move summoner.");

      while (move == MOVE_INVALID || move >= INVALID_KEY) {
        move = move_pc(d, mon, &d->mh, &d->iv, fog);
        d->update_pc_map(mon->x, mon->y);

        if (move == MOVE_STAIR) {
        fall_through_quicksand:;
          sand_counter = water_counter = 0;

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

          render_dungeon(d, d->pc, fog,
                         d->moves_trapped > 0 ? "You are trapped!"
                                              : "Make your move summoner.");

        } else if (move == TELEPORT) {
          render_dungeon(d, d->pc, fog,
                         d->moves_trapped > 0 ? "You are trapped!"
                                              : "Make your move summoner.");

        } else if (move == GAME_OVER) {
          goto over;
        } else if (move == MOVE_TRAPPED) {
          render_dungeon(d, d->pc, fog,
                         d->moves_trapped > 0 ? "You are trapped!"
                                              : "Make your move summoner.");
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

  delete d;
  endwin();
  exit(0);

  assert(!d);
  return 0;
}
