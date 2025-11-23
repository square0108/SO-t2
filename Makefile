CC=gcc
P2_CFLAGS=-I p2/include/ -Wall -Wextra

sim: p2/src/hash.c p2/src/mem_struct.c p2/src/sim.c 
	$(CC) -o sim p2/src/hash.c p2/src/mem_struct.c p2/src/sim.c $(P2_CFLAGS)

all: sim

.PHONY: all clean
clean:
	rm -f sim *.out