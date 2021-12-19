NVCC = nvcc

CXX = g++
CFLAGS = -std=c++11 -O3 -m64
opencv = `pkg-config opencv --cflags --libs`


.PHONY: all, clean

all: target

target: main.o filter.o image.o
	$(NVCC) $(CFLAGS) $^ -o $@ $(opencv)

main.o: main.cpp
	$(CXX) $(CFLAGS) -c $^ -o $@ $(opencv)

filter.o: filter.cpp
	$(CXX) $(CFLAGS) -c $^ -o $@ $(opencv)

image.o: image.cu
	$(NVCC) $(CFLAGS) -c $^ -o $@  $(opencv)

clean: 
	rm -rf *.o target


	
