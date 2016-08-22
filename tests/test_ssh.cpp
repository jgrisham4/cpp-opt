#include <iostream>
#include "remote_tools.h"

using namespace std;

int main() {

  // Creating connection
  connection ssh_connection;
  ssh_connection.open_connection("username@somehost");

  // Listing contents of the home directory
  ssh_connection.list_dir("$HOME")

  // Checking the existence of a file in a given directory
  bool status = ssh_connection.check_file_existence("$HOME",".bashrc");
  if (status) {
    cout << "Your bashrc exists!" << endl;
  }
  else {
    cout << "Your bashrc does not exist." << std::endl;
  }

  // Transferring a file
  ssh_connection.put_file("test","$HOME");

  // Getting a file from the remote machine
  ssh_connection.get_file("some_file","$HOME","some_file");

  // Closing the connection
  ssh_connection.close_connection();

  return 0;

}
