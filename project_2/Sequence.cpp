#include "Sequence.h"
#include <iostream>
#include <cassert>

Sequence::Sequence() : size_(0) { start_node_ = nullptr; }

Sequence::~Sequence() {
  struct Node *current_node = start_node_;

  while (current_node != nullptr) {
    Node *next_node = current_node->next;
    delete current_node;
    current_node = next_node;
  }
}

Sequence &Sequence::operator=(const Sequence &rhs) {
  if (this != &rhs) {
    Sequence other(rhs);
    swap(other);
  }

  return *this;
}

Sequence::Sequence(const Sequence &other) {
  start_node_ = new Node();
  start_node_->previous = nullptr;
  start_node_->next = nullptr;

  for (int i = 0; i < other.size(); i++) {
    ItemType value;

    other.get(i, value);
    insert(i, value);
  }
}

bool Sequence::empty() const {
  if (size_ > 0) return false;

  return true;
}

int Sequence::size() const { return size_; }

bool Sequence::insert(int pos, const ItemType &value) {
  if (pos < 0 || pos > size()) return false;  // Out of bounds.

  Node *new_node = new Node();
  new_node->value = value;
  new_node->next = nullptr;
  new_node->previous = nullptr;

  size_++;

  if (pos == 0) {
    if (start_node_ != nullptr) {
      new_node->next = start_node_;
      start_node_->previous = new_node;
    }

    start_node_ = new_node;

    return true;
  }

  Node *node_before = start_node_;
  for (int i = 0; i < pos - 1; i++) {
    node_before = node_before->next;
  }

  Node *node_after = node_before->next;
  node_before->next = new_node;
  if (node_after != nullptr) node_after->previous = new_node;
  new_node->previous = node_before;
  new_node->next = node_after;

  return true;
}

int Sequence::insert(const ItemType &value) {
  Node *new_node = new Node();
  new_node->value = value;
  new_node->next = nullptr;
  new_node->previous = nullptr;

  if (size() == 0) {
    size_++;
    start_node_ = new_node;
    return 0;
  }

  Node *current_node = start_node_;
  int i;
  for (i = 0; i < size() - 0; i++) {
    if (current_node->value > value) break;
    current_node = current_node->next;
  }

  insert(i, value);

  return true;
}

bool Sequence::erase(int pos) {
  if (pos < 0 || pos >= size()) return false;

  Node *current_node = start_node_;

  if (pos == 0) {
    if (current_node->next != nullptr) {
      current_node->next->previous = nullptr;
      Node *new_start_node = current_node->next;
      delete current_node;
      start_node_ = new_start_node;
    }
  } else {
    for (int i = 0; i < pos; i++) {
      current_node = current_node->next;
    }

    if (current_node->previous != nullptr)
      current_node->previous->next = current_node->next;
    if (current_node->next != nullptr)
      current_node->next->previous = current_node->previous;

    delete current_node;
  }

  size_--;
  return true;
}

int Sequence::remove(const ItemType &value) {
  int num_items_removed = 0;

  Node *current_node = start_node_;
  for (int i = 0;i < size(); i++) {
    if (current_node->value == value) {
      current_node = current_node->next;

      erase(i);
      i--;
      num_items_removed++;
    } else {
      current_node = current_node->next;
    }
  }
  return num_items_removed;
}

bool Sequence::get(int pos, ItemType &value) const {
  if (pos < 0 || pos >= size()) return false;

  Node *current_node = start_node_;
  for(int i = 0;i < pos;i++) {
    current_node = current_node->next;
  }

  value = current_node->value;

  return true;
}

bool Sequence::set(int pos, const ItemType &value) {
  if (pos < 0 || pos >= size()) return false;

  Node *current_node = start_node_;
  for(int i = 0;i < pos;i++) {
    current_node = current_node->next;
  }

  current_node->value = value;

  return true;
}

int Sequence::find(const ItemType &value) const {
  int p = -1;  // Default p value from spec.

  if (size() <= 0) return p;

  Node *current_node = start_node_;
  for (int i = 0; i < size(); i++) {
    if (current_node->value == value) {
      p = i;
      return p;
    }

    current_node = current_node->next;
  }

  return -1;
}

void Sequence::swap(Sequence &other) {
  int original_size = size();
  size_ = other.size();
  other.size_ = original_size;

  Node *original_start_node = start_node_;
  start_node_ = other.start_node_;
  other.start_node_ = original_start_node;
}

int Sequence::subsequence(const Sequence& sequence_1, const Sequence& sequence_2) {
  if(sequence_2.empty() || sequence_2.size() > sequence_1.size()) return -1;

  for(int i = 0;i < sequence_1.size();i++) {
    ItemType value_1, value_2;
    sequence_1.get(i, value_1);
    sequence_2.get(i, value_2);
    if(value_1 != value_2) continue;

    bool sequence_found = false;
    for(int j = 0;j < sequence_2.size() && sequence_found;j++) {
      sequence_found = true;
      sequence_1.get(i, value_1);
      sequence_2.get(i, value_2);

      if(value_2 != value_1) sequence_found = false;
    }


    if(sequence_found) return i;  // i is the start point of the sequence.
  }

  return -1;
}

void Sequence::dump() {
  Node *current = start_node_;
  for (int i = 0; current != nullptr; i++) {
    ::std::cout << i << ": " << current->value << ::std::endl;
    current = current->next;
  }
  ::std::cout << ::std::endl;
}
