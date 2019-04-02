#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>

#include "characters.h"
#include "dungeon.h"
#include "user_interface.h"

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

char *print_characters(const void *v) {
  static char out[2000];
  std::string arr = "0123456789abcdef@";
  character_t mon = *((character_t *)v);

  snprintf(out, 2000,
           "Char: %c  Speed: %2d  xpos: %2d  ypos: %2d  p: %d  lives: %d",
           arr[mon.characteristic], mon.speed, mon.location.xpos,
           mon.location.ypos, mon.p, mon.lives);

  return out;
}

int main(int argc, char *argv[]) {
  dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X];
  character_t *character_map[DUNGEON_Y][DUNGEON_X],
      *pc = (character_t *)malloc(sizeof(*pc)), *temp;
  heap_t mh;
  int move = 0, num_mon = 0, fog = 1;
  action_t action;

  if (argc > 5) {
    fprintf(stderr,
            "No flags are necessary but the following can be used:\n"
            "Usage: %s <--save|--load|--save --load|--load --save> \n  Can be "
            "added as an option: <--nummon 'int'>\n",
            argv[0]);
    return -1;
  }

  if (argc == 1) {
    action = save;

  } else if (!strcmp(argv[1], "--save")) {
    action = save;

    if (argc == 3 && !strcmp(argv[2], "--load")) {
      action = loadSave;

    } else if (argc == 4 && !strcmp(argv[2], "--nummon")) {
      num_mon = atoi(argv[3]);
      action = savenummon;

    } else if (argc == 5 && !strcmp(argv[2], "--load") &&
               !strcmp(argv[3], "--nummon")) {
      num_mon = atoi(argv[4]);
      action = loadSavenummon;

    } else if (argc != 2) {
      fprintf(stderr, "Invalid command line parameter: %s %s\n", argv[1],
              argv[2]);
      return -1;
    }
  } else if (!strcmp(argv[1], "--load")) {
    action = load;

    if (argc == 3 && !strcmp(argv[2], "--save")) {
      action = loadSave;

    } else if (argc == 4 && !strcmp(argv[2], "--nummon")) {
      num_mon = atoi(argv[3]);
      action = loadnummon;

    } else if (argc == 5 && !strcmp(argv[2], "--save") &&
               !strcmp(argv[3], "--nummon")) {
      num_mon = atoi(argv[4]);
      action = loadSavenummon;

    } else if (argc != 2) {
      fprintf(stderr, "Invalid command line parameter: %s\n", argv[1]);
      return -1;
    }

  } else if(!strcmp(argv[1], "--nummon")){
    if(argc == 3){
      action = savenummon;
      num_mon = atoi(argv[2]);
    }
    else {
      fprintf(stderr, "Invalid command line parameter: %s", argv[1]);
      return -1;
    }

  } else {
    action = save;
  }

  srand(time(NULL));

  first_dungeon(dungeon, character_map, &mh, pc, 3, num_mon, action);

  io_init_terminal();
  render_dungeon_first(dungeon, character_map, pc, &mh, fog);

  while ((temp = (character_t *)heap_remove_min(&mh))) {
    if (has_characteristic(temp->characteristic, PC)) {
      if (mh.size == 0) {
        game_over(WIN);
        break;
      }

      while (move == MOVE_INVALID || move >= INVALID_KEY) {
        move = move_pc(dungeon, character_map, temp, &mh, fog);
        update_pc_map(dungeon, temp->location);

        if (move == MOVE_INVALID) {
          invalid_move();

        } else if (move == INVALID_KEY) {
          invalid_key();

        } else if (move == MOVE_STAIR) {
          int lives = pc->lives;

          new_dungeon(dungeon, character_map, &mh, pc, lives, num_mon, action);
          update_pc_map(dungeon, temp->location);
          render_dungeon_first(dungeon, character_map, pc, &mh, fog);
          goto new_dung;

        } else if (move == QUIT) {
          game_over(QUIT);
          goto over;

        } else if (move == FOG_TOGGLE){
          fog = fog == 1 ? 0 : 1;

          render_dungeon(dungeon, character_map, pc, &mh, fog);

        } else {
          render_dungeon(dungeon, character_map, pc, &mh, fog);
        }
      }

    } else if (temp->lives != 0) {
      move_character(dungeon, character_map, temp, pc);
    }

    if (pc->lives == 0) {
      game_over(LOSE);
      goto over;
    }

    temp->p += (1000 / temp->speed);

    if (temp->lives > 0) {
      heap_insert(&mh, temp);

    } else {
      free(temp);
    }

    new_dung: move = 0;

    non_tunneling_path(dungeon, pc->location);
    tunneling_path(dungeon, pc->location);
  }

  over:

  free(pc);
  heap_delete(&mh);

  return 0;
}
