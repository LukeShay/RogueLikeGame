#include <climits>
#include <stdint.h>
#include <cstdlib>
#include <cstring>

#include "characters.h"
#include "path.h"

int can_see_PC(character_t *npc, character_t *pc)
{
  int x = 10, y = 4;

  if (npc->location.xpos - x < pc->location.xpos &&
      npc->location.xpos + x >= pc->location.xpos &&
      npc->location.ypos - y < pc->location.ypos &&
      npc->location.ypos + y >= pc->location.ypos)
    return 1;
  else
    return 0;
}

point_t get_smallest_neighbor_tunneling(point_t p,
                                        dungeon_space_t dungeon[][DUNGEON_X])
{
  point_t temp;
  dungeon_space_t space;
  space.cost_t = INT_MAX;
  uint8_t i, j;

  for (i = p.ypos - 1; i <= p.ypos + 1; i++)
  {
    for (j = p.xpos - 1; j <= p.xpos + 1; j++)
    {
      if (dungeon[i][j].cost_t < space.cost_t && (j != p.xpos || i != p.ypos))
      {
        space = dungeon[i][j];
        temp.xpos = j;
        temp.ypos = i;
      }
    }
  }
  return temp;
}

point_t
get_smallest_neighbor_non_tunneling(point_t p,
                                    dungeon_space_t dungeon[][DUNGEON_X])
{
  point_t temp;
  dungeon_space_t space;
  space.cost_nt = INT_MAX;
  uint8_t i, j;

  for (i = p.ypos - 1; i <= p.ypos + 1; i++)
  {
    for (j = p.xpos - 1; j <= p.xpos + 1; j++)
    {
      if (dungeon[i][j].cost_nt < space.cost_nt &&
          dungeon[i][j].hardness == 0 && (j != p.xpos || i != p.ypos))
      {
        space = dungeon[i][j];
        temp.xpos = j;
        temp.ypos = i;
      }
    }
  }
  return temp;
}

static void npc_0(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                  character_t *character_map[][DUNGEON_X], character_t *pc)
{
  character_map[npc->location.ypos][npc->location.xpos] = NULL;

  switch (npc->direction)
  {
  case 0:
    if (dungeon[npc->location.ypos - 1][npc->location.xpos].hardness != 0)
      npc->direction = rand() % 4;
    else
      npc->location.ypos -= 1;
    break;

  case 1:
    if (dungeon[npc->location.ypos][npc->location.xpos + 1].hardness != 0)
      npc->direction = rand() % 4;
    else
      npc->location.xpos += 1;
    break;

  case 2:
    if (dungeon[npc->location.ypos + 1][npc->location.xpos].hardness != 0)
      npc->direction = rand() % 4;
    else
      npc->location.ypos += 1;
    break;

  case 3:
    if (dungeon[npc->location.ypos][npc->location.xpos - 1].hardness != 0)
      npc->direction = rand() % 4;
    else
      npc->location.xpos -= 1;
    break;

  default:
    break;
  }

  if (character_map[npc->location.ypos][npc->location.xpos])
  {
    character_map[npc->location.ypos][npc->location.xpos]->lives--;

    if (has_characteristic(character_map[npc->location.ypos][npc->location.xpos]
                               ->characteristic,
                           PC))
    {
      npc->lives = 0;
    }
    else
    {
      character_map[npc->location.ypos][npc->location.xpos] = npc;
    }
  }
  else
    character_map[npc->location.ypos][npc->location.xpos] = npc;
}

