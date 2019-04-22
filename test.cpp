#include "character.hpp"
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

class compare_characters {
public:
  bool operator()(const character &key, const character &with) const {
    return key.p > with.p;
  }
};

int main(void) {
  priority_queue<character, vector<character>, compare_characters> pq;
  character c;
  int i;

  for (i = 0; i < 10; i++) {
    c.p = i;
    pq.push(c);
  }

  for (i = 0; i < 10; i++) {
    cout << pq.top().p << endl;
    pq.pop();
  }

  return 0;
}
