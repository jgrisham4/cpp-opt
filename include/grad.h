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

#ifndef GRADHEADERDEF
#define GRADHEADERDEF

#include <vector>

// Function for computing the gradient using the finite difference method
template <typename T, typename... Tn>
std::vector<T> grad_fdm(const std::vector<T>&X, const T FX, const std::vector<T>& dX, T (*f)(const std::vector<T>&,Tn...), Tn... params) {

  // Declaring variables
  std::vector<T> grad(X.size());
  std::vector<T> XpdX(X);  // X + dX
  T FXpdX;  // F(X+dX)

  // Finding gradients
  for (unsigned int i=0; i<X.size(); ++i) {
    XpdX[i] += dX[i];
    FXpdX = (*f)(XpdX,params...);
    grad[i] = (FXpdX - FX)/dX[i];
    XpdX = X;
  }

  return grad;

}


#endif
