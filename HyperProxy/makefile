MUDUO_DIRECTORY = $(HOME)/build/debug-install
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib
SRC_DIR = ./
CXXFLAGS = -g -O0 -Wall -Wextra -Werror \
	       -Wconversion -Wno-unused-parameter \
           -Wold-style-cast -Woverloaded-virtual \
           -Wpointer-arith -Wshadow -Wwrite-strings \
           -march=native -rdynamic \
           -I$(MUDUO_INCLUDE) -std=c++11
LDFLAGS = -L$(MUDUO_LIBRARY) -lprotobuf -lmuduo_net_cpp11 -lmuduo_base_cpp11 \
		  					 -lpthread -lrt -llua5.1 -lboost_program_options

all: HyperProxy_test

clean:
	rm -f *.o core

HyperProxy_test: codec.o RelayMsg.pb.o HyperProxy.o server.o
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

codec.o: $(SRC_DIR)codec.cc
	g++ $(CXXFLAGS) -c $^

RelayMsg.o: $(SRC_DIR)RelayMsg.pb.cc
	g++ $(CXXFLAGS) -c $^

HyperProxy.o: $(SRC_DIR)HyperProxy.cc
	g++ $(CXXFLAGS) -c $^

server.o: $(SRC_DIR)server.cc
	g++ $(CXXFLAGS) -c $^

.PHONY: all clean