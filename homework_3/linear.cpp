#include <iostream>
#include <string>
#include <cassert>
using namespace std;

bool somePredicate(double x) {
  return x > 0;  // include <cmath> for std::sin, etc.
}

// Return false if the somePredicate function returns false for at
// least one of the array elements; return true otherwise.
bool allTrue(const double a[], int n) {
  if(n < 1) return false;
  if(n == 1) return somePredicate(*a);
  return somePredicate(*a) && allTrue(a + 1, n - 1);
}

// Return the number of elements in the array for which the
// somePredicate function returns false.
int countFalse(const double a[], int n) {
  if(n < 1) return true;
  if(n == 1) return somePredicate(*a);
  return somePredicate(*a) ? 1 : 0 + allTrue(a + 1, n - 1);
}

// Return the subscript of the first element in the array for which
// the somePredicate function returns false.  If there is no such
// element, return -1.
int firstFalse(const double a[], int n) {
  if(n < 1) return -1;

  if (somePredicate(*a) == false) return 0;

  int next_iteration = firstFalse(a + 1, n - 1);
  if(next_iteration == -1) return -1;
  return next_iteration == -1 ? -1 : 1 + next_iteration;
}

// Return the subscript of the smallest double in the array (i.e.,
// the one whose value is <= the value of all elements).  If more
// than one element has the same smallest value, return the smallest
// subscript of such an element.  If the array has no elements to
// examine, return -1.
int indexOfMin(const double a[], int n) {
  if(n < 1) return -1;

  int min_index = 1 + indexOfMin(a + 1, n - 1);
  if(*a <= a[min_index]) return 0;
  return min_index;
}

// If all n2 elements of a2 appear in the n1 element array a1, in
// the same order (though not necessarily consecutively), then
// return true; otherwise (i.e., if the array a1 does not include
// a2 as a not-necessarily-contiguous subsequence), return false.
// (Of course, if a2 is empty (i.e., n2 is 0), return true.)
// For example, if a1 is the 7 element array
//    10 50 40 20 50 40 30
// then the function should return true if a2 is
//    50 20 30
// or
//    50 40 40
// and it should return false if a2 is
//    50 30 20
// or
//    10 20 20
bool includes(const double a1[], int n1, const double a2[], int n2) {
  if(n1 == 0 && n2 == 0) return true;
  if(n1 == 0 && n2 > 0) return false;

  if(*a1 == *a2) return includes(a1 + 1, n1 - 1, a2 + 1, n2 - 1);
  return includes(a1 + 1, n1 - 1, a2, n2);
}

int main() {
  // TODO(comran): Write more tests.
  double test1[] = {10, 50, 40, 20, 50, 40, 30};
  double test2[] = {50, 40, 30};
  assert(includes(test1, 7, test2, 3));

  double test3[] = {0.0, 1.0, 2.0};
  double test4[] = {1.0, 2.0, 3.0};
  assert(!allTrue(test3, 3));
  assert(allTrue(test4, 3));
  assert(countFalse(test4, 3) == 1);

  double test5[] = {1.0, -2.0, 3.0};
  assert(firstFalse(test5, 3) == 1);
  assert(firstFalse(test4, 3) == -1);

  assert(firstFalse(test3, 3) == 0);
  assert(firstFalse(test5, 3) == 1);

  cout << "Works.\n";
}
