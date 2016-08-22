#include <iostream>
#include <cmath>
#include "steepest_descent.h"

using namespace std;

template <typename T>
T rosenbrock(const std::vector<T>& X) {
  T x = X[0];
  T y = X[1];
  return pow(1.0 - x,2) + 100.0*pow(y-x*x,2);
}

template <typename T>
T quad(const std::vector<T>& X) {
  T x = X[0];
  T y = X[1];
  return x*x + y*y + 2;
}

template <typename T>
T booth(const std::vector<T>& X) {
  T x = X[0];
  T y = X[1];
  return pow(x+2.0*y-7.0,2) + pow(2.0*x+y-5.0,2);
}

int main() {

  std::vector<double> x({5.0,2.2});
  std::vector<double> x_opt;

  //x_opt = steepest_descent<double>(x, 1.0e-6, 10000, &rosenbrock<double>);
  x_opt = steepest_descent<double>(x,1.0e-6, 5000, &booth<double>);

  std::cout << "x_opt = " << std::endl;
  for (auto xx : x_opt) {
    std::cout << xx << std::endl;
  }
  std::cout << "f(x_opt) = " << booth(x_opt) << std::endl;

  return 0;

}
