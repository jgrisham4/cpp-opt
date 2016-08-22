#include <string>
#include "file_ops.h"

using namespace std;

int main() {

  string myfile("test");
  string variable("fleece");
  double value = 3.141526;

  replace_var(myfile,variable,value);
  //replace_var<string>(myfile,std::to_string(value),"fleece");

  return 0;

}