static void npc_1(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                  character_t *character_map[][DUNGEON_X], character_t *pc)
{
  point_t p = npc->location;

  if ((can_see_PC(npc, pc)))
  {
    p = get_smallest_neighbor_non_tunneling(p, dungeon);
  }

  character_map[npc->location.ypos][npc->location.xpos] = NULL;

  npc->location.xpos = p.xpos;
  npc->location.ypos = p.ypos;

  if (character_map[npc->location.ypos][npc->location.xpos])
  {
    character_map[npc->location.ypos][npc->location.xpos]->lives--;
    if (has_characteristic(character_map[npc->location.ypos][npc->location.xpos]
                               ->characteristic,
                           PC))
    {
      npc->lives = 0;
    }
    else
    {
      character_map[npc->location.ypos][npc->location.xpos]->lives--;
      character_map[npc->location.ypos][npc->location.xpos] = npc;
    }
  }
  else
    character_map[npc->location.ypos][npc->location.xpos] = npc;
}

static void npc_2(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                  character_t *character_map[][DUNGEON_X], character_t *pc)
{
  uint8_t x_direction = 0, y_direction = 0;

  if (npc->location.xpos != pc->location.xpos)
  {
    x_direction = (pc->location.xpos - npc->location.xpos) /
                  (uint8_t)abs(pc->location.xpos - npc->location.xpos);
  }

  if (npc->location.ypos != pc->location.ypos)
  {
    y_direction = (pc->location.ypos - npc->location.ypos) /
                  (uint8_t)abs(pc->location.ypos - npc->location.ypos);
  }

  uint8_t temp_x = npc->location.xpos + x_direction,
          temp_y = npc->location.ypos + y_direction;

  if (dungeon[temp_y][temp_x].hardness == 0)
  {
    character_map[npc->location.ypos][npc->location.xpos] = NULL;

    npc->location.xpos = temp_x;
    npc->location.ypos = temp_y;

    if (character_map[npc->location.ypos][npc->location.xpos])
    {
      character_map[npc->location.ypos][npc->location.xpos]->lives--;

      if (has_characteristic(
              character_map[npc->location.ypos][npc->location.xpos]
                  ->characteristic,
              PC))
      {
        npc->lives = 0;
      }
      else
      {
        character_map[npc->location.ypos][npc->location.xpos] = npc;
      }
    }
    else
      character_map[npc->location.ypos][npc->location.xpos] = npc;
  }
}

static void npc_3(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                  character_t *character_map[][DUNGEON_X], character_t *pc)
{
  point_t p;

  p = get_smallest_neighbor_non_tunneling(npc->location, dungeon);

  character_map[npc->location.ypos][npc->location.xpos] = NULL;

  npc->location.xpos = p.xpos;
  npc->location.ypos = p.ypos;

  if (character_map[npc->location.ypos][npc->location.xpos])
  {
    character_map[npc->location.ypos][npc->location.xpos]->lives--;
    if (has_characteristic(character_map[npc->location.ypos][npc->location.xpos]
                               ->characteristic,
                           PC))
    {
      npc->lives = 0;
    }
    else
    {
      character_map[npc->location.ypos][npc->location.xpos] = npc;
    }
  }
  else
    character_map[npc->location.ypos][npc->location.xpos] = npc;
}

