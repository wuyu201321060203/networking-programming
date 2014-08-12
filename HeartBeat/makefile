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

BIN_PROGRAM = echoServer

all: $(BIN_PROGRAM)

clean:
	test -z "$(BIN_PROGRAM)" || rm -f $(BIN_PROGRAM)
	rm -f *.o core

$(BIN_PROGRAM): codec.o EchoMessage.pb.o echo.o main.o util.o
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

codec.o: $(SRC)/codec.cc
	g++ $(CXXFLAGS) -c $^

EchoMessage.pb..o: $(SRC)/EchoMessage.pb.cc
	g++ $(CXXFLAGS) -c $^

echo.o: $(SRC)/echo.cc
	g++ $(CXXFLAGS) -c $^

main.o: $(SRC)/main.cc
	g++ $(CXXFLAGS) -c $^

util.o: $(SRC)/util.cc
	g++ $(CXXFLAGS) -c $^

.PHONY: all clean