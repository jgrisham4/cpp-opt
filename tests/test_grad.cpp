#include <iostream>
#include <cmath>
#include "grad.h"

using namespace std;

template <typename T>
T rosenbrock(const std::vector<T>& X) {
  T x = X[0];
  T y = X[1];
  return pow(1.0 - x,2) + 100.0*pow(y-x*x,2);
}

template <typename T>
std::vector<T> grad_exact(const std::vector<T>& X) {
  T x = X[0];
  T y = X[1];
  std::vector<T> grad_e(2);
  grad_e[0] = -2.0*(1.0 - x) - 400.0*x*(y-x*x);
  grad_e[1] = 200.0*(y-x*x);
  return grad_e;
}

int main() {

  std::vector<double> x({1.0,2.0});
  std::vector<double> dx({0.01,0.01});
  std::vector<double> grad_n(2), grad_e(2);

  grad_n = grad_fdm(x,rosenbrock(x), dx, &rosenbrock<double>);
  grad_e = grad_exact(x);

  std::cout << "\nNumerical gradient:" << std::endl;
  std::cout << grad_n[0] << " " << grad_n[1] << std::endl;
  std::cout << "\nExact gradient: " << std::endl;
  std::cout << grad_e[0] << " " << grad_e[1] << std::endl;
  std::cout << "\n";

  return 0;

}
