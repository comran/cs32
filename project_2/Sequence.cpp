#include "Sequence.h"
#include <iostream>
#include <cassert>

// Initialize an empty Sequence.
Sequence::Sequence() : size_(0), start_node_(nullptr) {}

// Clear out the linked list of nodes, if any still exist.
Sequence::~Sequence() {
  // Start at the first node.
  struct Node *current_node = start_node_;

  // Go down the tree, caching the next node and deleting the current node until
  // there are no more nodes in the tree (leading to a nullptr for the next
  // node).
  while (current_node != nullptr) {
    Node *next_node = current_node->next;
    delete current_node;
    current_node = next_node;
  }
}

// Populate Sequence with the contents of the right-hand-side Sequence
// parameter.
Sequence &Sequence::operator=(const Sequence &rhs) {
  if (this != &rhs) {
    // Copy the right-hand-size sequence to another temporary Sequence object,
    // and then swap the values in the temporary object with the values of this
    // one.
    Sequence temporary_rhs(rhs);
    swap(temporary_rhs);
  }

  // Return a reference to this object containing the copied values.
  return *this;
}

// Initialize a Sequence, copying all elements from the other Sequence into this
// Sequence.
Sequence::Sequence(const Sequence &other) : size_(0), start_node_(nullptr) {
  // Insert all values of the other Sequence into this Sequence, which will
  // also copy over the size of the other Sequence.
  for (int i = 0; i < other.size(); i++) {
    ItemType value;
    other.get(i, value);

    insert(i, value);
  }
}

// Check to see if Sequence is empty by checking for a non-zero positive size.
bool Sequence::empty() const {
  if (size() > 0) return false;

  return true;
}

// Getter for the internal size variable.
int Sequence::size() const { return size_; }

// Insert in the given value at the given position.
bool Sequence::insert(int pos, const ItemType &value) {
  if (pos < 0 || pos > size()) return false;  // Out of bounds.

  // The rest of this method does not look at size, so increment it here.
  size_++;

  // Create an unlinked node containing the given value.
  Node *new_node = new Node();
  new_node->value = value;
  new_node->next = nullptr;
  new_node->previous = nullptr;

  // Special case for a node that will replace the start_node_.
  if (pos == 0) {
    if (start_node_ != nullptr) {
      // Make this replacement for the start_node_ link to the one that already
      // exists.
      new_node->next = start_node_;
      start_node_->previous = new_node;
    }

    start_node_ = new_node;

    return true;
  }

  // Generate a pointer to the node before the place where we want to insert a
  // new node.
  Node *node_before = start_node_;
  for (int i = 0; i < pos - 1; i++) {
    node_before = node_before->next;
  }

  // Cache the node that will come after the node that we are inserting.
  Node *node_after = node_before->next;

  // Link the new node to the nodes before and after. If the node after the new
  // node does not exist, do not link to a random piece of memory.
  node_before->next = new_node;
  if (node_after != nullptr) node_after->previous = new_node;
  new_node->previous = node_before;
  new_node->next = node_after;

  return true;
}

// Insert a new node, such that items before new node < new node < items after
// new node.
int Sequence::insert(const ItemType &value) {
  // Create an unlinked node containing the given value.
  Node *new_node = new Node();
  new_node->value = value;
  new_node->next = nullptr;
  new_node->previous = nullptr;

  // Special case for dealing with an empty start_node_ when Sequence is empty.
  if (size() == 0) {
    size_++;
    start_node_ = new_node;
    return 0;
  }

  // Generate a pointer to the node before the place where we want to insert a
  // new node.
  Node *current_node = start_node_;
  int i;
  for (i = 0; i < size(); i++) {
    if (current_node->value > value) break;
    current_node = current_node->next;
  }

  // Use insert to put the new value in its spot to maintain the order of
  // Sequence.
  insert(i, value);

  return true;
}

bool Sequence::erase(int pos) {
  if (pos < 0 || pos >= size()) return false;  // Out of bounds.

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
  for (int i = 0; i < size(); i++) {
    ItemType value;
    if(!get(i, value)) break;
    ::std::cout << i << ": " << value << ::std::endl;
  }
  ::std::cout << ::std::endl;
}
