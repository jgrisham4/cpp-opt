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

#ifndef STEEPESTDESCENTHEADERDEF
#define STEEPESTDESCENTHEADERDEF

#include <fstream>
#include <vector>
#include "gss.h"
#include "grad.h"

#define tau 0.381966

/**
 * This header contains a templated function which is an implementation
 * of the steepest descent algorithm.
 */

// Steepest descent function
template <typename T, typename... Tn>
std::vector<T> steepest_descent(const std::vector<T>& X0, const T tol, const unsigned int max_iter, T (*f)(const std::vector<T>&,Tn...), Tn... params) {

  // Declaring variables
  std::vector<T> X(X0.size());
  std::vector<T> S(X0.size());
  std::vector<T> dX(X0.size());
  std::vector<T> Fhist;
  T alpha_opt;
  T x1d_opt, xl, xu, x1, x2, fl, fu, f1, f2;
  T eps = tol/1.0;
  unsigned int K;
  int N;
  T F;
  X = X0;
  F = (*f)(X0,params...);
  Fhist.push_back(F);

  // Lambda functions for 1D search
  auto project = [&X,&S] (T alpha) {std::vector<T> Xn; for (int i=0; i<X.size(); ++i) Xn.push_back(X[i] + alpha*S[i]); return Xn;};   // this returns the X which corresponds to X + alpha*S
  auto one_d_fun = [=] (T alpha) {return (*f)(project(alpha),params...); };

  // Step size for finite difference calcuation of gradient
  for (unsigned int i=0; i<X0.size(); ++i) {
    dX[i] = X0[i]/2000.0;
  }

  // Iterating
  for (unsigned int i=0; i<max_iter; ++i) {

    // Finding gradient
    S = grad_fdm<T>(X,F,dX,f,params...);
    for (unsigned int k=0; k<S.size(); ++k) {
      S[k] *= (T) -1.0;
    }

    // Performing 1D search to find minimum along the direction of
    // steepest descent
    K = 3;
    xl = 0.0;
    xu = 1.0;
    fl = one_d_fun(xl);
    fu = one_d_fun(xu);
    x1 = (1.0 - tau)*xl + tau*xu;
    x2 = tau*xl + (1.0 - tau)*xu;
    f1 = one_d_fun(x1);
    f2 = one_d_fun(x2);
    N = (int) (ceil(log(eps)/(log(1.0 - tau)) + 3.0));
    while (K<N) {
      ++K;

      if (f1>f2) {
        xl = x1;
        fl = f1;
        x1 = x2;
        f1 = f2;
        x2 = tau*xl + (1.0 - tau)*xu;
        f2 = one_d_fun(x2);
      }
      else {
        xu = x2;
        fu = f2;
        x2 = x1;
        f2 = f1;
        x1 = (1.0 - tau)*xl + tau*xu;
        f1 = one_d_fun(x1);
      }

    }
    alpha_opt = (xl + x1 + x2 + xu)/4.0;

    // Updating X
    for (unsigned int j=0; j<X.size(); ++j) {
      X[j] += alpha_opt*S[j];
    }
    F = (*f)(X,params...);
    Fhist.push_back(F);

#ifdef VERBOSE
    std::cout << "iteration: " << i << " ";
    for (auto val : X) {
      std::cout << val << " ";
    }
    std::cout << std::endl;
#endif 

    // Checking tolerance
    if (fabs(F)<tol) {
      std::cout << "Steepest descent complete." << std::endl;
      break;
    }
  }

  return X;

}

#endif
