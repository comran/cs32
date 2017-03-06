#include "Sequence.h"  // class template from problem 1

class Complex {
 public:
  Complex(double r = 0, double i = 0) : m_real(r), m_imag(i) {}
  double real() const { return m_real; }
  double imag() const { return m_imag; }

 private:
  double m_real;
  double m_imag;
};

int main() {
  Sequence<int> si;
  si.insert(50);  // OK
  Sequence<Complex> sc;
  sc.insert(0, Complex(50, 20));  // OK
  sc.insert(Complex(40, 10));     // error!
}
