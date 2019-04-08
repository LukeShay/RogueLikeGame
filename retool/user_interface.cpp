#include <climits>
#include <cstdlib>
#include <curses.h>
#include <string>

#include "user_interface.hpp"

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

#define DISPLAY_MAX_Y 23
#define DISPLAY_MAX_X 79

#define RED 1
#define CYAN 2

const char *victory =
    "\n                                       o\n"
    "                                      $\"\"$o\n"
    "                                     $\"  $$\n"
    "                                      $$$$\n"
    "                                      o \"$o\n"
    "                                     o\"  \"$\n"
    "                oo\"$$$\"  oo$\"$ooo   o$    \"$    ooo\"$oo  $$$\"o\n"
    "   o o o o    oo\"  o\"      \"o    $$o$\"     o o$\"\"  o$      \"$  "
    "\"oo   o o o o\n"
    "   \"$o   \"\"$$$\"   $$         $      \"   o   \"\"    o\"         $"
    "   \"o$$\"    o$$\n"
    "     \"\"o       o  $          $\"       $$$$$       o          $  ooo"
    "     o\"\"\n"
    "        \"o   $$$$o $o       o$        $$$$$\"       $o        \" $$$$"
    "   o\"\n"
    "         \"\"o $$$$o  oo o  o$\"         $$$$$\"        \"o o o o\"  "
    "\"$$$  $\n"
    "           \"\" \"$\"     \"\"\"\"\"            \"\"$\"            \""
    "\"\"      \"\"\" \"\n"
    "            \"oooooooooooooooooooooooooooooooooooooooooooooooooooooo$\n"
    "             \"$$$$\"$$$$\" $$$$$$$\"$$$$$$ \" \"$$$$$\"$$$$$$\"  $$$\""
    "\"$$$$\n"
    "              $$$oo$$$$   $$$$$$o$$$$$$o\" $$$$$$$$$$$$$$ o$$$$o$$$\"\n"
    "              $\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""
    "\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"$\n"
    "              $\"                                                 \"$\n"
    "              $\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\"$\""
    "$\"$\"$\"$\"$\"$\"$\"$\n"
    "                                   You win!\n\n";

const char *gg =
    " 		         _____  _____\n"
    "		    	        <     `/     |\n"
    "  				 >          (\n"
    "				|   _     _  |\n"
    "				|  |_) | |_) |\n"
    "				|  | \\ | |   |\n"
    "				|            |\n"
    "                 _______._______|            |___________  ____\n"
    "               _/                                        \\|    |\n"
    "              |                    A. Luser                    <\n"
    "              |_____.-._________              ____/|___________|\n"
    "				|            |\n"
    "				|            |\n"
    "				|            |\n"
    "				|            |\n"
    "				|   _        <\n"
    "				|__/         |\n"
    "				 / `--.      |\n"
    "			       %%|            |%%\n"
    "		           |/.%%%%|          -< @%%%%%%\n"
    "		          `%%%%\%%`@|     v      |@@%%@%%\n"
    "		         .%%%%%%@@@|%%    |     %%%%@@@%%%%@%%%%%%%%\n"
    "	            "
    "_.%%%%%%%%%%%%@@@@@@%%%%_/%%\\%%_%%@@%%%%@@@@@@@%%%%%%%%%%\n";

const char *quitter = "\"Champions don't quit.\" - Mike Tython\n\n\n           "
                      "           (Formally known as \'Mike Tyson\')";

int valid_move(dungeon_space_t dungeon[][DUNGEON_X],
               character_t *character_map[][DUNGEON_X], uint8_t x, uint8_t y,
               character_t *pc) {
  if (dungeon[y][x].hardness != 0)
    return MOVE_INVALID;

  if (character_map[y][x]) {
    character_map[y][x]->lives = 0;
  }

  character_map[pc->location.ypos][pc->location.xpos] = NULL;
  pc->location.ypos = y;
  pc->location.xpos = x;

  character_map[y][x] = pc;

  return MOVE_VALID;
}

int valid_stair(dungeon_space_t dungeon[][DUNGEON_X], character_t *pc, char s) {
  if (s == dungeon[pc->location.ypos][pc->location.xpos].terrain)
    return MOVE_STAIR;
  else
    return MOVE_INVALID;
}

