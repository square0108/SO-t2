CC=gcc
P1_CFLAGS= -pthread -Wall -Wextra
P2_CFLAGS=-I p2/include/ -Wall -Wextra

all: sim barr

barr: p1/main.c barrier.o
	$(CC) $(P1_CFLAGS) $^ -o $@

barrier.o: p1/src/barrier.c p1/inc/barrier.h
	$(CC) $(P1_CFLAGS) -c $< -o $@

sim: p2/src/hash.c p2/src/mem_struct.c p2/src/sim.c 
	$(CC) -o sim p2/src/hash.c p2/src/mem_struct.c p2/src/sim.c $(P2_CFLAGS)

.PHONY: all clean
clean:
	rm -f sim *.out *.o barr
