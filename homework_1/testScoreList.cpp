#include "ScoreList.h"

#include <iostream>
#include <cassert>

typedef unsigned long ItemType;

int main(int, char **) {
  ScoreList score_list;
  assert(score_list.size() == 0);

  int min = 100;
  int max = 0;
  for (int i = 0; i <= DEFAULT_MAX_ITEMS + 5; i++) {
    int num = 100 - i / 4;
    if (i >= DEFAULT_MAX_ITEMS) {
      assert(!score_list.add(num));
    } else {
      assert(score_list.add(num));
      if (num > max) max = num;
      if (num < min) min = num;
    }
  }

  assert(!score_list.add(-1));
  assert(!score_list.add(101));

  assert(score_list.size() == DEFAULT_MAX_ITEMS);

  assert(score_list.minimum() == min);

  assert(score_list.maximum() == max);

  ::std::cout << "Passed all tests" << ::std::endl;
}
