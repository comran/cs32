#include "Sequence.h"
#include <string>
#include "Sequence.h"

#include <iostream>
#include <cassert>
using namespace std;

void test() {
  Sequence *s = new Sequence();
  s->insert("TEST1");
  s->dump();
  s->insert("TEST3");
  s->dump();
  s->insert("TEST2");
  s->dump();
  s->insert("TEST0");
  s->insert("TEST0");
  s->insert("TEST0");
  s->insert("TEST0");
  s->insert("TEST0");
  s->dump();
  for(int i = 0;i < s->size();i++) {
    ItemType test;
    ::std::cout << "@ " << i << " FOUND: " << test << ::std::endl;
  }
  s->dump();
  s->set(0, "BLABLA");
  s->dump();
  ::std::cout << "Find..." << s->find("TEST3") << ::std::endl;
  ::std::cout << "REMOVING " << s->remove("TEST0") << " NODES.\n";
  s->dump();

  Sequence *g = new Sequence(*s);
  g->dump();
  for(int i = 4;i < g->size();i++) {
    ::std::cout << "HERE\n";
    g->erase(i);
  }

  ::std::cout << "Sequence found @ " << g->subsequence(*s, *g) << ::std::endl;

  delete s;
}

int main() {
  test();
  cout << "Passed all tests" << endl;
}
