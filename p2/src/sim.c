#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem_struct.h"
#include "hash.h"
#define IO_BUFFER_LEN 32
#define PATH_BUFFER_LEN 128
#define LINES_TO_READ 999999

extern _Bool DEBUG_MODE;

int sim(int argc, char* argv[]);

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
	unsigned int OFFSET_MASK, nbits_offset = 0;
	size_t n_frames, page_size;
	_Bool verbose = 0;

	// Check de syntax comando
	if (argc == 5) {
		if (strcmp(argv[3], "--verbose") == 0 || strcmp(argv[3], "-V") == 0) verbose = 1;
		else {
			fprintf(stderr, "usage: ./sim <n_frames> <page_size> [--verbose] trace.txt\n");
			exit(EXIT_FAILURE);
		}
	}
	else if (argc != 4) {
		fprintf(stderr, "usage: ./sim <n_frames> <page_size> [--verbose] trace.txt\n");
		exit(EXIT_FAILURE);
	}

	n_frames = strtol(argv[1],NULL,10);
	page_size = strtol(argv[2],NULL,10);
	int patharg_idx = argc-1;

	// Redondeo de page_size (si es que no es potencia de 2)
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
	size_t n_parses = 0, hits = 0, misses = 0;

	// info print
	fprintf(stdout, "No. of frames: \t%lu\nPage size: \t%lu\nOffset bits: \t%u\n", n_frames, page_size, nbits_offset);

	while (fgets(buf, IO_BUFFER_LEN, trace) != NULL && n_parses < LINES_TO_READ) {
		buf[strcspn(buf,"\n")] = 0; // elimina newline
		unsigned int parsed_adr = strtol(buf,NULL,16); // parsear string de ref. a num. hexadecimal
		unsigned int parsed_vpn = parsed_adr >> nbits_offset;
		unsigned int parsed_offset = parsed_adr & OFFSET_MASK;
		char hit_or_miss[32];
		hit_or_miss[0] = '\0';

		struct PageTableEntry* search_result = search_page(PT,parsed_vpn);

		// Miss debido a entrada faltante
		if (search_result == NULL) {
			search_result = init_PageTableEntry(parsed_vpn,0,0,0);
			insert_to_pt(PT, parsed_vpn, search_result);
			assign_frame(PFL, search_result);
			misses += 1;
			strncat(hit_or_miss, "Miss", 5);
		}
		// Miss debido a bit invalido (pag. en disco, capacity miss)
		else if (search_result->valid == 0) {
			assign_frame(PFL, search_result);
			misses += 1;
			strncat(hit_or_miss, "Miss", 5);
		}
		// Hit
		else {
			search_result->reference = 1; // pagina popular
			hits += 1;
			strncat(hit_or_miss, "Hit", 5);
		}
		n_parses++;

		if (verbose) {
			unsigned int phys_adr = (search_result->pfn << nbits_offset) | parsed_offset;
			if (DEBUG_MODE) {
				fprintf(stdout, "DIR.V.: %#x\tNPV: %#x\tOFFSET: %#x\tH/M: %s\tMARCO: \t%u\tDIR.F.: %#x\tCLOCK HAND: %lu\n", parsed_adr, search_result->vpn, parsed_offset, hit_or_miss, search_result->pfn, phys_adr, PFL->clock_idx);
			}
			else
				fprintf(stdout, "DIR.V.: %#x\tNPV: %#x\tOFFSET: %#x\tH/M: %s\tMARCO: \t%u\tDIR.F.: %#x\n", parsed_adr, search_result->vpn, parsed_offset, hit_or_miss, search_result->pfn, phys_adr);
		}
	}

	float miss_rate = ((float) misses) / ((float) n_parses);
	fprintf(stdout, "Referencias: %lu\tFallos de página: %lu\t Tasa de fallos: %f\n", n_parses, misses, miss_rate);

	if (feof(trace))
		fprintf(stdout, "Fin de archivo\n");
	fclose(trace);
	return 0;
}