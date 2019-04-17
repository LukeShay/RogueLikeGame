#include "character.hpp"
#include <iostream>

using namespace std;


int main(void) {
  character c;


  for (int i = 0; i < 12; i++) {
    if (c.equiped[i]) {
      cout << i << endl;
    }

  }


  return 0;
}
