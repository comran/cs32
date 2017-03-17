#ifndef MYMAP_INCLUDED
#define MYMAP_INCLUDED

#include <iostream>

template <typename KeyType, typename ValueType>
class MyMap {
 public:
  MyMap();
  ~MyMap();
  void clear();
  int size() const;
  void associate(const KeyType &key, const ValueType &value);

  // for a map that can't be modified, return a pointer to const ValueType
  const ValueType *find(const KeyType &key) const;

  // for a modifiable map, return a pointer to modifiable ValueType
  ValueType *find(const KeyType &key) {
    return const_cast<ValueType *>(const_cast<const MyMap *>(this)->find(key));
  }

  // C++11 syntax for preventing copying and assignment
  MyMap(const MyMap &) = delete;
  MyMap &operator=(const MyMap &) = delete;

 private:
  struct Node {
    KeyType key;
    ValueType value;
    Node *less;
    Node *more;
  };

  void deleteTree(Node *tree);
  void insert(Node *tree, const KeyType &key, const ValueType &value);
  ValueType *locate(Node *tree, const KeyType &key) const;

  Node *tree_;
  int size_;
};

// Initialize an empty tree, without a head and with a size of 0.
template <typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap()
    : tree_(nullptr), size_(0) {}

template <typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap() {
  clear();
}

// Deconstruct the tree and indicate it is nonexistent by assigning the head to
// nullptr.
template <typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear() {
  deleteTree(tree_);

  tree_ = nullptr;
}

template <typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const {
  return size_;
}

template <typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType &key,
                                          const ValueType &value) {
  // Initialize the tree, if not already done so.
  if (tree_ == nullptr) {
    tree_ = new Node({key, value, nullptr, nullptr});
    size_++;
    return;
  }

  // Otherwise, insert the value at the appropriate position in the BST.
  insert(tree_, key, value);
}

template <typename KeyType, typename ValueType>
const ValueType *MyMap<KeyType, ValueType>::find(const KeyType &key) const {
  // Don't attempt a search on an empty tree.
  if (tree_ == nullptr) return nullptr;

  return locate(tree_, key);
}

template <typename KeyType, typename ValueType>
ValueType *MyMap<KeyType, ValueType>::locate(Node *tree,
                                             const KeyType &key) const {
  // Assumes that tree already exists.

  // Found key at current branches, so return value.
  if (tree->key == key) return &tree->value;

  // Search the appropriate tree based on whether the key is greater than or
  // less than the current branch's value.
  if (key < tree->key && tree->less != nullptr) {
    return locate(tree->less, key);
  } else if (key > tree->key && tree->more != nullptr) {
    return locate(tree->more, key);
  }

  // No matching value found, so return nullptr.
  return nullptr;
}

template <typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::insert(Node *tree, const KeyType &key,
                                       const ValueType &value) {
  // Assumes that tree already exists.

  // Insert the new value into the tree by traveling through branches
  // recursively until either a match is found or a greater/less than position
  // for the key is found.
  if (key < tree->key) {
    if (tree->less == nullptr) {
      // No less-than, so insert this key into tree.
      tree->less = new Node{key, value, nullptr, nullptr};
      size_++;
    } else {
      // Traverse subtree on the lesser side.
      insert(tree->less, key, value);
    }
  } else if (key > tree->key) {
    if (tree->more == nullptr) {
      // No greater-than, so insert this key into tree.
      tree->more = new Node{key, value, nullptr, nullptr};
      size_++;
    } else {
      // Traverse subtree on the greater side.
      insert(tree->more, key, value);
    }
  } else {
    // Value matches, so replace value at current spot.
    tree->value = value;
  }
}

template <typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::deleteTree(Node *tree) {
  // Delete tree only if it exists. (stopping condition of recursion)
  if (tree == nullptr) return;

  // Deconstruct the lesser and greater sides of the tree. (recursively)
  deleteTree(tree->less);
  deleteTree(tree->more);

  // Finally delete the tree itself, and account for the change in size.
  delete tree;
  size_--;
}

#endif  // MYMAP_INCLUDED
