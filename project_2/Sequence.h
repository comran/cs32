#ifndef PROJECT_2_SEQUENCE
#define PROJECT_2_SEQUENCE

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
  inline int size() const { return size_; }
  bool insert(int pos, const ItemType &value);
  int insert(const ItemType &value);
  bool erase(int pos);
  int remove(const ItemType &value);
  bool get(int pos, ItemType &value) const;
  bool set(int pos, const ItemType &value);
  int find(const ItemType &value) const;
  void swap(Sequence &other);

 private:
  int size_;
  Node *start_node_;
};

int subsequence(const Sequence &searched_sequence,
                const Sequence &query_sequence);
void interleave(const Sequence &seq1, const Sequence &seq2, Sequence &result);

#endif  // PROJECT_2_SEQUENCE
