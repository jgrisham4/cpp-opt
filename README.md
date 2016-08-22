cpp-opt
=========================
This code represents some lightweight tools for doing optimization in
C++.  The main utility of this tool lies in handling ssh connections
and dealing with grabbing and replacing data in files.  These tasks are
necessary when doing optimization using an outside code for function 
evaluations and sensitivity calculations.  This type of work is very 
easy to do in Python, but not so much in C++.  The user can see some
examples in the tests subdirectory.  

Prerequisites:
------------------------
The code is dependent upon Boost for its lexical cast and libssh for
its ssh functionality.

Installation:
------------------------
The tests can be compiled using the makefile in tests.

License:
-----------------------
Distributed under GNU GPLv3.
