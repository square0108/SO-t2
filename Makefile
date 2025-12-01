CC=gcc
P1_CFLAGS= -lpthread -Wall -Wextra
P2_CFLAGS=-I p2/include/ -Wall -Wextra

barr: p1/main.c barrier.o
	$(CC) $^ -o $@ $(P1_CFLAGS)

barrier.o: p1/src/barrier.c p1/inc/barrier.h
	$(CC) -c $< -o $@ $(P1_CFLAGS)


sim: p2/src/hash.c p2/src/mem_struct.c p2/src/sim.c 
	$(CC) -o sim p2/src/hash.c p2/src/mem_struct.c p2/src/sim.c $(P2_CFLAGS)

all: sim barr

.PHONY: all clean
clean:
	rm -f sim *.out *.o barr
