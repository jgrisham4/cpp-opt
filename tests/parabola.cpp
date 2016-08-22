#include <iostream>
#include <cstdlib>
#include <cmath>
#include "gss.h"

// Objective function
double obj_func(double x, double coeff, double shift) {
  return coeff*pow(x - shift,2);
}

int main() {

  // Declaring function pointer
  double (*f)(double x, double coeff, double shift) = &obj_func;

  // Calling Golden Section search function
  gss(1.2,-10.0,10.0,1.0e-6,f,2.0,2.0);

  return 0;

}
