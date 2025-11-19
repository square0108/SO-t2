#include <stdio.h>

unsigned int page_size;
unsigned int n_frames;

/*
 * Lee un archivo de texto con una cantidad de direcciones virtuales, línea por línea.
 * usage: ``./sim n_frames page_size [--verbose] trace.txt``
*/
int main(int argc, char* argv[]) {
	if (argc == 3 || 4) {
		// tulox 500mg
	}
	else {
		dprintf(STDERR_FILENO, "usage: ./sim <n_frames> <page_size> [--verbose] trace.txt\n");
	}
	return 0;
}
