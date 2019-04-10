#include <climits>
#include <cstdlib>
#include <curses.h>
#include <string>

#include "io.hpp"

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

typedef enum item_type {
  not_valid,
  weapon,
  offhand,
  ranged,
  armor,
  helmet,
  cloak,
  gloves,
  boots,
  ring,
  amulet,
  light,
  scroll_i,
  book,
  flask,
  gold,
  ammunition,
  food,
  wand,
  container,
  stack
} item_type_t;

const std::string symbol = "*|)}[]({\\=\"_~?!$/,-\%&";

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

int item_symbol(item *i) {
  if (!i->type.compare("WEAPON")) {
    return weapon;
  } else if (!i->type.compare("OFFHAND")) {
    return offhand;
  } else if (!i->type.compare("RANGED")) {
    return ranged;
  } else if (!i->type.compare("ARMOR")) {
    return armor;
  } else if (!i->type.compare("HELMET")) {
    return helmet;
  } else if (!i->type.compare("CLOAK")) {
    return cloak;
  } else if (!i->type.compare("GLOVES")) {
    return gloves;
  } else if (!i->type.compare("BOOTS")) {
    return boots;
  } else if (!i->type.compare("RING")) {
    return ring;
  } else if (!i->type.compare("AMULET")) {
    return amulet;
  } else if (!i->type.compare("LIGHT")) {
    return light;
  } else if (!i->type.compare("SCROLL")) {
    return scroll_i;
  } else if (!i->type.compare("BOOK")) {
    return book;
  } else if (!i->type.compare("FLASK")) {
    return flask;
  } else if (!i->type.compare("GOLD")) {
    return gold;
  } else if (!i->type.compare("AMMUNITION")) {
    return ammunition;
  } else if (!i->type.compare("FOOD")) {
    return food;
  } else if (!i->type.compare("WAND")) {
    return wand;
  } else if (!i->type.compare("CONTAINER")) {
    return container;
  } else if (!i->type.compare("STACK")) {
    return stack;
  } else {
    return not_valid;
  }
}

int valid_move(dungeon *d, uint8_t x, uint8_t y, character *pc) {
  if (d->hardness_map[y][x] != 0)
    return MOVE_INVALID;

  if (d->character_map[y][x]) {
    d->character_map[y][x]->hp = 0;
  }

  d->character_map[pc->y][pc->x] = NULL;
  pc->y = y;
  pc->x = x;

  d->character_map[y][x] = pc;

  return MOVE_VALID;
}

int valid_stair(dungeon *d, character *pc, char s) {
  if (s == d->terrain_map[pc->y][pc->x])
    return MOVE_STAIR;
  else
    return MOVE_INVALID;
}

