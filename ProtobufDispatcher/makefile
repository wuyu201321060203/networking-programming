MUDUO_DIRECTORY = $(HOME)/build/debug-install
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib
SRC = ./
CXXFLAGS = -g -O0 -Wall -Wextra -Werror \
	       -Wconversion -Wno-unused-parameter \
           -Wold-style-cast -Woverloaded-virtual \
           -Wpointer-arith -Wshadow -Wwrite-strings \
           -march=native -rdynamic \
           -I$(MUDUO_INCLUDE)
LDFLAGS = -L$(MUDUO_LIBRARY) -lmuduo_net -lmuduo_base -lpthread -lrt -lprotobuf

all: dispatcher_test

clean:
	rm -f *.o core

dispatcher_test: query.pb.o dispatcher_test.o
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

query.pb.o: query.pb.cc
	g++ $(CXXFLAGS) -c $^

dispatcher_test.o: dispatcher_test.cc
	g++ $(CXXFLAGS) -c $^

.PHONY: all clean