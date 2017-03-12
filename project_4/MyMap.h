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
  void dump() { dumpTree(tree_); }

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
  void dumpTree(Node *tree);  // TODO(comran): Delete this!!!!!!!!!!!

  Node *tree_;
  int size_;
};

template <typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap()
    : tree_(nullptr), size_(0) {}

template <typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap() {
  clear();
}

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

  insert(tree_, key, value);
}

template <typename KeyType, typename ValueType>
const ValueType *MyMap<KeyType, ValueType>::find(const KeyType &key) const {
  if (tree_ == nullptr) return nullptr;

  return locate(tree_, key);
}

template <typename KeyType, typename ValueType>
ValueType *MyMap<KeyType, ValueType>::locate(Node *tree,
                                             const KeyType &key) const {
  // Assumes that tree already exists.

  if (tree->key == key) return &tree->value;

  if (key < tree->key && tree->less != nullptr) {
    return locate(tree->less, key);
  } else if (key > tree->key && tree->more != nullptr) {
    return locate(tree->more, key);
  }

  return nullptr;
}

template <typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::insert(Node *tree, const KeyType &key,
                                       const ValueType &value) {
  // Assumes that tree already exists.

  if (key < tree->key) {
    if (tree->less == nullptr) {
      tree->less = new Node{key, value, nullptr, nullptr};
      size_++;
    } else {
      insert(tree->less, key, value);
    }
  } else if (key > tree->key) {
    if (tree->more == nullptr) {
      tree->more = new Node{key, value, nullptr, nullptr};
      size_++;
    } else {
      insert(tree->more, key, value);
    }
  }
}

template <typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::deleteTree(Node *tree) {
  if (tree == nullptr) return;

  deleteTree(tree->less);
  deleteTree(tree->more);

  delete tree;
  size_--;
}

template <typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::dumpTree(Node *tree) {
  if (tree_ == nullptr) return;
  if (tree == nullptr) return;
  std::cout << tree->key << ": " << tree->value << std::endl;

  if (tree->less != nullptr) {
    std::cout << "LEFT: ";
    dumpTree(tree->less);
    std::cout << "End" << std::endl;
  }

  if (tree->more != nullptr) {
    std::cout << "RIGHT: ";
    dumpTree(tree->more);
    std::cout << "End" << std::endl;
  }
}

#endif // MYMAP_INCLUDED