int list_monsters(dungeon *d, character *pc) {
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
        if (d->character_map[i][j] &&
            !has_characteristic(d->character_map[i][j]->abilities, PC)) {
          if (num >= num_initial) {
            num_on_screen++;
            k++;

            mvprintw(k, print_start, "%c:", d->character_map[i][j]->symbol);

            if (i - pc->y < 0) {
              mvprintw(k, print_start + 2, "%3d north, ", pc->y - i);
            } else {
              mvprintw(k, print_start + 2, "%3d south, ", i - pc->y);
            }

            if (j - pc->x < 0) {
              mvprintw(k, print_start + 13, "%2d west", pc->x - j);
            } else {
              mvprintw(k, print_start + 13, "%2d east", j - pc->x);
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

void render_dungeon_teleport(dungeon *d, character *pc, heap_t *mh, int fog) {
  int x, y, i;

  clear();

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (x == pc->x && y == pc->y) {

        attron(COLOR_PAIR(COLOR_CYAN));
        mvaddch(y + 1, x, '@');
        attroff(COLOR_PAIR(COLOR_CYAN));
      } else {
        if (d->character_map[y][x] && d->character_map[y][x] != pc) {
          if (!d->character_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->character_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->character_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->character_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->character_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->character_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->character_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, d->character_map[y][x]->symbol);
          attroff(COLOR_PAIR(i));
        } else if (d->item_map[y][x]) {
          if (!d->item_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->item_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->item_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->item_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->item_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->item_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->item_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, symbol[item_symbol(d->item_map[y][x])]);
          attroff(COLOR_PAIR(i));
        } else {
          mvaddch(y + 1, x, d->terrain_map[y][x]);
        }
      }
    }
  }

  mvprintw(DISPLAY_MAX_Y - 1, DISPLAY_MAX_X - 9, "Lives: %2d", pc->hp);
  mvprintw(DISPLAY_MAX_Y, DISPLAY_MAX_X - 12, "Monsters: %2d", mh->size);

  refresh();
}

void teleport_pc(dungeon *d, character *pc, heap_t *mh, int fog) {
  int c;
  int new_x, new_y;

  d->character_map[pc->y][pc->x] = NULL;

  render_dungeon_teleport(d, pc, mh, 0);
  attron(COLOR_PAIR(COLOR_RED));
  mvprintw(0, 0, "Teleporting                  ");
  attroff(COLOR_PAIR(COLOR_RED));

  while ((c = getch()) != 't') {
    switch (c) {
    case 'y':
    case '7':
    case KEY_HOME:
      new_x = pc->x - 1;
      new_y = pc->y - 1;
      goto move;

    case 'k':
    case '8':
    case KEY_UP:
      new_x = pc->x;
      new_y = pc->y - 1;
      goto move;

    case 'u':
    case '9':
    case KEY_PPAGE:
      new_x = pc->x + 1;
      new_y = pc->y - 1;
      goto move;

    case 'l':
    case '6':
    case KEY_RIGHT:
      new_x = pc->x + 1;
      new_y = pc->y;
      goto move;

    case 'n':
    case '3':
    case KEY_NPAGE:
      new_x = pc->x + 1;
      new_y = pc->y + 1;
      goto move;

    case 'j':
    case '2':
    case KEY_DOWN:
      new_x = pc->x;
      new_y = pc->y + 1;
      goto move;

    case 'b':
    case '1':
    case KEY_END:
      new_x = pc->x - 1;
      new_y = pc->y + 1;
      goto move;

    case 'h':
    case '4':
    case KEY_LEFT:
      new_x = pc->x - 1;
      new_y = pc->y;
      goto move;

    case 'r':
      new_x = rand() % 77 + 1;
      new_y = rand() % 18 + 1;
      goto move;
    }
  move:
    if (new_y < 20 && new_y > 0 && new_x < 79 && new_x > 0) {

      pc->y = new_y;
      pc->x = new_x;

      if (c == 'r')
        goto done;
    }
    render_dungeon_teleport(d, pc, mh, 0);
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(0, 0, "Teleporting                  ");
    attroff(COLOR_PAIR(COLOR_RED));
  }

done:

  if (d->character_map[pc->y][pc->x]) {
    d->character_map[pc->y][pc->x]->hp = 0;
  }

  d->character_map[pc->y][pc->x] = pc;

  render_dungeon(d, pc, mh, fog);
}

int move_pc(dungeon *d, character *pc, heap_t *mh, int fog) {
  int c = getch();
  uint8_t new_x, new_y;

  switch (c) {
  case 'y':
  case '7':
  case KEY_HOME:
    new_x = pc->x - 1;
    new_y = pc->y - 1;
    return valid_move(d, new_x, new_y, pc);

  case 'k':
  case '8':
  case KEY_UP:
    new_x = pc->x;
    new_y = pc->y - 1;
    return valid_move(d, new_x, new_y, pc);

  case 'u':
  case '9':
  case KEY_PPAGE:
    new_x = pc->x + 1;
    new_y = pc->y - 1;
    return valid_move(d, new_x, new_y, pc);

  case 'l':
  case '6':
  case KEY_RIGHT:
    new_x = pc->x + 1;
    new_y = pc->y;
    return valid_move(d, new_x, new_y, pc);

  case 'n':
  case '3':
  case KEY_NPAGE:
    new_x = pc->x + 1;
    new_y = pc->y + 1;
    return valid_move(d, new_x, new_y, pc);

  case 'j':
  case '2':
  case KEY_DOWN:
    new_x = pc->x;
    new_y = pc->y + 1;
    return valid_move(d, new_x, new_y, pc);

  case 'b':
  case '1':
  case KEY_END:
    new_x = pc->x - 1;
    new_y = pc->y + 1;
    return valid_move(d, new_x, new_y, pc);

  case 'h':
  case '4':
  case KEY_LEFT:
    new_x = pc->x - 1;
    new_y = pc->y;
    return valid_move(d, new_x, new_y, pc);

  case '5':
  case ' ':
  case '.':
  case KEY_B2:
    return MOVE_VALID;

  case '>':
  case '<':
    return valid_stair(d, pc, (char)c);

  case 'm':
    list_monsters(d, pc);
    render_dungeon(d, pc, mh, fog);
    return LIST_MONSTERS;

  case 'T':
    display_tunneling_map(d);
    render_dungeon(d, pc, mh, fog);
    return TUNNELING_MAP;

  case 'D':
    display_non_tunneling_map(d);
    render_dungeon(d, pc, mh, fog);
    return NON_TUNNELING_MAP;

  case 's':
    display_default_map(d);
    render_dungeon(d, pc, mh, fog);
    return DEFAULT_MAP;

  case 'q':
    return QUIT;

  case 'f':
    return FOG_TOGGLE;

  case 't':
    teleport_pc(d, pc, mh, fog);
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
  init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
  init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
  init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
  init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
  init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_BLACK, COLOR_WHITE, COLOR_BLACK);
  init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
  init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
}

void invalid_move() {
  attron(COLOR_PAIR(COLOR_RED));
  mvprintw(0, 0, "Cannot move to that location!");
  attroff(COLOR_PAIR(COLOR_RED));
}

void invalid_key() {
  attron(COLOR_PAIR(COLOR_RED));
  mvprintw(0, 0, "Invalid key.                 ");
  attroff(COLOR_PAIR(COLOR_RED));
}

void render_dungeon(dungeon *d, character *pc, heap_t *mh, int fog) {
  int x, y, i;

  clear();

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (fog) {
        if (d->character_map[y][x] && y >= pc->y - PC_RADIUS &&
            y <= pc->y + PC_RADIUS && x >= pc->x - PC_RADIUS &&
            x <= pc->x + PC_RADIUS) {

          if (!d->character_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->character_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->character_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->character_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->character_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->character_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->character_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, d->character_map[y][x]->symbol);
          attroff(COLOR_PAIR(i));

        } else if (d->item_map[y][x] && y >= pc->y - PC_RADIUS &&
                   y <= pc->y + PC_RADIUS && x >= pc->x - PC_RADIUS &&
                   x <= pc->x + PC_RADIUS) {
          if (!d->item_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->item_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->item_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->item_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->item_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->item_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->item_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, symbol[item_symbol(d->item_map[y][x])]);
          attroff(COLOR_PAIR(i));

        } else {
          mvaddch(y + 1, x, d->pc_map[y][x]);
        }
      } else {
        if (d->character_map[y][x]) {
          if (!d->character_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->character_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->character_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->character_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->character_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->character_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->character_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, d->character_map[y][x]->symbol);
          attroff(COLOR_PAIR(i));
        } else if (d->item_map[y][x]) {
          if (!d->item_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->item_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->item_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->item_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->item_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->item_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->item_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, symbol[item_symbol(d->item_map[y][x])]);
          attroff(COLOR_PAIR(i));

        } else {
          mvaddch(y + 1, x, d->terrain_map[y][x]);
        }
      }
    }
  }

  mvprintw(DISPLAY_MAX_Y - 1, DISPLAY_MAX_X - 9, "Lives: %2d", pc->hp);
  mvprintw(DISPLAY_MAX_Y, DISPLAY_MAX_X - 12, "Monsters: %2d", mh->size);

  refresh();
}

