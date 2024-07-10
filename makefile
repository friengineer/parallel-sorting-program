EXE = main
CC = gcc
CCFLAGS = -Wall -fopenmp

all:
	$(CC) $(CCFLAGS) -o $(EXE) main.c

