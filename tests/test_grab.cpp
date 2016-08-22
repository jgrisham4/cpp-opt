#include <iostream>
#include "file_ops.h"

using namespace std;

int main() {

  // Declaring variables
  unsigned int pos = 1;
  unsigned int line = 7;
  double Tavg;
  string Tfile("Tavg.dat");
  
  // Calling function
  Tavg = get_value<double>(Tfile,line,pos);

  cout << "Tavg = " << Tavg << " K" << endl;

  return 0;

}
