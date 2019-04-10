#include <climits>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdint.h>

#include "character_utils.hpp"

#define MIN_NUM_MONS 10

int can_see_PC(character *npc, character *pc) {
  int x = 10, y = 4;

  if (npc->x - x < pc->x && npc->x + x >= pc->x && npc->y - y < pc->y &&
      npc->y + y >= pc->y)
    return 1;
  else
    return 0;
}

point_t get_smallest_neighbor_tunneling(point_t p, dungeon *d) {
  point_t temp;
  int cost = INT_MAX;
  uint8_t i, j;

  for (i = p.ypos - 1; i <= p.ypos + 1; i++) {
    for (j = p.xpos - 1; j <= p.xpos + 1; j++) {
      if (d->cost_t_map[i][j] < cost && (j != p.xpos || i != p.ypos)) {
        cost = d->cost_t_map[i][j];
        temp.xpos = j;
        temp.ypos = i;
      }
    }
  }
  return temp;
}

point_t get_smallest_neighbor_non_tunneling(point_t p, dungeon *d) {
  point_t temp;
  int cost = INT_MAX;
  uint8_t i, j;

  for (i = p.ypos - 1; i <= p.ypos + 1; i++) {
    for (j = p.xpos - 1; j <= p.xpos + 1; j++) {
      if (d->cost_nt_map[i][j] < cost && d->hardness_map[i][j] == 0 &&
          (j != p.xpos || i != p.ypos)) {
        cost = d->cost_nt_map[i][j];
        temp.xpos = j;
        temp.ypos = i;
      }
    }
  }
  return temp;
}

static void npc_0(character *npc, dungeon *d, character *pc) {
  d->character_map[npc->y][npc->x] = NULL;

  switch (npc->direction) {
  case 0:
    if (d->hardness_map[npc->y - 1][npc->x] != 0)
      npc->direction = rand() % 4;
    else
      npc->y -= 1;
    break;

  case 1:
    if (d->hardness_map[npc->y][npc->x + 1] != 0)
      npc->direction = rand() % 4;
    else
      npc->x += 1;
    break;

  case 2:
    if (d->hardness_map[npc->y + 1][npc->x] != 0)
      npc->direction = rand() % 4;
    else
      npc->y += 1;
    break;

  case 3:
    if (d->hardness_map[npc->y][npc->x - 1] != 0)
      npc->direction = rand() % 4;
    else
      npc->x -= 1;
    break;

  default:
    break;
  }

  if (d->character_map[npc->y][npc->x]) {
    d->character_map[npc->y][npc->x]->hp--;

    if (has_characteristic(d->character_map[npc->y][npc->x]->abilities, PC)) {
      npc->hp = 0;
    } else {
      d->character_map[npc->y][npc->x] = npc;
    }
  } else
    d->character_map[npc->y][npc->x] = npc;
}

static void npc_1(character *npc, dungeon *d, character *pc) {
  point_t p;
  p.xpos = npc->x;
  p.ypos = npc->y;

  if ((can_see_PC(npc, pc))) {
    p = get_smallest_neighbor_non_tunneling(p, d);
  }

  d->character_map[npc->y][npc->x] = NULL;

  npc->x = p.xpos;
  npc->y = p.ypos;

  if (d->character_map[npc->y][npc->x]) {
    d->character_map[npc->y][npc->x]->hp--;
    if (has_characteristic(d->character_map[npc->y][npc->x]->abilities, PC)) {
      npc->hp = 0;
    } else {
      d->character_map[npc->y][npc->x]->hp--;
      d->character_map[npc->y][npc->x] = npc;
    }
  } else
    d->character_map[npc->y][npc->x] = npc;
}