int list_monsters(character_t *character_map[][DUNGEON_X], character_t *pc) {
  std::string arr = "0123456789abcdef@";
  int i, j, k, num_initial = 0, num, print_start = 26, c = INT_MAX,
               num_on_screen, max_lines = 17;

  do {
    if (c == KEY_DOWN && num_on_screen >= max_lines) {
      num_initial++;
    } else if (c == KEY_UP) {
      num_initial > 0 ? num_initial-- : num_initial;
    }

    k = 3;
    num = num_on_screen = 0;

    clear();

    mvprintw(0, print_start + 2, "****************");
    mvprintw(1, print_start + 2, "**  Monsters  **");
    mvprintw(2, print_start + 2, "****************");

    attron(COLOR_PAIR(1));
    mvprintw(DISPLAY_MAX_Y - 1, 0, "Use arrow keys to scroll");
    mvprintw(DISPLAY_MAX_Y, 0, "Use ESC key to exit");
    attroff(COLOR_PAIR(1));

    for (i = 0; i < DUNGEON_Y; i++) {
      for (j = 0; j < DUNGEON_X; j++) {
        if (character_map[i][j] &&
            !has_characteristic(character_map[i][j]->characteristic, PC)) {
          if (num >= num_initial) {
            num_on_screen++;
            k++;

            mvprintw(k, print_start,
                     "%c:", arr[character_map[i][j]->characteristic]);

            if (i - pc->location.ypos < 0) {
              mvprintw(k, print_start + 2, "%3d north, ",
                       pc->location.ypos - i);
            } else {
              mvprintw(k, print_start + 2, "%3d south, ",
                       i - pc->location.ypos);
            }

            if (j - pc->location.xpos < 0) {
              mvprintw(k, print_start + 13, "%2d west", pc->location.xpos - j);
            } else {
              mvprintw(k, print_start + 13, "%2d east", j - pc->location.xpos);
            }
          } else {
            num++;
          }
        }

        if (num_on_screen == max_lines)
          goto max_on_screen;
      }
    }

  max_on_screen:
    refresh();
  } while (((c = getch()) != 27));

  return 0;
}

void render_dungeon_teleport(dungeon_space_t dungeon[][DUNGEON_X],
                             character_t *character_map[][DUNGEON_X],
                             character_t *pc, heap_t *mh, int fog) {
  int x, y, i;
  std::string symbol = "0123456789abcdef@";

  if (mh->size == 0)
    game_over(WIN);
  else {

    clear();

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (x == pc->location.xpos && y == pc->location.ypos) {
          i = CYAN;

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, symbol[16]);
          attroff(COLOR_PAIR(i));
        } else {
          if (character_map[y][x] && character_map[y][x] != pc) {
            i = RED;

            attron(COLOR_PAIR(i));
            mvaddch(y + 1, x, symbol[character_map[y][x]->characteristic]);
            attroff(COLOR_PAIR(i));
          } else {
            mvaddch(y + 1, x, dungeon[y][x].terrain);
          }
        }
      }
    }

    mvprintw(DISPLAY_MAX_Y - 1, DISPLAY_MAX_X - 9, "Lives: %2d", pc->lives);
    mvprintw(DISPLAY_MAX_Y, DISPLAY_MAX_X - 12, "Monsters: %2d", mh->size);

    refresh();
  }
}

void teleport_pc(dungeon_space_t dungeon[][DUNGEON_X],
                 character_t *character_map[][DUNGEON_X], character_t *pc,
                 heap_t *mh, int fog) {
  int c;
  int new_x, new_y;

  character_map[pc->location.ypos][pc->location.xpos] = NULL;

  render_dungeon_teleport(dungeon, character_map, pc, mh, 0);
  attron(COLOR_PAIR(RED));
  mvprintw(0, 0, "Teleporting                  ");
  attroff(COLOR_PAIR(RED));

  while ((c = getch()) != 't') {
    switch (c) {
    case 'y':
    case '7':
    case KEY_HOME:
      new_x = pc->location.xpos - 1;
      new_y = pc->location.ypos - 1;
      goto move;

    case 'k':
    case '8':
    case KEY_UP:
      new_x = pc->location.xpos;
      new_y = pc->location.ypos - 1;
      goto move;

    case 'u':
    case '9':
    case KEY_PPAGE:
      new_x = pc->location.xpos + 1;
      new_y = pc->location.ypos - 1;
      goto move;

    case 'l':
    case '6':
    case KEY_RIGHT:
      new_x = pc->location.xpos + 1;
      new_y = pc->location.ypos;
      goto move;

    case 'n':
    case '3':
    case KEY_NPAGE:
      new_x = pc->location.xpos + 1;
      new_y = pc->location.ypos + 1;
      goto move;

    case 'j':
    case '2':
    case KEY_DOWN:
      new_x = pc->location.xpos;
      new_y = pc->location.ypos + 1;
      goto move;

    case 'b':
    case '1':
    case KEY_END:
      new_x = pc->location.xpos - 1;
      new_y = pc->location.ypos + 1;
      goto move;

    case 'h':
    case '4':
    case KEY_LEFT:
      new_x = pc->location.xpos - 1;
      new_y = pc->location.ypos;
      goto move;

    case 'r':
      new_x = rand() % 77 + 1;
      new_y = rand() % 18 + 1;
      goto move;
    }
  move:
    if (new_y < 20 && new_y > 0 && new_x < 79 && new_x > 0) {

      pc->location.ypos = new_y;
      pc->location.xpos = new_x;

      if (c == 'r')
        goto done;
    }
    render_dungeon_teleport(dungeon, character_map, pc, mh, 0);
    attron(COLOR_PAIR(RED));
    mvprintw(0, 0, "Teleporting                  ");
    attroff(COLOR_PAIR(RED));
  }

done:

  if (character_map[pc->location.ypos][pc->location.xpos]) {
    character_map[pc->location.ypos][pc->location.xpos]->lives = 0;
  }

  character_map[pc->location.ypos][pc->location.xpos] = pc;

  render_dungeon(dungeon, character_map, pc, mh, fog);
}