void render_dungeon_first(dungeon *d, character *pc, heap_t *mh, int fog) {
  int x, y, i;

  clear();

  for (y = 0; y < DUNGEON_Y; y++) {
    for (x = 0; x < DUNGEON_X; x++) {
      if (fog) {
        if (d->character_map[y][x] && y >= pc->y - PC_RADIUS &&
            y <= pc->y + PC_RADIUS && x >= pc->x - PC_RADIUS &&
            x <= pc->x + PC_RADIUS) {
          if (!d->character_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->character_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->character_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->character_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->character_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->character_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->character_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, d->character_map[y][x]->symbol);
          attroff(COLOR_PAIR(i));

        } else if (d->item_map[y][x] && y >= pc->y - PC_RADIUS &&
                   y <= pc->y + PC_RADIUS && x >= pc->x - PC_RADIUS &&
                   x <= pc->x + PC_RADIUS) {
          if (!d->item_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->item_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->item_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->item_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->item_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->item_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->item_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, symbol[item_symbol(d->item_map[y][x])]);
          attroff(COLOR_PAIR(i));

        } else {
          mvaddch(y + 1, x, d->pc_map[y][x]);
        }
      } else {
        if (d->character_map[y][x]) {
          if (!d->character_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->character_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->character_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->character_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->character_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->character_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->character_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, d->character_map[y][x]->symbol);
          attroff(COLOR_PAIR(i));
        } else if (d->item_map[y][x]) {
          if (!d->item_map[y][x]->color.compare("RED")) {
            i = COLOR_RED;
          } else if (!d->item_map[y][x]->color.compare("BLUE")) {
            i = COLOR_BLUE;
          } else if (!d->item_map[y][x]->color.compare("MAGENTA")) {
            i = COLOR_MAGENTA;
          } else if (!d->item_map[y][x]->color.compare("YELLOW")) {
            i = COLOR_YELLOW;
          } else if (!d->item_map[y][x]->color.compare("BLACK")) {
            i = COLOR_BLACK;
          } else if (!d->item_map[y][x]->color.compare("CYAN")) {
            i = COLOR_CYAN;
          } else if (!d->item_map[y][x]->color.compare("GREEN")) {
            i = COLOR_GREEN;
          } else {
            i = COLOR_WHITE;
          }

          attron(COLOR_PAIR(i));
          mvaddch(y + 1, x, symbol[item_symbol(d->item_map[y][x])]);
          attroff(COLOR_PAIR(i));

        } else {
          mvaddch(y + 1, x, d->terrain_map[y][x]);
        }
      }
    }
  }

  mvprintw(DISPLAY_MAX_Y - 1, DISPLAY_MAX_X - 9, "Lives: %2d", pc->hp);
  mvprintw(DISPLAY_MAX_Y, DISPLAY_MAX_X - 12, "Monsters: %2d", mh->size - 1);

  refresh();
}

void display_tunneling_map(dungeon *d) {
  int x, y, c;

  do {
    clear();
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        mvprintw(y + 1, x, "%d", d->cost_t_map[y][x] % 10);
      }
    }
    refresh();
  } while ((c = getch()) != 27);
}

void display_non_tunneling_map(dungeon *d) {
  int x, y, c;

  do {
    clear();
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        if (d->cost_nt_map[y][x] == INT_MAX) {
          mvaddch(y + 1, x, ' ');

        } else {
          mvprintw(y + 1, x, "%d", d->cost_nt_map[y][x] % 10);
        }
      }
    }
    refresh();
  } while ((c = getch()) != 27);
}

void display_default_map(dungeon *d) {
  int x, y, c;

  do {
    clear();
    for (y = 0; y < DUNGEON_Y; y++) {
      for (x = 0; x < DUNGEON_X; x++) {
        mvaddch(y + 1, x, d->terrain_map[y][x]);
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
