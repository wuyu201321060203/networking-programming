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
LDFLAGS = -L$(MUDUO_LIBRARY) -lmuduo_net -lmuduo_base -lpthread -lrt

all: sudoku_server

clean:
	rm -f *.o core

sudoku_server: sudoku.o server_threadpool.o
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

sudoku.o: $(SRC)/sudoku.cc
	g++ $(CXXFLAGS) -c $^

server_threadpool.o: $(SRC)/server_threadpool.cc
	g++ $(CXXFLAGS) -c $^

.PHONY: all clean