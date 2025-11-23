#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem_struct.h"
#include "hash.h"
#define IO_BUFFER_LEN 32
#define PATH_BUFFER_LEN 128
#define LINES_TO_READ 999999

//unsigned int VALID_BIT_MASK;
unsigned int OFFSET_MASK;

int main(int argc, char* argv[]) {
	sim(argc, argv);
	return 0;
}

/*
 * Lee un archivo de texto con una cantidad de direcciones virtuales, línea por línea.
 * usage: ``./sim n_frames page_size [--verbose] trace.txt``
 * 
 * - PAGE_SIZE se redondeará a la potencia de 2 más cercana
*/
int sim(int argc, char* argv[]) {
	_Bool verbose = 0;

	// Check de syntax comando
	if (argc == 4) verbose = 1;
	else if (argc != 3) {
		fprintf(stderr, "usage: ./sim <n_frames> <page_size> [--verbose] trace.txt\n");
		exit(EXIT_FAILURE);
	}
	size_t n_frames = strtol(argv[1],NULL,10);
	size_t page_size = strtol(argv[2],NULL,10);
	int patharg_idx = argc-1;

	// Redondeo de page_size (si es que no es potencia de 2)
	unsigned int nbits_offset = 0;
	while (page_size >>= 1) ++nbits_offset;
	page_size = ((size_t)1) << (nbits_offset);

	// I/O archivo .txt
	char buf[IO_BUFFER_LEN];
	FILE* trace = fopen(argv[patharg_idx],"r");
	if (trace == NULL) {
		fprintf(stderr, "Error abriendo archivo: %s\n Por favor especificar path relativo a archivo", argv[patharg_idx]);
		exit(EXIT_FAILURE);
	}

	// Main loop
	OFFSET_MASK = page_size - 1;
	struct PageTable* PT = init_PageTable(page_size, modulo_hash);
	struct PageFrameList* PFL = init_PageFrameList(n_frames, page_size);
	size_t n_parses = 0;

	// debugging prints
	fprintf(stdout, "No. of frames: \t%lu\n", n_frames);
	fprintf(stdout, "page_size: \t%lu\n", page_size);
	fprintf(stdout, "offset mask: \t%#x\n", OFFSET_MASK);
	fprintf(stdout, "bits offset: \t%u\n", nbits_offset);

	while (fgets(buf, IO_BUFFER_LEN, trace) != NULL && n_parses < LINES_TO_READ) {
		buf[strcspn(buf,"\n")] = 0; // elimina newline
		unsigned int parsed_adr = strtol(buf,NULL,16);
		unsigned int parsed_vpn = parsed_adr >> nbits_offset;
		unsigned int parsed_offset = parsed_adr & OFFSET_MASK;

		struct PageTableEntry* search_result = search_page(PT,parsed_vpn);

		if (search_result == NULL) { // Miss debido a entrada faltante
			fprintf(stdout,"miss!\t");
			search_result = init_PageTableEntry(parsed_vpn,0,0,0);
			insert_to_pt(PT, parsed_vpn, search_result);
			assign_frame(PFL, search_result);
		}
		else if (search_result->valid == 0) { // Miss debido a memoria no mapeada
			fprintf(stdout,"capacity miss!\t");
			assign_frame(PFL, search_result);
		}
		else { // hit
			fprintf(stdout,"hit!\t");
		}
		n_parses++;
		fprintf(stdout, "address: %#x; VPN: %#x; PFN: %#x;\n", parsed_adr, search_result->vpn, search_result->pfn);
	}

	if (feof(trace))
		fprintf(stdout, "Fin de archivo\n");
	fclose(trace);
	return 0;
}