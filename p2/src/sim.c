#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define IO_BUFFER_LEN 32
#define PATH_BUFFER_LEN 128

//unsigned int VALID_BIT_MASK;
unsigned int OFFSET_MASK;

/*
 * Lee un archivo de texto con una cantidad de direcciones virtuales, línea por línea.
 * usage: ``./sim n_frames page_size [--verbose] trace.txt``
 * 
 * - PAGE_SIZE se redondeará a la potencia de 2 más cercana
*/
int main(int argc, char* argv[]) {
	size_t n_frames = strtol(argv[1],NULL,10);
	size_t page_size = strtol(argv[2],NULL,10);
	int patharg_idx = argc-1;
	_Bool verbose = 0;

	// Check de syntax comando
	if (argc == 4) verbose = 1;
	else if (argc != 3) {
		fprintf(stderr, "usage: ./sim <n_frames> <page_size> [--verbose] trace.txt\n");
		exit(EXIT_FAILURE);
	}

	// Redondeo de page_size (si es que no es potencia de 2)
	unsigned int target_pow = 0;
	while (page_size >>= 1) ++target_pow;
	page_size = ((size_t)1) << (target_pow);
	OFFSET_MASK = page_size - 1;

	// I/O archivo .txt
	char buf[IO_BUFFER_LEN];
	FILE* trace = fopen(argv[patharg_idx],"r");
	if (trace == NULL) {
		fprintf(stderr, "Error abriendo archivo: %s\n Por favor especificar path relativo a archivo", argv[patharg_idx]);
		exit(EXIT_FAILURE);
	}

	/* Lee todas las traces (descomentar y arreglar mas tarde)
	while (fgets(buf, IO_BUFFER_LEN, trace) != NULL) {
		fprintf(stdout, "%s", buf);
	}
	*/
	fgets(buf, IO_BUFFER_LEN, trace);
	buf[strcspn(buf,"\n")] = 0; // elimina newline
	unsigned int parsed_adr = strtol(buf,NULL,16);
	fprintf(stdout, "page_size: %lu\n", page_size);
	fprintf(stdout, "offset mask: %#x\n", OFFSET_MASK);
	fprintf(stdout, "address read: %#x\n", parsed_adr);

	if (feof(trace))
		fprintf(stdout, "Fin de archivo\n");
	fclose(trace);
	return 0;
}