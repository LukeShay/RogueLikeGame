#include "path.hpp"
#include <climits>
#include <cstdlib>

void get_neighbors(uint8_t x, uint8_t y, neighbor_t arr[]) {
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

void non_tunneling_path(dungeon *d, point_t pc) {
  uint8_t i, j;

  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      d->cost_nt_map[i][j] = INT_MAX;
    }
  }

  d->cost_nt_map[pc.ypos][pc.xpos] = 0;
  uint8_t x, y;
  int c;
  neighbor_t neighbors[8];

  queue_t pq;
  queue_init(&pq);
  queue_push(&pq, pc.xpos, pc.ypos, 0);
  while (queue_size(&pq)) {
    queue_pop(&pq, &x, &y, &c);
    get_neighbors(x, y, neighbors);

    for (i = 0; i < 8; i++) {
      if (d->hardness_map[neighbors[i].y][neighbors[i].x] == 0 &&
          c + 1 < d->cost_nt_map[neighbors[i].y][neighbors[i].x]) {
        d->cost_nt_map[neighbors[i].y][neighbors[i].x] = c + 1;
        queue_push(&pq, neighbors[i].x, neighbors[i].y, c + 1);
      }
    }
  }

  queue_delete(&pq);
}

void tunneling_path(dungeon *d, point_t pc) {
  uint8_t i, j;
  uint8_t x, y;
  int c;
  int temp_c;
  neighbor_t neighbors[8];
  queue_t pq;

  for (i = 0; i < DUNGEON_Y; i++) {
    for (j = 0; j < DUNGEON_X; j++) {
      d->cost_t_map[i][j] = INT_MAX;
    }
  }

  d->cost_t_map[pc.ypos][pc.xpos] = 0;

  queue_init(&pq);
  queue_push(&pq, pc.xpos, pc.ypos, 0);

  while (queue_size(&pq) != 0) {
    queue_pop(&pq, &x, &y, &c);
    get_neighbors(x, y, neighbors);

    for (i = 0; i < 8; i++) {
      temp_c = c + (d->hardness_map[y][x] / 85) + 1;

      if (neighbors[i].x != 0 && neighbors[i].x != DUNGEON_X &&
          neighbors[i].y != 0 && neighbors[i].y != DUNGEON_Y &&
          temp_c < d->cost_t_map[neighbors[i].y][neighbors[i].x] &&
          d->hardness_map[neighbors[i].y][neighbors[i].x] != 255) {
        d->cost_t_map[neighbors[i].y][neighbors[i].x] = temp_c;
        queue_push(&pq, neighbors[i].x, neighbors[i].y, temp_c);
      }
    }
  }
  queue_delete(&pq);
}
