#include "character_utils.hpp"
#include "dungeon.hpp"
#include "heap.hpp"
#include "parser.hpp"
#include "path.hpp"

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

int main(void) {
  srand(time(NULL));

  character *pc = new character;
  dungeon *d = new dungeon(pc, 3, 10, save);
  std::vector<character> mv;
  std::vector<item> iv;
  heap_t mh;

  parse(&mv, &iv);

  generate_monsters(d, &mh, &mv);

  delete pc;
  delete d;

  return 0;
}
