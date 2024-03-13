FLAGS = -Werror -std=c++11 -g
CC = g++

ifneq ($(OS),Windows_NT)
	FLAGS += -lncurses
endif

all: main

main: main.o game.o console.o
	$(CC) $(FLAGS) -o main main.o game.o console.o

console.o: console/console.cpp console/console.h
	$(CC) $(FLAGS) -c -o console.o console/console.cpp

main.o: main.cpp game.h
	$(CC) $(FLAGS) -c -o main.o main.cpp

game.o: game.cpp game.h
	$(CC) $(FLAGS) -c -o game.o game.cpp

clean:
	rm -f *.o main.exe main