static void npc_2(character *npc, dungeon *d, character *pc) {
  uint8_t x_direction = 0, y_direction = 0;

  if (npc->x != pc->x) {
    x_direction = (pc->x - npc->x) / (uint8_t)abs(pc->x - npc->x);
  }

  if (npc->y != pc->y) {
    y_direction = (pc->y - npc->y) / (uint8_t)abs(pc->y - npc->y);
  }

  uint8_t temp_x = npc->x + x_direction, temp_y = npc->y + y_direction;

  if (d->hardness_map[temp_y][temp_x] == 0) {
    d->character_map[npc->y][npc->x] = NULL;

    npc->x = temp_x;
    npc->y = temp_y;

    if (d->character_map[npc->y][npc->x]) {
      d->character_map[npc->y][npc->x]->hp--;

      if (has_characteristic(d->character_map[npc->y][npc->x]->abilities, PC)) {
        npc->hp = 0;
      } else {
        d->character_map[npc->y][npc->x] = npc;
      }
    } else
      d->character_map[npc->y][npc->x] = npc;
  }
}

static void npc_3(character *npc, dungeon *d, character *pc) {
  point_t p;
  p.xpos = npc->x;
  p.ypos = npc->y;

  p = get_smallest_neighbor_non_tunneling(p, d);

  d->character_map[npc->y][npc->x] = NULL;

  npc->x = p.xpos;
  npc->y = p.ypos;

  if (d->character_map[npc->y][npc->x]) {
    d->character_map[npc->y][npc->x]->hp--;
    if (has_characteristic(d->character_map[npc->y][npc->x]->abilities, PC)) {
      npc->hp = 0;
    } else {
      d->character_map[npc->y][npc->x] = npc;
    }
  } else
    d->character_map[npc->y][npc->x] = npc;
}

static void npc_4(character *npc, dungeon *d, character *pc) {
  d->character_map[npc->y][npc->x] = NULL;

  switch (npc->direction) {
  case 0:
    if (d->hardness_map[npc->y - 1][npc->x] == 255)
      npc->direction = rand() % 4;
    else if (d->hardness_map[npc->y - 1][npc->x] == 0) {
      npc->y -= 1;
    } else if (d->hardness_map[npc->y - 1][npc->x] < 86) {
      d->hardness_map[npc->y - 1][npc->x] = 0;
      d->terrain_map[npc->y - 1][npc->x] = '#';
      npc->y -= 1;
    } else {
      d->hardness_map[npc->y - 1][npc->x] -= 85;
    }
    break;

  case 1:
    if (d->hardness_map[npc->y][npc->x + 1] == 255)
      npc->direction = rand() % 4;
    else if (d->hardness_map[npc->y][npc->x + 1] == 0) {
      npc->x += 1;
    } else if (d->hardness_map[npc->y][npc->x + 1] < 86) {
      d->hardness_map[npc->y][npc->x + 1] = 0;
      d->terrain_map[npc->y][npc->x + 1] = '#';
      npc->x += 1;
    } else {
      d->hardness_map[npc->y][npc->x + 1] -= 85;
    }
    break;

  case 2:
    if (d->hardness_map[npc->y + 1][npc->x] == 255)
      npc->direction = rand() % 4;
    else if (d->hardness_map[npc->y + 1][npc->x] == 0) {
      npc->y += 1;
    } else if (d->hardness_map[npc->y + 1][npc->x] < 86) {
      d->hardness_map[npc->y + 1][npc->x] = 0;
      d->terrain_map[npc->y + 1][npc->x] = '#';
      npc->y += 1;
    } else {
      d->hardness_map[npc->y + 1][npc->x] -= 85;
    }
    break;

  case 3:
    if (d->hardness_map[npc->y][npc->x - 1] == 255)
      npc->direction = rand() % 4;
    else if (d->hardness_map[npc->y][npc->x - 1] == 0) {
      npc->x -= 1;
    } else if (d->hardness_map[npc->y][npc->x - 1] < 86) {
      d->hardness_map[npc->y][npc->x - 1] = 0;
      d->terrain_map[npc->y][npc->x - 1] = '#';
      npc->x -= 1;
    } else {
      d->hardness_map[npc->y][npc->x + 1] -= 85;
    }
    break;

  default:
    break;
  }

  if (d->character_map[npc->y][npc->x]) {
    d->character_map[npc->y][npc->x]->hp--;

    if (has_characteristic(d->character_map[npc->y][npc->x]->abilities, PC)) {
      npc->hp = 0;
    } else {
      d->character_map[npc->y][npc->x] = npc;
    }
  } else {
    d->character_map[npc->y][npc->x] = npc;
  }
}

