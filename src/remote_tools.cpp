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

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <streambuf>
#include <string>
#include <cstring>
#include <libssh/libssh.h>
#include "remote_tools.h"

/**
 * ctor
 */

connection::connection() {
  port = 22;
  connection_open = false;
  hash = NULL;
  channel = NULL;
}


/** 
 * dtor
 */ 

connection::~connection() {
  if (connection_open) {
    std::cout << "Closing connection in destructor." << std::endl;
    ssh_disconnect(session);
    ssh_free(session);
  }
  if (hash!=NULL) {
    std::cout << "Freeing hash in destructor." << std::endl;
    free(hash);
  }
}

/**
 * Method for opening ssh connection
 *
 * @param[in] target hostname and username (eg, grisham@cfdlab.uta.edu)
 */
void connection::open_connection(const std::string target) {

  // Opening connection
  session = ssh_new();
  if (session==NULL) {
    std::cerr << "\nERROR: Can't open connection." << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Setting options
  ssh_options_set(session,SSH_OPTIONS_HOST,target.c_str());
  int rc = ssh_connect(session);
  if (rc != SSH_OK) {
    std::cerr << "Error connecting to host: " << target << std::endl;
    ssh_get_error(session);
    exit(-1);
  }

  connection_open = true;

  // Checking if the server is known 
  state = ssh_is_server_known(session);
  hlen = ssh_get_pubkey_hash(session,&hash);
  if (hlen < 0) {
    std::cerr << "ERROR: Server is not known." << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Authenticating using public key
  int auth_status = ssh_userauth_publickey_auto(session,NULL,NULL);
  if (auth_status==SSH_AUTH_ERROR) {
    std::cerr << "\nERROR: Authentication using public key failed." << std::endl;
    ssh_get_error(session);
    exit(-1);
  }
  
  std::cout << "Connection to " << target << " open." << std::endl;

}

/**
 * Method for closing the connection.
 *
 */
void connection::close_connection() {

  ssh_disconnect(session);
  ssh_free(session);
  connection_open = false;
  std::cout << "Connection closed." << std::endl;

}

/**
 * Method for listing the contents of a directory.
 *
 * @param[in] dir directory whose contents are shown.
 */
void connection::list_dir(const std::string dir) {

  // Making sure connection is open
  if (!connection_open) {
    std::cerr << "\nERROR: Connection must be open to list files." << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
  }

  // Creating channel
  int ch_status;
  channel = ssh_channel_new(session);
  if (channel==NULL) {
    std::cerr << "\nERROR: New channel not created." << std::endl;
    std::cerr << "From method: connection::list_dir()" << std::endl;
    std::cerr << "Exiting." << std::endl;
    exit(-1);
  }

  // Opening channel
  ch_status = ssh_channel_open_session(channel);
  if (ch_status!=SSH_OK) {
    std::cerr << "\nERROR: Can't open channel." << std::endl;
    std::cerr << "From method: connection::list_dir()" << std::endl;
    std::cerr << "Exiting." << std::endl;
    exit(-1);
    ssh_channel_free(channel);
  }

  // Passing command 
  std::ostringstream reqss;
  reqss << "ls -l " << dir;
  std::string req = reqss.str();
  ch_status = ssh_channel_request_exec(channel,req.c_str());
  if (ch_status!=SSH_OK) {
    std::cerr << "\nERROR: Can't execute command: " << req << std::endl;
    std::cerr << "From method: connection::list_dir()" << std::endl;
    std::cerr << "Exiting." << std::endl;
    exit(-1);
    ssh_channel_free(channel);
  }

  // Reading data from channel
  char buffer[512];
  unsigned int nbytes;
  nbytes = ssh_channel_read(channel,buffer,sizeof(buffer),0);
  while (nbytes > 0) {
    if (fwrite(buffer,1,nbytes,stdout)!=nbytes) {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
      std::cerr << "\nERROR: Problem (1) reading data from channel." << std::endl;
      std::cerr << "Exiting.\n" << std::endl;
      exit(-1);
    }
    nbytes = ssh_channel_read(channel,buffer,sizeof(buffer),0);
  }
  
  if (nbytes<0) {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    std::cerr << "\nERROR: Problem (2) reading data from channel." << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Closing the channel
  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);

}

/**
 * Method for checking the existence of a file.
 *
 * @param[in] dir directory in which file will be checked.
 * @param[in] filename name of file which will be searched for.
 * @returns true or false, depending on whether or not the file exists.
 */

bool connection::check_file_existence(const std::string dir, const std::string filename) {

  // Making sure connection is open
  if (!connection_open) {
    std::cerr << "\nERROR: Connection must be open to list files." << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
  }

  // Creating channel
  int ch_status;
  channel = ssh_channel_new(session);
  if (channel==NULL) {
    std::cerr << "\nERROR: New channel not created." << std::endl;
    std::cerr << "From method: connection::check_file_existence()" << std::endl;
    std::cerr << "Exiting." << std::endl;
    exit(-1);
  }

  // Opening channel
  ch_status = ssh_channel_open_session(channel);
  if (ch_status!=SSH_OK) {
    std::cerr << "\nERROR: Can't open channel." << std::endl;
    std::cerr << "From method: connection::check_file_existence()" << std::endl;
    std::cerr << "Exiting." << std::endl;
    exit(-1);
    ssh_channel_free(channel);
  }

  // Passing command 
  std::ostringstream reqss;
  reqss << "ls " << dir;
  std::string req = reqss.str();
  ch_status = ssh_channel_request_exec(channel,req.c_str());
  if (ch_status!=SSH_OK) {
    std::cerr << "\nERROR: Can't execute command: " << req << std::endl;
    std::cerr << "From method: connection::check_file_existence()" << std::endl;
    std::cerr << "Exiting." << std::endl;
    exit(-1);
    ssh_channel_free(channel);
  }

  // Reading data from channel
  FILE* dir_contents;
  dir_contents = fopen("contents","w");
  if (dir_contents==NULL) {
    std::cerr << "ERROR: Can't open file named contents for writing." << std::endl;
    std::cerr << "From method: connection::check_file_existence()" << std::endl;
    std::cerr << "Exiting." << std::endl;
  }
  char buffer[512];
  unsigned int nbytes;
  nbytes = ssh_channel_read(channel,buffer,sizeof(buffer),0);
  while (nbytes > 0) {
    if (fwrite(buffer,1,nbytes,dir_contents)!=nbytes) {
      ssh_channel_close(channel);
      ssh_channel_free(channel);
      std::cerr << "\nERROR: Problem (1) reading data from channel." << std::endl;
      std::cerr << "From method: connection::check_file_existence()" << std::endl;
      std::cerr << "Exiting.\n" << std::endl;
      exit(-1);
    }
    nbytes = ssh_channel_read(channel,buffer,sizeof(buffer),0);
  }
  fclose(dir_contents);
  
  if (nbytes<0) {
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    std::cerr << "\nERROR: Problem (2) reading data from channel." << std::endl;
    std::cerr << "From method: connection::check_file_existence()" << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Closing the channel
  ssh_channel_send_eof(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);

  // Reading file
  std::ifstream dircontents("contents");
  if (!dircontents.is_open()) {
    std::cerr << "\nERROR: Can't open file named contents for reading." << std::endl;
    std::cerr << "From method: connection::check_file_existence()" << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }
  std::string line;
  std::ostringstream output;
  std::size_t found;
  output << std::fixed;
  bool located = false;
  while (getline(dircontents,line)) {
    found = line.find(filename);
    if (found!=std::string::npos) {
      located = true;
      break;
    }
  }

  return located;

}

/**
 * Method for scp-ing a file from local machine to remote host.
 * 
 * @param[in] src_file name of file to be transferred.
 * @param[in] target_dir directory to which the file will be transferred.
 */

void connection::put_file(const std::string src_file, const std::string target_dir) {

  // Making sure ssh session is open
  if (!connection_open) {
    std::cerr << "\nERROR: Connection must be open to scp files." << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
  }

  // Creating new scp 
  scp = ssh_scp_new(session,SSH_SCP_WRITE,target_dir.c_str());
  if (scp==NULL) {
    std::cerr << "\nERROR: Couldn't create new scp session." << std::endl;
    std::cerr << ssh_get_error(session) << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Initializing scp connection
  int rc = ssh_scp_init(scp);
  if (rc!=SSH_OK) {
    std::cerr << "\nERROR: Couldn't initialize the scp connection." << std::endl;
    std::cerr << ssh_get_error(session) << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    ssh_scp_free(scp);
    exit(-1);
  }
  
  // Reading the file into an std::string
  std::ifstream srcstream(src_file.c_str());
  std::string file_contents;
  srcstream.seekg(0,std::ios::end);
  file_contents.reserve(srcstream.tellg());
  srcstream.seekg(0,std::ios::beg);
  file_contents.assign((std::istreambuf_iterator<char>(srcstream)),std::istreambuf_iterator<char>());
  int length = strlen(file_contents.c_str());

  // Creating the file
  rc = ssh_scp_push_file(scp,src_file.c_str(),length,0600);
  if (rc!=SSH_OK) {
    std::cerr << "\nERROR: Couldn't push the file." << std::endl;
    std::cerr << ssh_get_error(session) << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Writing the data to the file
  rc = ssh_scp_write(scp,file_contents.c_str(),length);
  if (rc!=SSH_OK) {
    std::cerr << "\nERROR: Couldn't write data to the file." << std::endl;
    std::cerr << ssh_get_error(session) << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Cleaning up
  ssh_scp_close(scp);
  ssh_scp_free(scp);

}

/**
 * Method for getting a file from the remote host.
 *
 * @param[in] target_file name of file to be scp-ed.
 * @param[in] target_dir name of directory which contains target_file.
 * @param[in] local_file name or full path at which file will be stored.
 */

void connection::get_file(const std::string target_file, const std::string target_dir, const std::string local_file) {

  // Making sure ssh session is open
  if (!connection_open) {
    std::cerr << "\nERROR: Connection must be open to scp files." << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
  }

  // Forming file name
  std::string trgt(target_dir+"/"+target_file);

  // Creating new scp 
  scp = ssh_scp_new(session,SSH_SCP_READ,trgt.c_str());
  if (scp==NULL) {
    std::cerr << "\nERROR: Couldn't create new scp session." << std::endl;
    std::cerr << ssh_get_error(session) << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Initializing scp connection
  int rc = ssh_scp_init(scp);
  if (rc!=SSH_OK) {
    std::cerr << "\nERROR: Couldn't initialize the scp connection." << std::endl;
    std::cerr << ssh_get_error(session) << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    ssh_scp_free(scp);
    exit(-1);
  }
 
  // Creating pull request
  int size,mode;
  char *filename, *buffer;
  rc = ssh_scp_pull_request(scp);
  if (rc!=SSH_SCP_REQUEST_NEWFILE) {
    std::cerr << "\nERROR: Can't receive information about file." << std::endl;
    std::cerr << ssh_get_error(session) << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Getting size and filename, etc
  size = ssh_scp_request_get_size(scp);
  filename = strdup(ssh_scp_request_get_filename(scp));
  mode = ssh_scp_request_get_permissions(scp);
  std::cout << "Reading " << filename << " size: " << size << std::endl;

  // Allocating memory for the buffer
  buffer = (char *)malloc(size);
  if (buffer==NULL) {
    std::cerr << "\nERROR: Can't allocate memory for buffer." << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Reading file
  ssh_scp_accept_request(scp);
  rc = ssh_scp_read(scp,buffer,size);
  if (rc==SSH_ERROR) {
    std::cerr << "\nERROR: Problem receiving file data:" << std::endl;
    std::cerr << ssh_get_error(session) << std::endl;
    std::cerr << trgt << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    free(buffer);
    exit(-1);
  }

  // Writing 
  FILE* fp;
  fp = fopen(local_file.c_str(),"w");
  fwrite(buffer,1,size,fp);
  fclose(fp);

  // Pulling
  rc = ssh_scp_pull_request(scp);
  if (rc!=SSH_ERROR) {
    std::cerr << "\nERROR: Unexpected request:" << std::endl;
    std::cerr << ssh_get_error(session) << std::endl;
    std::cerr << "Exiting.\n" << std::endl;
    exit(-1);
  }

  // Cleaning up
  free(filename);
  free(buffer);
  ssh_scp_close(scp);
  ssh_scp_free(scp);

}
