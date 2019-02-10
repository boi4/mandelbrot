.PHONY: all, clean, run

all:  mandel run

debug: mandel.cpp
	g++ mandel.cpp -o mandel -lSDL2 -g

mandel: mandel.cpp
	g++ mandel.cpp -o mandel -O2 -lSDL2

run:
	./mandel