static void npc_4(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                  character_t *character_map[][DUNGEON_X], character_t *pc)
{
  character_map[npc->location.ypos][npc->location.xpos] = NULL;

  switch (npc->direction)
  {
  case 0:
    if (dungeon[npc->location.ypos - 1][npc->location.xpos].hardness == 255)
      npc->direction = rand() % 4;
    else if (dungeon[npc->location.ypos - 1][npc->location.xpos].hardness ==
             0)
    {
      npc->location.ypos -= 1;
    }
    else if (dungeon[npc->location.ypos - 1][npc->location.xpos].hardness <
             86)
    {
      dungeon[npc->location.ypos - 1][npc->location.xpos].hardness = 0;
      dungeon[npc->location.ypos - 1][npc->location.xpos].terrain = '#';
      npc->location.ypos -= 1;
    }
    else
    {
      dungeon[npc->location.ypos - 1][npc->location.xpos].hardness -= 85;
    }
    break;

  case 1:
    if (dungeon[npc->location.ypos][npc->location.xpos + 1].hardness == 255)
      npc->direction = rand() % 4;
    else if (dungeon[npc->location.ypos][npc->location.xpos + 1].hardness ==
             0)
    {
      npc->location.xpos += 1;
    }
    else if (dungeon[npc->location.ypos][npc->location.xpos + 1].hardness <
             86)
    {
      dungeon[npc->location.ypos][npc->location.xpos + 1].hardness = 0;
      dungeon[npc->location.ypos][npc->location.xpos + 1].terrain = '#';
      npc->location.xpos += 1;
    }
    else
    {
      dungeon[npc->location.ypos][npc->location.xpos + 1].hardness -= 85;
    }
    break;

  case 2:
    if (dungeon[npc->location.ypos + 1][npc->location.xpos].hardness == 255)
      npc->direction = rand() % 4;
    else if (dungeon[npc->location.ypos + 1][npc->location.xpos].hardness ==
             0)
    {
      npc->location.ypos += 1;
    }
    else if (dungeon[npc->location.ypos + 1][npc->location.xpos].hardness <
             86)
    {
      dungeon[npc->location.ypos + 1][npc->location.xpos].hardness = 0;
      dungeon[npc->location.ypos + 1][npc->location.xpos].terrain = '#';
      npc->location.ypos += 1;
    }
    else
    {
      dungeon[npc->location.ypos + 1][npc->location.xpos].hardness -= 85;
    }
    break;

  case 3:
    if (dungeon[npc->location.ypos][npc->location.xpos - 1].hardness == 255)
      npc->direction = rand() % 4;
    else if (dungeon[npc->location.ypos][npc->location.xpos - 1].hardness ==
             0)
    {
      npc->location.xpos -= 1;
    }
    else if (dungeon[npc->location.ypos][npc->location.xpos - 1].hardness <
             86)
    {
      dungeon[npc->location.ypos][npc->location.xpos - 1].hardness = 0;
      dungeon[npc->location.ypos][npc->location.xpos - 1].terrain = '#';
      npc->location.xpos -= 1;
    }
    else
    {
      dungeon[npc->location.ypos][npc->location.xpos + 1].hardness -= 85;
    }
    break;

  default:
    break;
  }

  if (character_map[npc->location.ypos][npc->location.xpos])
  {
    character_map[npc->location.ypos][npc->location.xpos]->lives--;

    if (has_characteristic(character_map[npc->location.ypos][npc->location.xpos]
                               ->characteristic,
                           PC))
    {
      npc->lives = 0;
    }
    else
    {
      character_map[npc->location.ypos][npc->location.xpos] = npc;
    }
  }
  else
  {
    character_map[npc->location.ypos][npc->location.xpos] = npc;
  }
}

static void npc_5(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                  character_t *character_map[][DUNGEON_X], character_t *pc)
{
  point_t p = npc->location;

  if ((can_see_PC(npc, pc)))
  {
    p = get_smallest_neighbor_tunneling(p, dungeon);
  }

  character_map[npc->location.ypos][npc->location.xpos] = NULL;

  if (dungeon[p.ypos][p.xpos].hardness == 0)
  {
    npc->location.ypos = p.ypos;
    npc->location.xpos = p.xpos;

    if (character_map[npc->location.ypos][npc->location.xpos])
    {
      character_map[npc->location.ypos][npc->location.xpos]->lives--;
      if (has_characteristic(
              character_map[npc->location.ypos][npc->location.xpos]
                  ->characteristic,
              PC))
      {
        npc->lives = 0;
      }
      else
      {
        character_map[npc->location.ypos][npc->location.xpos] = npc;
      }
    }
    else
      character_map[npc->location.ypos][npc->location.xpos] = npc;
  }
  else if (dungeon[p.ypos][p.xpos].hardness < 86)
  {
    npc->location.ypos = p.ypos;
    npc->location.xpos = p.xpos;

    dungeon[p.ypos][p.xpos].hardness = 0;
    dungeon[p.ypos][p.xpos].terrain = '#';

    character_map[npc->location.ypos][npc->location.xpos] = npc;
  }
  else
  {
    dungeon[p.ypos][p.xpos].hardness -= 85;
    character_map[npc->location.ypos][npc->location.xpos] = npc;
  }
}

