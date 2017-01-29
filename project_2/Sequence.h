#ifndef HOMEWORK_1_SEQUENCE
#define HOMEWORK_1_SEQUENCE

#include <iostream>
#include <string>

typedef ::std::string ItemType;

struct Node {
  ItemType value;
  struct Node *next;
  struct Node *previous;
};

class Sequence {
 public:
  Sequence();
  Sequence(const Sequence &other);
  ~Sequence();
  Sequence &operator=(const Sequence &rhs);
  bool empty() const;
  int size() const;
  bool insert(int pos, const ItemType &value);
  int insert(const ItemType &value);
  bool erase(int pos);
  int remove(const ItemType &value);
  bool get(int pos, ItemType &value) const;
  bool set(int pos, const ItemType &value);
  int find(const ItemType &value) const;
  void swap(Sequence &other);
  int subsequence(const Sequence& sequence_1, const Sequence& sequence_2);

  //////////////////////////////////////////////////////////////////////////////
  void dump();  // TODO(comran): REMOVE
                // THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //////////////////////////////////////////////////////////////////////////////

 private:
  int size_;
  Node *start_node_;
};

#endif  // HOMEWORK_1_SEQUENCE
