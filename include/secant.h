/*
 * This file is part of cpp-opt.
 * 
 * cpp-opt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cpp-opt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with cpp-opt.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SECANTHEADERDEF
#define SECANTHEADERDEF

#include <fstream>
#include <vector>

/**
 * This header contains a templated function for root finding.  A variable
 * argument list can be passed to the function using variadic templates. 
 *
 * @param x0 initial guess.
 * @param tol tolerance used to monitor convergence.
 * @param max_iter max number of iterations to be used.
 * @param (*f)(T,Tn...) function pointer.
 * @param params parameter pack passed to *f.
 * @return the value of the root.
 *
 * \author James Grisham
 * \date 08/12/2015
 *
 */

template <typename T,typename... Tn> 
T secant(T x0, T tol, const unsigned int max_iter, T (*f)(T,Tn...), Tn... params) {

  // Declaring some variables
  int i=2;
  std::vector<T> x(max_iter);
  std::vector<T> F(max_iter);
  x[0] = x0;
  F[0] = (*f)(x0,params...);

  // Figuring out the first step
  // This probably isn't a good way to do it
  x[1] = 1.1*x[0];
  F[1] = (*f)(x[1],params...);

  // Iterating
  while ((fabs(F[i-1])>tol)&&(i<max_iter)) {
    x[i] = x[i-1] - F[i-1]*(x[i-1]-x[i-2])/(F[i-1]-F[i-2]);
    F[i] = (*f)(x[i],params...);
#ifdef VERBOSE
    std::cout << "Iteration: " << i << " x = " << x[i] << " f = " << F[i] << std::endl;
#endif
    ++i;
  }

  return x[i-1];

}

#endif
