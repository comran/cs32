#include "Sequence.h"
#include <iostream>
#include <cassert>

Sequence::Sequence() : size_(0) {}

bool Sequence::empty() const {
  if (size_ > 0) return false;

  return true;
}

int Sequence::size() const { return size_; }

bool Sequence::insert(int pos, const ItemType &value) {
  if (pos < 0 || pos > size()) return false;  // Out of bounds.

  size_++;
  if (pos == size()) values_[size() - 1] = value;  // Edge case.

  for (int i = size() - 1; i > pos; i--) {
    // Shift elements after pos to index + 1.
    values_[i] = values_[i - 1];
  }
  values_[pos] = value;  // Put value in its correct place.

  return true;
}

int Sequence::insert(const ItemType &value) {
  if (size() >= DEFAULT_MAX_ITEMS) return -1;

  int p = size();

  // Find a p.
  for (int i = 0; i < size(); i++) {
    if (values_[i] >= value) {
      // Found a position where elements less than the position are less than
      // value and vice versa.
      p = i;
      break;
    }
  }

  size_++;

  // Shift all values greater than pos to the next index.
  for (int i = size() - 1; i > p; i--) {
    values_[i] = values_[i - 1];
  }
  values_[p] = value;  // Move value into place.

  return p;
}

bool Sequence::erase(int pos) {
  if (pos < 0 || pos >= size()) return false;

  // Move all other elements to fill the erased position.
  for (int i = pos; i < size(); i++) {
    values_[i] = values_[i + 1];
  }

  size_--;

  return true;
}

int Sequence::remove(const ItemType &value) {
  int num_items_removed = 0;

  for (int i = 0; i < size(); i++) {
    if (values_[i] == value) {

      for (int j = i; j < size() - 1; j++) {
        values_[j] = values_[j + 1];
      }

      size_--;
      num_items_removed++;
      i--;  // Check the same index again since everything shifted down.
    }
  }

  return num_items_removed;
}

bool Sequence::get(int pos, ItemType &value) const {
  if (pos < 0 || pos >= size()) return false;

  value = values_[pos];
  return true;
}

bool Sequence::set(int pos, const ItemType &value) {
  if (pos < 0 || pos >= size()) return false;

  values_[pos] = value;
  return true;
}

int Sequence::find(const ItemType &value) const {
  int p = -1;  // Default p value from spec.

  if (size() <= 0) return p;
  for (int i = 0; i < size(); i++) {
    if (values_[i] == value) {
      p = i;
      return p;
    }
  }

  return p;
}

void Sequence::swap(Sequence &other) {
  int original_size = size();
  size_ = other.size();
  other.size_ = original_size;

  for (int i = 0; i < DEFAULT_MAX_ITEMS; i++) {
    ItemType tmp = values_[i];
    values_[i] = other.values_[i];
    other.values_[i] = tmp;
  }
}
