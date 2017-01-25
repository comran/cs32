#include "Sequence.h"
#include <iostream>
#include <string>
#include <cassert>

int main() {
  {
    Sequence s;
    s.insert(0, 1);
    s.insert(1, 2);
    s.insert(2, 2);
    s.insert(3, 4);
    s.insert(4, 5);
    assert(s.remove(2) == 2);
    assert(s.size() == 3);
    unsigned long x;
    assert(s.get(0, x) && x == 1);
    assert(s.get(1, x) && x == 4);
    assert(s.get(2, x) && x == 5);
    assert(!s.get(3, x) && x == 5);

    assert(s.remove(1));
    assert(s.size() == 2);
    assert(!s.remove(1));
    assert(s.size() == 2);
    assert(s.remove(4));
    assert(s.size() == 1);
    assert(s.remove(5));
    assert(s.size() == 0);
    assert(!s.remove(5));
    assert(s.size() == 0);
    assert(s.insert(1) == 0);
    assert(s.size() == 1);
  }

  {
    Sequence z;
    assert(z.insert(z.size(), 1));
    assert(z.insert(z.size(), 2));
    assert(!z.insert(z.size() * 2, 3));
    assert(!z.erase(z.size() * 2));
    assert(z.find(1) == 0);
    assert(z.find(2) == 1);
    assert(z.find(3) == -1);
    assert(z.erase(z.size() - 1));
    assert(z.size() == 1);

    unsigned long test1 = 4;
    assert(z.set(0, test1));
    assert(z.find(4) == 0);

    unsigned long test2 = 5;
    assert(z.get(0, test2) && test2 == 4);
  }

  Sequence x, y;

  assert(x.empty());
  assert(y.empty());

  for (int i = 0; i <= DEFAULT_MAX_ITEMS * 2; i++) {
    if (i >= DEFAULT_MAX_ITEMS) {
      assert(x.insert(1) == -1);
    } else if (i >= 50) {
      assert(x.insert(2) == 50);
    } else {
      assert(x.insert(1) == 0);
    }
  }

  assert(x.size() == DEFAULT_MAX_ITEMS);  // Make sure the index was capped.

  x.swap(y);

  // Make sure the array size is still capped.
  assert(y.size() == DEFAULT_MAX_ITEMS);

  assert(y.set(10, 1000001));
  assert(y.set(11, 1000001));
  assert(y.set(91, 1000001));
  assert(y.remove(1000001) == 3);
  assert(y.size() == DEFAULT_MAX_ITEMS - 3);

  ::std::cout << "Passed all tests" << ::std::endl;
}