int move_pc(dungeon_space_t dungeon[][DUNGEON_X],
            character_t *character_map[][DUNGEON_X], character_t *pc,
            heap_t *mh, int fog) {
  int c = getch();
  uint8_t new_x, new_y;

  switch (c) {
  case 'y':
  case '7':
  case KEY_HOME:
    new_x = pc->location.xpos - 1;
    new_y = pc->location.ypos - 1;
    return valid_move(dungeon, character_map, new_x, new_y, pc);

  case 'k':
  case '8':
  case KEY_UP:
    new_x = pc->location.xpos;
    new_y = pc->location.ypos - 1;
    return valid_move(dungeon, character_map, new_x, new_y, pc);

  case 'u':
  case '9':
  case KEY_PPAGE:
    new_x = pc->location.xpos + 1;
    new_y = pc->location.ypos - 1;
    return valid_move(dungeon, character_map, new_x, new_y, pc);

  case 'l':
  case '6':
  case KEY_RIGHT:
    new_x = pc->location.xpos + 1;
    new_y = pc->location.ypos;
    return valid_move(dungeon, character_map, new_x, new_y, pc);

  case 'n':
  case '3':
  case KEY_NPAGE:
    new_x = pc->location.xpos + 1;
    new_y = pc->location.ypos + 1;
    return valid_move(dungeon, character_map, new_x, new_y, pc);

  case 'j':
  case '2':
  case KEY_DOWN:
    new_x = pc->location.xpos;
    new_y = pc->location.ypos + 1;
    return valid_move(dungeon, character_map, new_x, new_y, pc);

  case 'b':
  case '1':
  case KEY_END:
    new_x = pc->location.xpos - 1;
    new_y = pc->location.ypos + 1;
    return valid_move(dungeon, character_map, new_x, new_y, pc);

  case 'h':
  case '4':
  case KEY_LEFT:
    new_x = pc->location.xpos - 1;
    new_y = pc->location.ypos;
    return valid_move(dungeon, character_map, new_x, new_y, pc);

  case '5':
  case ' ':
  case '.':
  case KEY_B2:
    return MOVE_VALID;

  case '>':
  case '<':
    return valid_stair(dungeon, pc, (char)c);

  case 'm':
    list_monsters(character_map, pc);
    render_dungeon(dungeon, character_map, pc, mh, fog);
    return LIST_MONSTERS;

  case 'T':
    display_tunneling_map(dungeon);
    render_dungeon(dungeon, character_map, pc, mh, fog);
    return TUNNELING_MAP;

  case 'D':
    display_non_tunneling_map(dungeon);
    render_dungeon(dungeon, character_map, pc, mh, fog);
    return NON_TUNNELING_MAP;

  case 's':
    display_default_map(dungeon);
    render_dungeon(dungeon, character_map, pc, mh, fog);
    return DEFAULT_MAP;

  case 'q':
    return QUIT;

  case 'f':
    return FOG_TOGGLE;

  case 't':
    teleport_pc(dungeon, character_map, pc, mh, fog);
    return TELEPORT;

  default:
    return INVALID_KEY;
  }
}

void io_init_terminal() {
  initscr();
  raw();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_CYAN, COLOR_BLACK);
}

void invalid_move() {
  attron(COLOR_PAIR(RED));
  mvprintw(0, 0, "Cannot move to that location!");
  attroff(COLOR_PAIR(RED));
}

void invalid_key() {
  attron(COLOR_PAIR(RED));
  mvprintw(0, 0, "Invalid key.                 ");
  attroff(COLOR_PAIR(RED));
}

