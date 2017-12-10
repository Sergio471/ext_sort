CFLAGS=-std=c++14

all: ext_sort generator

ext_sort: ext_sort.cpp
	g++ $(CFLAGS) -o $@ $^

generator: generator.cpp
	g++ $(CFLAGS) -o $@ $^
