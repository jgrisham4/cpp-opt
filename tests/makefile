CXX:=g++
CXXFLAGS:=-std=c++11
CPPFLAGS:=-DVERBOSE 
INCDIR:=../include
INCLUDE:=-I$(INCDIR)
LIBS:=-lssh
SRCDIR:=../src
DEPS:=$(wildcard $(INCLUDE)/*.h)
SRCS:=$(wildcard ../src/*.cpp)
OBJS:=$(patsubst %.cpp,%.o,$(SRCS))
TRGTS:=$(patsubst %.cpp,%,$(filter-out $(wildcard *ssh*),$(wildcard *.cpp)))

all: $(OBJS) $(TRGTS) test_ssh
	
$(OBJS): $(SRCS)
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) $(LIBS) -o $@ $<

test_ssh.o: test_ssh.cpp $(OBJS) $(INCDIR)/remote_tools.h $(SRCDIR)/remote_tools.cpp 
	$(CXX) -c -o $@ $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) $<

test_ssh: test_ssh.o $(OBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(CPPFLAGS) $(INCLUDE) $(LIBS) $^ 

%: %.cpp
	$(CXX) $< -o $@ $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE)

.PHONY: clean
clean:
	rm $(TRGTS) $(OBJS) test_ssh test_ssh.o