void render_dungeon(dungeon_space_t dungeon[][DUNGEON_X],
                    character_t *character_map[][DUNGEON_X], character_t *pc,
                    heap_t *mh, int fog) {
  int x, y, i;
  std::string symbol = "0123456789abcdef@";

  if (mh->size == 0)
    game_over(WIN);
  else {

    clear();

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (fog) {
          if (character_map[y][x] && y >= pc->location.ypos - PC_RADIUS &&
              y <= pc->location.ypos + PC_RADIUS &&
              x >= pc->location.xpos - PC_RADIUS &&
              x <= pc->location.xpos + PC_RADIUS) {
            i = has_characteristic(character_map[y][x]->characteristic, PC)
                    ? CYAN
                    : RED;

            attron(COLOR_PAIR(i));
            mvaddch(y + 1, x, symbol[character_map[y][x]->characteristic]);
            attroff(COLOR_PAIR(i));

          } else {
            mvaddch(y + 1, x, dungeon[y][x].pc_map);
          }
        } else {
          if (character_map[y][x]) {
            i = has_characteristic(character_map[y][x]->characteristic, PC)
                    ? CYAN
                    : RED;

            attron(COLOR_PAIR(i));
            mvaddch(y + 1, x, symbol[character_map[y][x]->characteristic]);
            attroff(COLOR_PAIR(i));
          } else {
            mvaddch(y + 1, x, dungeon[y][x].terrain);
          }
        }
      }
    }

    mvprintw(DISPLAY_MAX_Y - 1, DISPLAY_MAX_X - 9, "Lives: %2d", pc->lives);
    mvprintw(DISPLAY_MAX_Y, DISPLAY_MAX_X - 12, "Monsters: %2d", mh->size);

    refresh();
  }
}

void render_dungeon_first(dungeon_space_t dungeon[][DUNGEON_X],
                          character_t *character_map[][DUNGEON_X],
                          character_t *pc, heap_t *mh, int fog) {
  int x, y, i;
  std::string symbol = "0123456789abcdef@";

  if (mh->size == 0)
    game_over(WIN);
  else {

    clear();

    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (fog) {
          if (character_map[y][x] && y >= pc->location.ypos - PC_RADIUS &&
              y <= pc->location.ypos + PC_RADIUS &&
              x >= pc->location.xpos - PC_RADIUS &&
              x <= pc->location.xpos + PC_RADIUS) {
            i = has_characteristic(character_map[y][x]->characteristic, PC)
                    ? CYAN
                    : RED;

            attron(COLOR_PAIR(i));
            mvaddch(y + 1, x, symbol[character_map[y][x]->characteristic]);
            attroff(COLOR_PAIR(i));

          } else {
            mvaddch(y + 1, x, dungeon[y][x].pc_map);
          }
        } else {
          if (character_map[y][x]) {
            i = has_characteristic(character_map[y][x]->characteristic, PC)
                    ? CYAN
                    : RED;

            attron(COLOR_PAIR(i));
            mvaddch(y + 1, x, symbol[character_map[y][x]->characteristic]);
            attroff(COLOR_PAIR(i));
          } else {
            mvaddch(y + 1, x, dungeon[y][x].terrain);
          }
        }
      }
    }

    mvprintw(DISPLAY_MAX_Y - 1, DISPLAY_MAX_X - 9, "Lives: %2d", pc->lives);
    mvprintw(DISPLAY_MAX_Y, DISPLAY_MAX_X - 12, "Monsters: %2d", mh->size - 1);

    refresh();
  }
}

void display_tunneling_map(dungeon_space_t dungeon[][DUNGEON_X]) {
  int x, y, c;

  do {
    clear();
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        mvprintw(y + 1, x, "%d", dungeon[y][x].cost_t % 10);
      }
    }
    refresh();
  } while ((c = getch()) != 27);
}

void display_non_tunneling_map(dungeon_space_t dungeon[][DUNGEON_X]) {
  int x, y, c;

  do {
    clear();
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (dungeon[y][x].cost_nt == INT_MAX) {
          mvaddch(y + 1, x, ' ');

        } else {
          mvprintw(y + 1, x, "%d", dungeon[y][x].cost_nt % 10);
        }
      }
    }
    refresh();
  } while ((c = getch()) != 27);
}

void display_default_map(dungeon_space_t dungeon[][DUNGEON_X]) {
  int x, y, c;

  do {
    clear();
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        mvaddch(y + 1, x, dungeon[y][x].terrain);
      }
    }
    refresh();
  } while ((c = getch()) != 27);
}

void game_over(int result) {
  clear();

  if (result == LOSE)
    mvprintw(0, 10, gg);
  else if (result == WIN)
    mvprintw(0, 10, victory);
  else {
    attron(A_BOLD);
    mvprintw(10, 20, quitter);
    attroff(A_BOLD);
  }

  refresh();
  getch();
  endwin();
  exit(0);
}
