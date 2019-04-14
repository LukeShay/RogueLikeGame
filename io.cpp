#include <climits>
#include <cstdlib>
#include <cstring>
#include <curses.h>

#include "character_utils.hpp"
#include "io.hpp"

#define DISPLAY_MAX_Y 23
#define DISPLAY_MAX_X 79

typedef enum item_type {
  sym_not_valid,
  sym_weapon,
  sym_offhand,
  sym_ranged,
  sym_armor,
  sym_helmet,
  sym_cloak,
  sym_gloves,
  sym_boots,
  sym_ring,
  sym_amulet,
  sym_light,
  sym_scroll_i,
  sym_book,
  sym_flask,
  sym_gold,
  sym_ammunition,
  sym_food,
  sym_wand,
  sym_container,
  sym_stack
} item_type_t;

const char *symbol = "*|)}[]({\\=\"_~?!$/,-\%&";
const char *equiped_item_symbol = "abcdefghijkl";

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

void render_pc_inventory(character *pc) {
  int print_start = 26, i;
  char c;
  clear();

  mvprintw(0, print_start + 2, "***************");
  mvprintw(1, print_start + 2, "** Inventory **");
  mvprintw(2, print_start + 2, "***************");

  for (i = 0; i < sizeof(pc->inventory) / sizeof(pc->inventory[0]); i++) {

    if (pc->inventory[i]) {
      mvprintw(4 + i, print_start, "%d) %s", i, pc->inventory[i]->name.c_str());

    } else {
      mvprintw(4 + i, print_start, "%d)", i);
    }
  }

  refresh();

  while ((c = getch()) != 27) {
  }
}

void render_pc_equipment(character *pc) {
  int print_start = 26, i;
  char c;
  clear();

  mvprintw(0, print_start + 2, "***************");
  mvprintw(1, print_start + 2, "** Equipment **");
  mvprintw(2, print_start + 2, "***************");

  for (i = 0; i < sizeof(pc->equiped) / sizeof(pc->equiped[0]); i++) {

    if (pc->equiped[i]) {
      mvprintw(4 + i, print_start, "%c) %s", equiped_item_symbol[i],
               pc->equiped[i]->name.c_str());

    } else {
      mvprintw(4 + i, print_start, "%c)", equiped_item_symbol[i]);
    }
  }

  refresh();

  while ((c = getch()) != 27) {
  }
}

void pickup_item(dungeon *d, character *pc) {
  int equiped_slot, inventory_slot;

  if (d->item_map[pc->y][pc->x]) {
    equiped_slot = item_slot(d->item_map[pc->y][pc->x]->type);
    inventory_slot = empty_inventory_slot(pc->inventory);

    if (!pc->equiped[equiped_slot]) {
      pc->equiped[equiped_slot] = d->item_map[pc->y][pc->x];
      d->item_map[pc->y][pc->x] = NULL;

      attron(COLOR_PAIR(COLOR_RED));
      mvprintw(0, 0, "Item put in slot: %c", equiped_item_symbol[equiped_slot]);
      attroff(COLOR_PAIR(COLOR_RED));

    } else if (inventory_slot < 10) {
      pc->inventory[inventory_slot] = d->item_map[pc->y][pc->x];
      d->item_map[pc->y][pc->x] = NULL;

      attron(COLOR_PAIR(COLOR_RED));
      mvprintw(0, 0, "Item put in slot: %d", inventory_slot);
      attroff(COLOR_PAIR(COLOR_RED));
    } else {
      attron(COLOR_PAIR(COLOR_RED));
      mvprintw(0, 0, "Inventory is full!           ");
      attroff(COLOR_PAIR(COLOR_RED));
    }
  } else {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(0, 0, "No item on floor!            ");
    attroff(COLOR_PAIR(COLOR_RED));
  }
}

