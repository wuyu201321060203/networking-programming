MUDUO_DIRECTORY = $(HOME)/build/debug-install
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib
SRC = ./
CXXFLAGS = -g -O0 -Wall -Wextra -Werror \
	       -Wconversion -Wno-unused-parameter \
           -Wold-style-cast -Woverloaded-virtual \
           -Wpointer-arith -Wshadow -Wwrite-strings \
           -march=native -rdynamic -std=c++11 \
           -I$(MUDUO_INCLUDE)
LDFLAGS = -L$(MUDUO_LIBRARY) -lmuduo_net_cpp11 -lmuduo_base_cpp11 -lpthread -lrt -lprotobuf \
		  					 -lboost_program_options

all: echoClient

clean:
	rm -f *.o core

echoClient: codec.o EchoMessage.pb.o echoClient.o
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

codec.o: $(SRC)/codec.cc
	g++ $(CXXFLAGS) -c $^

EchoMessage.pb..o: $(SRC)/EchoMessage.pb.cc
	g++ $(CXXFLAGS) -c $^

echoClient.o: $(SRC)/echoClient.cc
	g++ $(CXXFLAGS) -c $^

.PHONY: all clean