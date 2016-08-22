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

#ifndef FILEOPSHEADERDEF
#define FILEOPSHEADERDEF

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <iterator>
#include <sys/stat.h>
#include <unistd.h>
#include "boost/lexical_cast.hpp"

/**
 * The replace_var function opens the provided file and searches
 * for the specific string named var.  If var is found, it will be
 * replaced with the numeric value val.  This function is templated
 * so that any type can be substituted for the variable.
 *
 * @param filename name of the file that will be opened and searched.
 * @param var variable which will be searched for.
 * @param val value that will replace all occurences of the variable provided.
 * @numfmt used to set the number format of the std::ostringstream (optional, default is std::fixed).
 *
 * Author        : James Grisham
 * Date          : 06/17/2015
 * Revision date :
 */

template<typename T> 
void replace_var(const std::string filename, const std::string var, const T val, const std::string numfmt="fixed") {
  
  // Declaring some variables
  std::size_t found;
  bool found_var = false;

  // Opening input file
  std::ifstream infile(filename.c_str());
  if(!infile.is_open()) {
    std::cerr << "\nERROR: Can't open " << filename << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Reading file line by line and searching for var and replacing
  std::string line;
  std::ostringstream output;
  //output.precision(16);
  output << std::fixed;
  while (getline(infile,line)) {

    // Searching line for var
    found = line.find(var);
    while (found!=std::string::npos) {
      std::ostringstream valstring;
      if (numfmt.compare("fixed")==0) {
        valstring << std::fixed;
      }
      else if (numfmt.compare("scientific")==0) {
        valstring << std::scientific;
      }
      valstring << val;
      line.replace(line.find(var),var.length(),valstring.str());
      found_var = true;
      found = line.find(var);
    }
    output << line << "\n";

  }

  // Closing stream
  infile.close();

  if (found_var) {

    // Overwriting old file
    std::ofstream outfile(filename.c_str());
    if (!outfile.is_open()) {
      std::cerr << "\nERROR: Can't open " << filename << " to write." << std::endl;
      std::cerr << "Exiting.\n" << std::endl;
      exit(-1);
    }
    outfile << output.str();
    outfile.close();

  }
  else {
    std::cout << "WARNING: Did not find " << var << " in file named " << filename << std::endl;
  }

}

/**
 * The accessible function checks to see whether the given input 
 * file is accessible using POSIX functions.
 *
 * @param name name of the file which will be checked.
 * @return Whether the file is accessible or not.
 *
 * Author        : James Grisham
 * Date          : 06/17/2015
 * Revision date :
 */

bool accessible(const std::string name) {
  struct stat ss;
  return stat(name.c_str(),&ss)==0;
}

/**
 * The copy_file function uses input and output streams to copy a file from
 * one place to another.
 *
 * @param source_file file to be copied.
 * @param dest_file destination.
 * 
 * Author        : James Grisham
 * Date          : 06/17/2015
 * Revision date :
 */

void copy_file(const std::string source_file, const std::string dest_file) {

  // Checking to make sure the file is accessible
  if (!accessible(source_file)) {
    std::cerr << "\nERROR: input file " << source_file << " not accessible." << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
  }

  // Copying using streams
  std::ifstream src(source_file.c_str(),std::ios::binary);
  std::ofstream dst(dest_file.c_str(),std::ios::binary);
  dst << src.rdbuf();
  src.close();
  dst.close();

}

/**
 * The make_dir function is used to create a directory.  If the directory
 * already exists, it doesn't do anything.
 *
 * @param dirname name of the directory to be created.
 * 
 * Author        : James Grisham
 * Date          : 06/17/2015
 * Revision date :
 */

void make_dir(const std::string dirname) {

  // Checking to see if the directory exists
  if (!accessible(dirname)) {

    // Making the directory
    mkdir(dirname.c_str(),0777);

  }
  else {
    std::cout << "Directory " << dirname << " already exists." << std::endl;
  }

}

/**
 * The get_value function grabs a value from a file.  It is templated so that the
 * user can return any type.  Boost is required because lexical_cast is the best
 * way to convert any type to any other type.
 * 
 * @param inputfile the name of the file from which the value will be taken.
 * @param line_num line number to grab data from (zero-based).
 * @param pos an integer which holds the position in the string where the value is.
 * @return The value from the given position in the file.
 *
 * Author        : James Grisham
 * Date          : 07/06/2015
 * Revision date : 
 */
template <typename T> 
T get_value(const std::string inputfile, const unsigned int line_num, const unsigned int pos) {

  // Opening file stream
  std::ifstream infile(inputfile.c_str());
  if (!infile.is_open()) {
    std::cerr << "\nERROR: Can't open " << inputfile << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Reading line
  std::string line;
  int line_counter = 0;
  bool found_line = false;
  while(getline(infile,line)) {
    if (line_counter == line_num) {
      found_line = true;
      break;
    }
    line_counter++;
  }
  
  if (!found_line) {
    std::cerr << "\nERROR: Didn't find line " << line_num << " in file named " << inputfile << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Splitting the string
  std::stringstream ss(line);
  std::istream_iterator<std::string> begin(ss);
  std::istream_iterator<std::string> end;
  std::vector<std::string> sarray(begin,end);

 // Checking for out-of-bounds
  if (pos>sarray.size()) {
    std::cerr << "\nERROR: Requested index doesn't exist." << std::endl;
    for (unsigned int i=0; i<sarray.size(); ++i) {
      std::cerr << "index: " << i << " entry: " << sarray[i] << std::endl;
    }
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Recasting using boost lexical_cast which can convert anything to anything else
  T return_val = boost::lexical_cast<T>(sarray[pos]);

  return return_val;

}

#endif
