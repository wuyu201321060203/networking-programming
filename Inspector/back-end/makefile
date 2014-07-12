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
LDFLAGS = -L$(MUDUO_LIBRARY) -lmuduo_base_cpp11 -lpthread -lrt -ltcmalloc_and_profiler \
							 -lmuduo_net_cpp11

all: Inspector_test

clean:
	rm -f *.o core
Inspector_test: HttpContext.o HttpResponse.o HttpServer.o ProcessInfo.o \
				ProcessInspector.o PerformanceInspector.o CpuUtilizationInspector.o \
				BandwidthInspector.o Inspector.o Inspector_test.o
	g++ -o $@ $^ $(LDFLAGS)

HttpContext.o: $(SRC)HttpContext.cpp
	g++ $(CXXFLAGS) -c $^

HttpResponse.o: $(SRC)HttpResponse.cpp
	g++ $(CXXFLAGS) -c $^

HttpServer.o: $(SRC)HttpServer.cpp
	g++ $(CXXFLAGS) -c $^

ProcessInfo.o: $(SRC)ProcessInfo.cpp
	g++ $(CXXFLAGS) -c $^

ProcessInspector.o: $(SRC)ProcessInspector.cpp
	g++ $(CXXFLAGS) -c $^

PerformanceInspector.o: $(SRC)PerformanceInspector.cpp
	g++ $(CXXFLAGS) -c $^

CpuUtilizationInspector.o: $(SRC)CpuUtilizationInspector.cpp
	g++ $(CXXFLAGS) -c $^

BandwidthInspector.o: $(SRC)BandwidthInspector.cpp
	g++ $(CXXFLAGS) -c $^

Inspector.o: $(SRC)Inspector.cpp
	g++ $(CXXFLAGS) -c $^

Inspector_test.o: $(SRC)Inspector_test.cpp
	g++ $(CXXFLAGS) -c $^

.PHONY: all clean