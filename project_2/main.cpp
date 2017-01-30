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

  ::std::cout << "HERE1\n";
  Sequence *g = new Sequence(*s);
  g->dump();
  for(int i = 2;i < g->size();i++) {
    g->erase(i);
    i--;
    ::std::cout << "Erasing " << i << "\n";
  }
  ::std::cout << "Dump of S...\n";
  s->dump();
  ::std::cout << "Dump of G...\n";
  g->dump();

  ::std::cout << "Sequence found @ " << g->subsequence(*s, *g) << ::std::endl;

  delete s;
  delete g;
}

int main() {
  test();
  cout << "Passed all tests" << endl;
}
