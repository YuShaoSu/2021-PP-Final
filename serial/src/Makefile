CXX = g++
CFLAGS = -std=c++14 -O3 -m64
opencv = `pkg-config opencv --cflags --libs`
.PHONY: all, clean

all: target

target: main.cpp filter.cpp image.cpp
	$(CXX) $(CFLAGS) $^ -o $@ $(opencv)


clean: 
	rm -rf *.o target


	