static void npc_6(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                  character_t *character_map[][DUNGEON_X], character_t *pc)
{
  uint8_t x_direction = 0, y_direction = 0;

  if (npc->location.xpos != pc->location.xpos)
  {
    x_direction = (pc->location.xpos - npc->location.xpos) /
                  (uint8_t)abs(pc->location.xpos - npc->location.xpos);
  }

  if (npc->location.ypos != pc->location.ypos)
  {
    y_direction = (pc->location.ypos - npc->location.ypos) /
                  (uint8_t)abs(pc->location.ypos - npc->location.ypos);
  }

  uint8_t temp_x = npc->location.xpos + x_direction, temp_y = npc->location.ypos + y_direction;

  if (dungeon[temp_y][temp_x].hardness == 0)
  {
    character_map[npc->location.ypos][npc->location.xpos] = NULL;

    npc->location.xpos = temp_x;
    npc->location.ypos = temp_y;

    if (character_map[npc->location.ypos][npc->location.xpos])
    {
      character_map[npc->location.ypos][npc->location.xpos]->lives--;

      if (has_characteristic(
              character_map[npc->location.ypos][npc->location.xpos]
                  ->characteristic,
              PC))
      {
        npc->lives = 0;
      }
      else
      {
        character_map[npc->location.ypos][npc->location.xpos] = npc;
      }
    }
    else
      character_map[npc->location.ypos][npc->location.xpos] = npc;
  }
  else if (dungeon[temp_y][temp_x].hardness < 86)
  {
    character_map[npc->location.ypos][npc->location.xpos] = NULL;

    npc->location.ypos = temp_y;
    npc->location.xpos = temp_x;

    dungeon[npc->location.ypos][npc->location.xpos].hardness = 0;
    dungeon[npc->location.ypos][npc->location.xpos].terrain = '#';

    character_map[npc->location.ypos][npc->location.xpos] = npc;
  }
  else if (dungeon[temp_y][temp_x].hardness < 255)
  {
    dungeon[temp_y][temp_x].hardness -= 85;
  }
}

static void npc_7(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                  character_t *character_map[][DUNGEON_X], character_t *pc)
{
  point_t p;

  p = get_smallest_neighbor_non_tunneling(npc->location, dungeon);

  if (dungeon[p.ypos][p.xpos].hardness == 0)
  {
    character_map[npc->location.ypos][npc->location.xpos] = NULL;

    npc->location.ypos = p.ypos;
    npc->location.xpos = p.xpos;

    if (character_map[p.ypos][p.xpos])
    {
      character_map[p.ypos][p.xpos]->lives--;
      if (character_map[p.ypos][p.xpos] == pc)
      {
        npc->lives = 0;
      }
      else
      {
        character_map[npc->location.ypos][npc->location.xpos] = npc;
      }
    }
    else
    {
      character_map[npc->location.ypos][npc->location.xpos] = npc;
    }
  }
  else if (dungeon[p.ypos][p.xpos].hardness < 86)
  {
    character_map[npc->location.ypos][npc->location.xpos] = NULL;

    npc->location.ypos = p.ypos;
    npc->location.xpos = p.xpos;

    dungeon[p.ypos][p.xpos].hardness = 0;
    dungeon[p.ypos][p.xpos].terrain = '#';

    character_map[npc->location.ypos][npc->location.xpos] = npc;
  }
  else if (dungeon[p.ypos][p.xpos].hardness < 255)
  {
    dungeon[p.ypos][p.xpos].hardness -= 85;
  }
}