static void npc_5(character *npc, dungeon *d, character *pc) {
  point_t p;
  p.xpos = npc->x;
  p.ypos = npc->y;

  if ((can_see_PC(npc, pc))) {
    p = get_smallest_neighbor_tunneling(p, d);
  }

  d->character_map[npc->y][npc->x] = NULL;

  if (d->hardness_map[p.ypos][p.xpos] == 0) {
    npc->y = p.ypos;
    npc->x = p.xpos;

    if (d->character_map[npc->y][npc->x]) {
      d->character_map[npc->y][npc->x]->hp--;
      if (has_characteristic(d->character_map[npc->y][npc->x]->abilities, PC)) {
        npc->hp = 0;
      } else {
        d->character_map[npc->y][npc->x] = npc;
      }
    } else
      d->character_map[npc->y][npc->x] = npc;
  } else if (d->hardness_map[p.ypos][p.xpos] < 86) {
    npc->y = p.ypos;
    npc->x = p.xpos;

    d->hardness_map[p.ypos][p.xpos] = 0;
    d->terrain_map[p.ypos][p.xpos] = '#';

    d->character_map[npc->y][npc->x] = npc;
  } else {
    d->hardness_map[p.ypos][p.xpos] -= 85;
    d->character_map[npc->y][npc->x] = npc;
  }
}

static void npc_6(character *npc, dungeon *d, character *pc) {
  uint8_t x_direction = 0, y_direction = 0;

  if (npc->x != pc->x) {
    x_direction = (pc->x - npc->x) / (uint8_t)abs(pc->x - npc->x);
  }

  if (npc->y != pc->y) {
    y_direction = (pc->y - npc->y) / (uint8_t)abs(pc->y - npc->y);
  }

  uint8_t temp_x = npc->x + x_direction, temp_y = npc->y + y_direction;

  if (d->hardness_map[temp_y][temp_x] == 0) {
    d->character_map[npc->y][npc->x] = NULL;

    npc->x = temp_x;
    npc->y = temp_y;

    if (d->character_map[npc->y][npc->x]) {
      d->character_map[npc->y][npc->x]->hp--;

      if (has_characteristic(d->character_map[npc->y][npc->x]->abilities, PC)) {
        npc->hp = 0;
      } else {
        d->character_map[npc->y][npc->x] = npc;
      }
    } else
      d->character_map[npc->y][npc->x] = npc;
  } else if (d->hardness_map[temp_y][temp_x] < 86) {
    d->character_map[npc->y][npc->x] = NULL;

    npc->y = temp_y;
    npc->x = temp_x;

    d->hardness_map[npc->y][npc->x] = 0;
    d->terrain_map[npc->y][npc->x] = '#';

    d->character_map[npc->y][npc->x] = npc;
  } else if (d->hardness_map[temp_y][temp_x] < 255) {
    d->hardness_map[temp_y][temp_x] -= 85;
  }
}

static void npc_7(character *npc, dungeon *d, character *pc) {
  point_t p;
  p.xpos = npc->x;
  p.ypos = npc->y;

  p = get_smallest_neighbor_non_tunneling(p, d);

  if (d->hardness_map[p.ypos][p.xpos] == 0) {
    d->character_map[npc->y][npc->x] = NULL;

    npc->y = p.ypos;
    npc->x = p.xpos;

    if (d->character_map[p.ypos][p.xpos]) {
      d->character_map[p.ypos][p.xpos]->hp--;
      if (d->character_map[p.ypos][p.xpos] == pc) {
        npc->hp = 0;
      } else {
        d->character_map[npc->y][npc->x] = npc;
      }
    } else {
      d->character_map[npc->y][npc->x] = npc;
    }
  } else if (d->hardness_map[p.ypos][p.xpos] < 86) {
    d->character_map[npc->y][npc->x] = NULL;

    npc->y = p.ypos;
    npc->x = p.xpos;

    d->hardness_map[p.ypos][p.xpos] = 0;
    d->terrain_map[p.ypos][p.xpos] = '#';

    d->character_map[npc->y][npc->x] = npc;
  } else if (d->hardness_map[p.ypos][p.xpos] < 255) {
    d->hardness_map[p.ypos][p.xpos] -= 85;
  }
}

