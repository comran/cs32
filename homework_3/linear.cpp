bool allTrue(const double a[], int n) {
  if(n < 1) return false;
  if(n == 1) return somePredicate(*a);
  return somePredicate(*a) && allTrue(a + 1, n - 1);
}

int countFalse(const double a[], int n) {
  if(n < 1) return true;
  if(n == 1) return somePredicate(*a);
  return somePredicate(*a) ? 1 : 0 + allTrue(a + 1, n - 1);
}

int firstFalse(const double a[], int n) {
  if(n < 1) return -1;

  if (somePredicate(*a) == false) return 0;

  int next_iteration = firstFalse(a + 1, n - 1);
  if(next_iteration == -1) return -1;
  return next_iteration == -1 ? -1 : 1 + next_iteration;
}

int indexOfMin(const double a[], int n) {
  if(n < 1) return -1;

  int min_index = 1 + indexOfMin(a + 1, n - 1);
  if(*a <= a[min_index]) return 0;
  return min_index;
}

bool includes(const double a1[], int n1, const double a2[], int n2) {
  if(n1 == 0 && n2 == 0) return true;
  if(n1 == 0 && n2 > 0) return false;

  if(*a1 == *a2) return includes(a1 + 1, n1 - 1, a2 + 1, n2 - 1);
  return includes(a1 + 1, n1 - 1, a2, n2);
}