void place_monster(character_t *character, dungeon_space_t dungeon[][DUNGEON_X],
                   character_t *character_map[][DUNGEON_X], int num_spaces,
                   int num_mon)
{
  int k = 1, rand_x = 1, rand_y = 1, redos = 0;
  uint8_t i, j;

  for (i = 0; i < DUNGEON_Y && k <= num_mon; i += rand_y)
  {
    if (num_mon == num_spaces || redos >= 3000)
    {
      for (j = 1; j < DUNGEON_X - 1 && k <= num_mon; j++)
      {
        if (dungeon[i][j].terrain != ' ' && character_map[i][j] == NULL)
        {
          character->location.xpos = j;
          character->location.ypos = i;

          k++;
        }
      }
      rand_y = 1;
    }
    else
    {
      for (j = 0; j < DUNGEON_X && k <= num_mon; j += rand_x)
      {
        if (dungeon[i][j].hardness == 0 && character_map[i][j] == NULL)
        {
          character->location.xpos = j;
          character->location.ypos = i;

          rand_x = rand() % num_spaces / num_mon + 4;
          k++;
        }
        redos++;
      }
      rand_y = rand() % 3 + 1;
    }
  }
}

void pc_init(character_t *pc_char, point_t pc,
             character_t *character_map[DUNGEON_Y][DUNGEON_X],
             dungeon_space_t dungeon[][DUNGEON_X], heap_t *mh, int num_lives)
{
  pc_char->characteristic = 16;
  pc_char->speed = 10;
  pc_char->p = 10;
  pc_char->lives = num_lives;

  pc_char->location.xpos = pc.xpos;
  pc_char->location.ypos = pc.ypos;

  character_map[pc_char->location.ypos][pc_char->location.xpos] = pc_char;
  heap_insert(mh, pc_char);
}

void generate_monsters(point_t pc,
                       character_t *character_map[DUNGEON_Y][DUNGEON_X],
                       dungeon_space_t dungeon[][DUNGEON_X], int num_spaces,
                       int num_mon, heap_t *mh)
{
  int i;
  character_t *temp;

  num_mon = num_mon >= num_spaces ? num_spaces - 1 : num_mon;

  for (i = 0; i < num_mon; i++)
  {
    temp = (character_t *)malloc(sizeof(*temp));
    memset(temp, 0, sizeof(*temp));

    temp->characteristic = rand() & 0xf;
    temp->speed = rand() % 16 + 5;
    temp->p = 1000 / temp->speed;
    temp->direction = rand() % 4;
    temp->lives = 1;

    place_monster(temp, dungeon, character_map, num_spaces, num_mon);
    character_map[temp->location.ypos][temp->location.xpos] = temp;
    heap_insert(mh, temp);
  }
}

void npc_erratic(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                 character_t *character_map[][DUNGEON_X], character_t *pc);

void (*npc_move_func[])(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                        character_t *character_map[][DUNGEON_X],
                        character_t *pc) = {
    npc_0, npc_1, npc_2, npc_3,
    npc_4, npc_5, npc_6, npc_7,
    npc_erratic, npc_erratic, npc_erratic, npc_erratic,
    npc_erratic, npc_erratic, npc_erratic, npc_erratic};

void npc_erratic(character_t *npc, dungeon_space_t dungeon[][DUNGEON_X],
                 character_t *character_map[][DUNGEON_X], character_t *pc)
{
  int e = rand() % 2;
  if (e)
  {
    npc->direction = rand() % 4;
    npc_0(npc, dungeon, character_map, pc);
  }
  else
  {
    npc_move_func[npc->characteristic & 0x00000007](npc, dungeon, character_map,
                                                    pc);
  }
}

void move_character(dungeon_space_t dungeon[][DUNGEON_X],
                    character_t *character_map[][DUNGEON_X],
                    character_t *to_move, character_t *pc)
{
  npc_move_func[to_move->characteristic](to_move, dungeon, character_map, pc);
}

void reset_character_map(character_t *character_map[][DUNGEON_X], heap_t *mh) {}
