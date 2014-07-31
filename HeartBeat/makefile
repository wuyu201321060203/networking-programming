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

all: echoServer9877

clean:
	rm -f *.o core

echoServer9877: echo.o main.o
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

echo.o: $(SRC)/echo.cc
	g++ $(CXXFLAGS) -c $^

main.o: $(SRC)/main.cc
	g++ $(CXXFLAGS) -c $^

.PHONY: all clean