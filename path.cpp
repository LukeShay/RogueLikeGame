#include "path.h"
#include <climits>
#include <cstdlib>

int32_t neighbor_cmp(const void *key, const void *with) {
  return ((neighbor_t *)key)->c - ((neighbor_t *)with)->c;
}

void get_neighbors(uint8_t x, uint8_t y,
                   dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                   neighbor_t arr[]) {
  uint8_t i, j, k = 0;

  for (i = y - 1; i <= y + 1; i++) {
    for (j = x - 1; j <= x + 1; j++) {
      if (!(i == y && j == x)) {
        arr[k].x = j;
        arr[k].y = i;
        k++;
      }
    }
  }
}

void non_tunneling_path(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X],
                        point_t pc) {
  uint8_t i, j;

  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      dungeon[i][j].cost_nt = INT_MAX;
    }
  }

  dungeon[pc.ypos][pc.xpos].cost_nt = 0;
  uint8_t x, y;
  int c;
  neighbor_t neighbors[8];

  // neighbor_t *n = (neighbor_t *)malloc(sizeof(*n));
  // n->x = pc.xpos;
  // n->y = pc.ypos;
  // n->c = 0;

  queue_t pq; //heap_t ph;
  queue_init(&pq); //heap_init(&ph, neighbor_cmp, NULL);
  queue_push(&pq, pc.xpos, pc.ypos, 0); //heap_insert(&ph, n);

  while (queue_size(&pq)) {
    queue_pop(&pq, &x, &y, &c);
    get_neighbors(x, y, dungeon, neighbors);

    for (i = 0; i < 8; i++) {
      if (dungeon[neighbors[i].y][neighbors[i].x].hardness == 0 &&
          c + 1 < dungeon[neighbors[i].y][neighbors[i].x].cost_nt) {
        dungeon[neighbors[i].y][neighbors[i].x].cost_nt = c + 1;
        queue_push(&pq, neighbors[i].x, neighbors[i].y, c + 1);
      }
    }
  }
  queue_delete(&pq);
}

void tunneling_path(dungeon_space_t dungeon[DUNGEON_Y][DUNGEON_X], point_t pc) {
  uint8_t i, j;
  uint8_t x, y;
  int c;
  int temp_c;
  neighbor_t neighbors[8];
  queue_t pq;

  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      dungeon[i][j].cost_t = INT_MAX;
    }
  }

  dungeon[pc.ypos][pc.xpos].cost_t = 0;

  queue_init(&pq);
  queue_push(&pq, pc.xpos, pc.ypos, 0);

  while (queue_size(&pq) != 0) {
    queue_pop(&pq, &x, &y, &c);
    get_neighbors(x, y, dungeon, neighbors);

    for (i = 0; i < 8; i++) {
      temp_c = c + (dungeon[y][x].hardness / 85) + 1;

      if (neighbors[i].x != 0 && neighbors[i].x != DUNGEON_X &&
          neighbors[i].y != 0 && neighbors[i].y != DUNGEON_Y &&
          temp_c < dungeon[neighbors[i].y][neighbors[i].x].cost_t &&
          dungeon[neighbors[i].y][neighbors[i].x].hardness != 255) {
        dungeon[neighbors[i].y][neighbors[i].x].cost_t = temp_c;
        queue_push(&pq, neighbors[i].x, neighbors[i].y, temp_c);
      }
    }
  }
  queue_delete(&pq);
}
