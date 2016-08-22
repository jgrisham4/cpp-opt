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

#ifndef REMOTEHEADERDEF
#define REMOTEHEADERDEF

#include <string>
#include <libssh/libssh.h>

class connection {

  private:
    ssh_session session;
    unsigned int port;
    int state;
    int hlen;
    unsigned char* hash;
    bool connection_open;
    ssh_channel channel;
    ssh_scp scp;

  public:
    connection();
    ~connection();
    void open_connection(const std::string target);
    void close_connection();
    void list_dir(const std::string dir);
    bool check_file_existence(const std::string dir, const std::string filename);
    void put_file(const std::string src_file, const std::string target_dir);
    void get_file(const std::string target_file, const std::string target_dir, const std::string local_file);

};

#endif
