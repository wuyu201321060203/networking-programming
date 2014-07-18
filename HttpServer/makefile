MUDUO_DIRECTORY = $(HOME)/build/debug-install
MUDUO_INCLUDE = $(MUDUO_DIRECTORY)/include
MUDUO_LIBRARY = $(MUDUO_DIRECTORY)/lib
SRC = ./
CXXFLAGS = -g -O0 -Wall -Wextra \
	       -Wconversion -Wno-unused-parameter \
           -Wold-style-cast -Woverloaded-virtual \
           -Wpointer-arith -Wshadow -Wwrite-strings \
           -march=native -rdynamic -std=c++11 \
           -I$(MUDUO_INCLUDE)
LDFLAGS = -L$(MUDUO_LIBRARY) -lmuduo_base_cpp11 -lpthread -lrt \
							 -lmuduo_net_cpp11

all: HttpServer_test

clean:
	rm -f *.o core

HttpServer_test: HttpContext.o HttpResponse.o HttpServer.o HttpServer_test.o
	g++ -o $@ $^ $(LDFLAGS)

HttpContext.o: $(SRC)HttpContext.cpp
	g++ $(CXXFLAGS) -c $^

HttpResponse.o: $(SRC)HttpResponse.cpp
	g++ $(CXXFLAGS) -c $^

HttpServer.o: $(SRC)HttpServer.cpp
	g++ $(CXXFLAGS) -c $^

HttpServer_test.o: $(SRC)HttpServer_test.cpp
	g++ $(CXXFLAGS) -c $^


.PHONY: all clean