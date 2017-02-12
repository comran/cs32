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

// Returns whether or not sequence is empty.
bool Sequence::empty() const {
  if (size() > 0) return false;

  return true;
}

// Getter for the internal size variable.

// Insert in the given value at the given position. Returns whether or not this
// action was successful.
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
// new node. Returns the position of the newly inserted element.
int Sequence::insert(const ItemType &value) {
  // Special case for dealing with an empty start_node_ when Sequence is empty.
  if (size() == 0) {
    if(start_node_ != nullptr) delete start_node_;  // Avoid memory leak.

    // Create an unlinked node containing the given value.
    Node *new_node = new Node();
    new_node->value = value;
    new_node->next = nullptr;
    new_node->previous = nullptr;

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

  return i;
}

// Erase one element in sequence at the position given. Returns whether
// this action was successful or not.
bool Sequence::erase(int pos) {
  if (pos < 0 || pos >= size()) return false;  // Out of bounds.

  Node *current_node = start_node_;

  // Special case for erasing the start node.
  if (pos == 0) {
    if (current_node->next != nullptr) {
      // Replace start_node_ with the element at index 1 in the sequence, if
      // such an element exists.
      current_node->next->previous = nullptr;
      Node *new_start_node = current_node->next;
      delete current_node;
      start_node_ = new_start_node;
    }
  } else {
    // Generate a pointer to the element being deleted.
    for (int i = 0; i < pos; i++) {
      current_node = current_node->next;
    }

    // Link the nodes before and after the element to each other.
    if (current_node->previous != nullptr)
      current_node->previous->next = current_node->next;
    if (current_node->next != nullptr)
      current_node->next->previous = current_node->previous;

    // Finally delete the node at pos, which is a dangling pointer at this
    // point.
    delete current_node;
  }

  size_--;  // Update size to account for the deleted node.
  return true;
}

// Remove the first element that equals value, and return the number of deleted
// nodes. If no node was deleted, return 0.
int Sequence::remove(const ItemType &value) {
  int num_items_removed(0);

  // Go through the sequence, comparing the value of each node to the value
  // given in the parameter.
  Node *current_node = start_node_;
  for (int i = 0; i < size(); i++) {
    if (current_node->value == value) {
      // Scroll to the next node before deleting the note that has a matching
      // value.
      current_node = current_node->next;

      erase(i);             // Erase the matching node.
      i--;                  // Account for the change in size.
      num_items_removed++;  // Increment our count of removed numbers.
    } else {
      current_node = current_node->next;
    }
  }

  return num_items_removed;
}

// Store the value at the index "pos" in the value parameter, and return whether
// this action was successful or not.
bool Sequence::get(int pos, ItemType &value) const {
  if (pos < 0 || pos >= size()) return false;  // Out of bounds

  // Get a pointer to the item at index pos.
  Node *current_node = start_node_;
  for (int i = 0; i < pos; i++) {
    current_node = current_node->next;
  }

  // Set value to the value of the item at index pos.
  value = current_node->value;

  return true;
}

// Set the node at pos to the value given, and return whether this action was
// successful or not.
bool Sequence::set(int pos, const ItemType &value) {
  if (pos < 0 || pos >= size()) return false;  // Out of bounds

  // Get a pointer to the item at index pos.
  Node *current_node = start_node_;
  for (int i = 0; i < pos; i++) {
    current_node = current_node->next;
  }

  // Set value of the item at index pos to the parameter value.
  current_node->value = value;

  return true;
}

// Find the first instance of value in the Sequence, and return the index of
// this first value. (Return -1 if no match is found)
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

// Swap the elements in this sequence with those in the Sequence given as a
// parameter.
void Sequence::swap(Sequence &other) {
  // Swap the sizes of both Sequences.
  int original_size = size();
  size_ = other.size();
  other.size_ = original_size;

  // Swap the linked list of values of both Sequences.
  Node *original_start_node = start_node_;
  start_node_ = other.start_node_;
  other.start_node_ = original_start_node;
}

// See if there is a contigious sequence of query_sequence within
// searched_sequence, and
// return the index of the start of this sequence if it exists. If such a
// sequence does not exist, return -1.
int subsequence(const Sequence &searched_sequence,
                const Sequence &query_sequence) {
  // Return a failed search if we have nothing in the query sequence, or if
  // there are more elements in the query sequence than in the sequence that is
  // being searched for that sequence.
  if (query_sequence.empty() ||
      query_sequence.size() > searched_sequence.size())
    return -1;

  // Search for an instance of the first element of query sequence.
  for (int i = 0; i < searched_sequence.size(); i++) {
    ItemType value_1, value_2;
    searched_sequence.get(i, value_1);
    query_sequence.get(0, value_2);
    if (value_1 != value_2) continue;  // Mismatch, so try the next index.

    // See if the following elements also match the respective elements from
    // query_sequence.
    bool sequence_found = true;
    for (int j = 1; j < query_sequence.size() && sequence_found; j++) {
      if (!searched_sequence.get(i + j, value_1) ||
          !query_sequence.get(j, value_2) || value_2 != value_1) {
        // Failed match detected, so abort this sub-search.
        sequence_found = false;
      }
    }

    // Search succeeded, so return the first element where query_sequence was
    // found within searched_sequence.
    if (sequence_found) return i;  // i is the start point of the sequence.
  }

  return -1;  // Failed search, so return a failure code.
}

// Weave seq1 and seq2 into each other such that the first element of seq1 is
// followed by the first element of seq2, and that element is followed by the
// second element of seq1 (if such an element exists), which is followed by the
// second element of seq2 (if such an element exists), and so on. Put the
// result in the Sequence parameter given.
void interleave(const Sequence &seq1, const Sequence &seq2, Sequence &result) {
  Sequence result_temp(result);  // To avoid aliasing.

  // Clean the result Sequence.
  while (!result_temp.empty()) result_temp.erase(0);

  // Check to see if there is nothing to interweave, and return the proper
  // result.
  if (seq1.empty()) {
    result_temp = seq2;
  } else if (seq2.empty()) {
    result_temp = seq1;
  }

  // Interweave seq1 and seq2.
  int i(0), j(0);
  while (i < seq1.size() || j < seq2.size()) {
    ItemType value;
    // Add an element from seq1 (if such an element exists).
    if (i < seq1.size()) {
      seq1.get(i, value);
      result_temp.insert(i + j, value);
      i++;
    }

    // Add an element from seq2 (if such an element exists).
    if (j < seq2.size()) {
      seq2.get(j, value);
      result_temp.insert(i + j, value);
      j++;
    }
  }

  result = result_temp;
}
