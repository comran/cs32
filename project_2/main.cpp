#include "Sequence.h"
#include <string>
#include "Sequence.h"

#include <iostream>
#include <cassert>
using namespace std;

void test() {
  Sequence *s = new Sequence();
  {
    // Make sure Sequence is empty when given no copy constructor and nothing to
    // insert.
    assert(s->size() == 0 && s->empty());
  }

  {
    // Test that insert works when adding multiple items.
    s->insert("TEST1");
    assert(s->size() == 1 && !s->empty());
    s->insert("TEST3");
    s->insert("TEST2");
    s->insert("TEST0");
    s->insert("TEST0");
    s->insert("TEST0");
    s->insert("TEST0");
    s->insert("TEST0");
    assert(s->size() == 8);
  }

  {
    // Test that get() works and that the one-parameter insert instance inserted
    // according to alphabetical order.
    int i;
    ItemType item, last_item;
    for (i = 0; s->get(i, item); i++) {
      if (i > 0) assert(item > last_item || item == last_item);
      last_item = item;
    }
  }

  {
    // Test that find works.
    ItemType item_at_index;
    assert(s->get(s->find("TEST2"), item_at_index) && item_at_index == "TEST2");
  }

  {
    // Test that remove works and that it caught all instances.
    assert(s->remove("TEST0") == 5 && s->find("TEST0") == -1);

    ItemType item;
    for (int i = 0; s->get(i, item); i++) {
      assert(item != "TEST0");
    }
  }

  {
    // Test that insert only works within the bounds of the Sequence.
    assert(!s->insert(s->size() + 1, "TEST4") && s->find("TEST4") == -1);
    assert(!s->insert(-1, "TEST4") && s->find("TEST4") == -1);
    assert(s->insert(s->size(), "TEST4") && s->find("TEST4") == s->size() - 1);
  }

  {
    // Test that erase only works within the bounds of Sequence.
    assert(!s->erase(s->size()) && s->find("TEST4") == s->size() - 1);
    assert(!s->erase(-1) && s->find("TEST4") == s->size() - 1);
    assert(s->erase(s->size() - 1) && s->find("TEST4") == -1);
  }

  {
    // Test that set only works within the bounds of Sequence and doesn't add
    // a new item.
    assert(!s->set(s->size(), "TEST4") && s->find("TEST4") == -1);
    assert(!s->set(-1, "TEST4") && s->find("TEST4") == -1);
    assert(s->set(s->size() - 1, "TEST4") && s->find("TEST4") == s->size() - 1);
  }

  {
    // Test that swap works.
    Sequence *g = new Sequence();
    g->insert(0, "TEST7");
    g->insert(1, "TEST8");

    ItemType item;
    assert(s->size() == 3 && s->get(0, item) && item == "TEST1" &&
           s->get(1, item) && item == "TEST2" && s->get(2, item) &&
           item == "TEST4");
    assert(g->size() == 2 && g->get(0, item) && item == "TEST7" &&
           g->get(1, item) && item == "TEST8");

    s->swap(*g);

    assert(g->size() == 3 && g->get(0, item) && item == "TEST1" &&
           g->get(1, item) && item == "TEST2" && g->get(2, item) &&
           item == "TEST4");
    assert(s->size() == 2 && s->get(0, item) && item == "TEST7" &&
           s->get(1, item) && item == "TEST8");

    // Test that swap still works on empty sequences.
    while (s->size() > 0) s->erase(0);
    assert(s->size() == 0);
    assert(g->size() == 3);
    s->swap(*g);
    assert(s->size() == 3);
    assert(g->size() == 0);

    delete g;
  }

  assert(s->set(s->size() - 1, "TEST3"));  // Restore some order to s first...
  assert(s->insert("TEST4") == s->size() - 1);
  assert(s->insert("TEST5") == s->size() - 1);

  {
    // Test if subsequence fails to find a sequence that does not exist in the
    // searched sequence.
    Sequence *g = new Sequence();
    g->insert(0, "TEST2");
    g->insert(1, "TEST4");
    g->insert(2, "TEST3");

    assert(subsequence(*s, *g) == -1);

    *g = *s;  // Test copy opeeration.

    // Test that a subsequence of s can be found.

    // Erase part of g so it turns into a subsequence of s.
    for (int i = 0; i < 3; i++) g->erase(0);
    assert(subsequence(*s, *g) == 3);

    delete g;
  }

  {
    // Test that interleave produces a properly interweaved Sequence.
    Sequence *g = new Sequence(), *h = new Sequence();
    g->insert(0, "TEST5");
    g->insert(1, "TEST6");
    g->insert(2, "TEST7");

    interleave(*s, *g, *h);

    ItemType item;
    assert(h->size() == s->size() + g->size() && h->get(0, item) &&
           item == "TEST1" && h->get(1, item) && item == "TEST5" &&
           h->get(2, item) && item == "TEST2" && h->get(3, item) &&
           item == "TEST6" && h->get(4, item) && item == "TEST3" &&
           h->get(5, item) && item == "TEST7" && h->get(6, item) &&
           item == "TEST4" && h->get(7, item) &&
           item == "TEST5");

    delete g;
    delete h;
  }

  {
    // Test that interleave properly deals with aliasing.
    // The following should not result in an infinite loop, and should instead
    // simply clear s.
    int s_size = s->size();
    interleave(*s, *s, *s);
    assert(s->size() == s_size * 2);
  }

  while(s->size() > 0) s->erase(0);

  s->insert("TEST1");
  s->insert("TEST2");
  s->insert("TEST3");
  s->insert("TEST4");
  s->insert("TEST5");

  {
    // Test that the copy constructor works.
    Sequence *g = new Sequence(*s);
    ItemType item, item2;
    for (int i = 0; s->get(i, item) && g->get(i, item2); i++) {
      assert(item == item2);
    }

    delete g;
  }

  delete s;
}

int main() {
  test();
  cout << "Passed all tests" << endl;
}
