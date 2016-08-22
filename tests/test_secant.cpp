#include <iostream>
#include <cmath>
#include "../include/secant.h"

using namespace std;

double f(double x, double a) {
  return (x-a)*(x-a);
}

int main() {

  // Declaring some variables
  
  // Calling function
  double x_0 = secant(1.0, 1.0e-8, 1000, &f, 2.0);
  std::cout << "x_0 = " << x_0 << std::endl;
  std::cout << "Should be 2.0." << std::endl;

  return 0;

}