int item_symbol(item *i) {
  if (!i->type.compare("WEAPON")) {
    return sym_weapon;
  } else if (!i->type.compare("OFFHAND")) {
    return sym_offhand;
  } else if (!i->type.compare("RANGED")) {
    return sym_ranged;
  } else if (!i->type.compare("ARMOR")) {
    return sym_armor;
  } else if (!i->type.compare("HELMET")) {
    return sym_helmet;
  } else if (!i->type.compare("CLOAK")) {
    return sym_cloak;
  } else if (!i->type.compare("GLOVES")) {
    return sym_gloves;
  } else if (!i->type.compare("BOOTS")) {
    return sym_boots;
  } else if (!i->type.compare("RING")) {
    return sym_ring;
  } else if (!i->type.compare("AMULET")) {
    return sym_amulet;
  } else if (!i->type.compare("LIGHT")) {
    return sym_light;
  } else if (!i->type.compare("SCROLL")) {
    return sym_scroll_i;
  } else if (!i->type.compare("BOOK")) {
    return sym_book;
  } else if (!i->type.compare("FLASK")) {
    return sym_flask;
  } else if (!i->type.compare("GOLD")) {
    return sym_gold;
  } else if (!i->type.compare("AMMUNITION")) {
    return sym_ammunition;
  } else if (!i->type.compare("FOOD")) {
    return sym_food;
  } else if (!i->type.compare("WAND")) {
    return sym_wand;
  } else if (!i->type.compare("CONTAINER")) {
    return sym_container;
  } else if (!i->type.compare("STACK")) {
    return sym_stack;
  } else {
    return sym_not_valid;
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

    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(DISPLAY_MAX_Y - 1, 0, "Use arrow keys to scroll");
    mvprintw(DISPLAY_MAX_Y, 0, "Use ESC key to exit");
    attroff(COLOR_PAIR(COLOR_RED));

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

  mvprintw(DISPLAY_MAX_Y - 1, DISPLAY_MAX_X - 9, "   HP: %2d", pc->hp);
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

  while ((c = getch()) != 'g') {
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

void display_item(character *pc, int equiped, int item_slot) {
  if (equiped && pc->equiped[item_slot]) {
    mvprintw(0, 0, "%s", pc->equiped[item_slot]->desc.c_str());

  } else if (pc->inventory[item_slot]) {
    mvprintw(0, 0, "%s", pc->inventory[item_slot]->desc.c_str());
  } else {
    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(0, 0, "No item in that slot.");
    attroff(COLOR_PAIR(COLOR_RED));
  }
}

void inspect_item(character *pc) {
  int c;

  clear();

  attron(COLOR_PAIR(COLOR_RED));
  mvprintw(DISPLAY_MAX_Y - 1, 0, "Enter inventory or carry slot");
  mvprintw(DISPLAY_MAX_Y, 0, "Use \'I\' key to exit");
  attroff(COLOR_PAIR(COLOR_RED));

  refresh();

  while ((c = getch()) != 'I') {
    clear();

    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(DISPLAY_MAX_Y - 1, 0, "Enter inventory or carry slot");
    mvprintw(DISPLAY_MAX_Y, 0, "Use \'I\' key to exit");
    attroff(COLOR_PAIR(COLOR_RED));

    switch (c) {
    case 'a':
      display_item(pc, 1, 0);
      break;
    case 'b':
      display_item(pc, 1, 1);
      break;
    case 'c':
      display_item(pc, 1, 2);
      break;
    case 'd':
      display_item(pc, 1, 3);
      break;
    case 'e':
      display_item(pc, 1, 4);
      break;
    case 'f':
      display_item(pc, 1, 5);
      break;
    case 'g':
      display_item(pc, 1, 6);
      break;
    case 'h':
      display_item(pc, 1, 7);
      break;
    case 'i':
      display_item(pc, 1, 8);
      break;
    case 'j':
      display_item(pc, 1, 9);
      break;
    case 'k':
      display_item(pc, 1, 10);
      break;
    case 'l':
      display_item(pc, 1, 11);
      break;
    case '0':
      display_item(pc, 0, 0);
      break;
    case '1':
      display_item(pc, 0, 1);
      break;
    case '2':
      display_item(pc, 0, 2);
      break;
    case '3':
      display_item(pc, 0, 3);
      break;
    case '4':
      display_item(pc, 0, 4);
      break;
    case '5':
      display_item(pc, 0, 5);
      break;
    case '6':
      display_item(pc, 0, 6);
      break;
    case '7':
      display_item(pc, 0, 7);
      break;
    case '8':
      display_item(pc, 0, 8);
      break;
    case '9':
      display_item(pc, 0, 9);
      break;

    default:
      attron(COLOR_PAIR(COLOR_RED));
      mvprintw(0, 0, "Not an item slot.");
      attroff(COLOR_PAIR(COLOR_RED));
    }
    refresh();
  }
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

  case 'g':
    teleport_pc(d, pc, mh, fog);
    return TELEPORT;

  case 'w':
  case 't':
  case 'd':
  case 'x':
  case 'i':
    render_pc_inventory(pc);
    return TELEPORT;

  case 'e':
    render_pc_equipment(pc);
    return TELEPORT;

  case 'I':
    inspect_item(pc);
    return TELEPORT;
  case 'L':

  case ',':
    pickup_item(d, pc);
    return ITEM_PICKUP;

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

  mvprintw(DISPLAY_MAX_Y - 1, DISPLAY_MAX_X - 9, "   HP: %2d", pc->hp);
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

  mvprintw(DISPLAY_MAX_Y - 1, DISPLAY_MAX_X - 9, "   HP: %2d", pc->hp);
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