void place_monster(character *c, dungeon *d, int num_spaces, int num_mon) {
  int k = 1, rand_x = 1, rand_y = 1, redos = 0;
  uint8_t i, j;

  for (i = 0; i < DUNGEON_Y && k <= num_mon; i += rand_y) {
    if (num_mon == num_spaces || redos >= 3000) {
      for (j = 1; j < DUNGEON_X - 1 && k <= num_mon; j++) {
        if (d->terrain_map[i][j] != ' ' && d->character_map[i][j] == NULL) {
          c->x = j;
          c->y = i;

          k++;
        }
      }
      rand_y = 1;
    } else {
      for (j = 0; j < DUNGEON_X && k <= num_mon; j += rand_x) {
        if (d->hardness_map[i][j] == 0 && d->character_map[i][j] == NULL) {
          c->x = j;
          c->y = i;

          rand_x = rand() % num_spaces / num_mon + 4;
          k++;
        }
        redos++;
      }
      rand_y = rand() % 3 + 1;
    }
  }
}

void pc_init(character *pc_char, point_t pc, int num_lives) {
  pc_char->abilities = 128;
  pc_char->speed = 10;
  pc_char->p = 10;
  pc_char->hp = num_lives;
  pc_char->name = "PC";
  pc_char->symbol = '@';
  pc_char->color = "CYAN";

  pc_char->x = pc.xpos;
  pc_char->y = pc.ypos;
}

void generate_monsters(dungeon *d, heap_t *mh,
                       std::vector<character_desc> *mv) {
  int rand_num, num_mons = 0, min_mons = rand() % 3 + MIN_NUM_MONS;
  character *c;
  std::vector<character_desc>::iterator it;

not_enough_monsters:
  for (it = mv->begin(); it != mv->end(); it++) {
    rand_num = rand() % 100;
    if (rand_num < it->rarity && it->placed != 1) {
      c = new character;

      c->name = it->name;
      c->color = it->color;
      c->desc = it->desc;
      c->abilities = it->parse_abilities();
      c->symbol = it->symbol;
      c->speed = it->speed.roll_dice();
      c->hp = it->hp.roll_dice();
      c->ad.parse_dice(it->ad.form);
      c->p = 1000 / c->speed;
      c->direction = rand() % 4;

      c->pick_location(d->character_map, d->hardness_map);

      heap_insert(mh, c);
      num_mons++;

      if ((c->abilities & UNIQ)) {
        it->placed = 1;
      }
    }
    if (num_mons > min_mons) {
      break;
    }
  }

  if (num_mons < min_mons) {
    goto not_enough_monsters;
  }
}

void npc_erratic(character *npc, dungeon *d, character *pc);

void (*npc_move_func[])(character *npc, dungeon *d, character *pc) = {
    npc_0,       npc_1,       npc_2,       npc_3,
    npc_4,       npc_5,       npc_6,       npc_7,
    npc_erratic, npc_erratic, npc_erratic, npc_erratic,
    npc_erratic, npc_erratic, npc_erratic, npc_erratic};

void npc_erratic(character *npc, dungeon *d, character *pc) {
  int e = rand() % 2;
  if (e) {
    npc->direction = rand() % 4;
    npc_0(npc, d, pc);
  } else {
    npc_move_func[npc->abilities & 0x00000007](npc, d, pc);
  }
}

void move_monster(dungeon *d, character *to_move, character *pc) {
  npc_move_func[0](to_move, d, pc); // to_move->abilities & MOVEMENT_ABILITIES
}
