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

#ifndef GSSHEADERDEF
#define GSSHEADERDEF

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <fstream>
#include <cmath>

#define tau 0.381966

/**
 * This header contains a function for performing a Golden Section search.
 * A variable argument list can be passed using a parameter pack.  This is
 * accomplished using variadic templates.  The optimum values are written
 * to a file named gss_results if compiled with preprocessor flag -DVERBOSE.
 *
 * @param[in] X0 initial guess.
 * @param[in] Xmin lower bound.
 * @param[in] Xmax upper bound.
 * @param[in] tol tolerance used to monitor convergence.
 * @param[in] (*f)(T,Tn...) function pointer for the objective function.
 * @param[in] params parameter pack used to pass any necessary information to the objective function.
 * @return The optimum value of the independent variable.
 *
 * Author        : James Grisham
 * Date          : 05/16/2015
 * Revision Date :
 *
 */

template <typename T,typename... Tn>
T gss(const T X0, const T Xmin, const T Xmax, const T tol, T (*f)(const T,Tn...),Tn... params) {

  // Declaring variables
  T Xopt,Xl,Xu,X1,X2;              // Design variables
  T Fl,Fu,F1,F2;                   // Function values
  T eps = tol/(Xmax - Xmin);       // Relative tolerance
  unsigned int K = 3;              // Counter
  int N;                           // Number of iterations required to achieve eps

  // Performing initial function evaluations
  Xl = Xmin;
  Xu = Xmax;
  Fl = (*f)(Xl,params...);
  Fu = (*f)(Xu,params...);

  // Calculating the initial X1 and X2 along with F1 and F2
  X1 = (1.0 - tau)*Xl + tau*Xu;
  X2 = tau*Xl + (1.0 - tau)*Xu;
  F1 = (*f)(X1,params...);
  F2 = (*f)(X2,params...);

  // Determining number of iterations required for convergence
  N = (int) (ceil(log(eps)/(log(1.0 - tau)) + 3.0));

  // Iterations
  while (K<N) {

    // Incrementing K
    ++K;

    if (F1>F2) {
      Xl = X1;
      Fl = F1;
      X1 = X2;
      F1 = F2;
      X2 = tau*Xl + (1.0 - tau)*Xu;
      F2 = (*f)(X2,params...);
    }
    else {
      Xu = X2;
      Fu = F2;
      X2 = X1;
      F2 = F1;
      X1 = (1.0 - tau)*Xl + tau*Xu;
      F1 = (*f)(X1,params...);
    }

#ifdef VERBOSE
    std::cout << "Function evaluation: " << K << " Xu = " << Xu << " Xl = " << Xl << std::endl;
#endif

  }

  // Setting the optimum X value of the design variable to be equal to the 
  // average of all the points
  Xopt = (Xl + X1 + X2 + Xu)/4.0;
  T Fopt = (*f)(Xopt,params...);

#ifdef VERBOSE
  std::cout << "\nXopt = " << Xopt << " Fopt = " << Fopt << "\n" << std::endl;
#endif

  // Writing results to file
#ifdef VERBOSE
  std::ofstream outf("gss_results");
  outf.setf(std::ios_base::scientific);
  outf << "Xopt = " << Xopt << std::endl;
  outf << "Fopt = " << Fopt << std::endl;
  outf.close();
#endif

  return Xopt;

}

#endif
