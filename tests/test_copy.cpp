#include <iostream>
#include <string>
#include "file_ops.h"

using namespace std;

int main() {

  string src_f = "/home/grisham/asdf";
  string dst_f = "asdf";
  copy_file(src_f,dst_f);

  make_dir("test1");
  system("touch ./test1/aaa");
  make_dir("test1");

  return 0;

}
