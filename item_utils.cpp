#include "item_utils.hpp"
#include <cstdlib>
#include <vector>

#define MIN_NUM_ITEMS 10

void generate_items(dungeon *d, std::vector<item_desc> *iv) {
  int rand_num, num_items = 0, min_items = rand() % 3 + MIN_NUM_ITEMS;
  item *i;
  std::vector<item_desc>::iterator it;

  for (it = iv->begin(); it != iv->end(); it++) {
    it->placed = 0;
  }

not_enough_items:
  for (it = iv->begin(); it != iv->end(); it++) {
    rand_num = rand() % 100;
    if (rand_num < it->rarity && it->placed != 1 && it->destroyed != 1) {
      i = new item;

      i->name = it->name;
      i->color = it->color;
      i->desc = it->desc;
      i->type = it->type;
      i->hit_bonus = it->hit_bonus.roll_dice();
      i->damage_bonus = it->damage_bonus.roll_dice();
      i->dodge_bonus = it->dodge_bonus.roll_dice();
      i->defense_bonus = it->defense_bonus.roll_dice();
      i->weight = it->weight.roll_dice();
      i->speed_bonus = it->speed_bonus.roll_dice();
      i->special_attr = it->special_attr.roll_dice();
      i->value = it->value.roll_dice();

      if (!it->artifact_status.compare("true")) {
        it->placed = 1;
      }

      i->pick_location(d->hardness_map, d->item_map);

      num_items++;
    }
    if (num_items > min_items + 5) {
      break;
    }
  }

  if (num_items < min_items) {
    goto not_enough_items;
  }
}
