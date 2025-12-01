#include "mem_struct.h"
#include <stdio.h>
#include <stdlib.h>
#define PT_INIT_SIZE 8193

_Bool DEBUG_MODE = 0;

// Manejo y asignacion de marcos de pagina

/*
* Ojo que debe utilizarse el N.P.V. de una dirección, NO una dirección virtual completa.
*/
struct PageTableEntry* search_page(struct PageTable* pt, unsigned int address_vpn) {
  size_t pos_idx = pt->hash_function(address_vpn,pt->max_buckets);
  size_t collisions = 0;
  size_t max_collisions = pt->max_buckets;

  while (collisions < max_collisions) {
    struct PageTableEntry* search_result = pt->array[pos_idx];

    if (search_result == NULL) { // entrada/valor vacía, FALLO de pagina
      return NULL;
    }
    else if (search_result->vpn == address_vpn) { // HIT
      return search_result;
    }
    else if (search_result->vpn != address_vpn) { // Colisión de tabla hash
      collisions += 1;
      pos_idx = (pos_idx + 1) % pt->max_buckets;
    }
  }

  printf("Limite de colisiones en PageTable alcanzado\n");
  return NULL;
}

int insert_to_pt(struct PageTable* pt, unsigned int address_vpn, struct PageTableEntry* new_pte) {
  size_t pos_idx = pt->hash_function(address_vpn, pt->max_buckets);
  size_t collisions = 0;

  while (collisions < pt->max_buckets) {
    // Revisar slot/bucket obtenido por el hash
    struct PageTableEntry** search_result = &pt->array[pos_idx];
    if (*search_result == NULL) {
      *search_result = new_pte;
      return 0;
    }
    else if ((*search_result)->vpn == address_vpn) { // HIT de insercion..
      // Actualizar existente
      (*search_result)->pfn = new_pte->pfn;
      (*search_result)->valid = new_pte->valid;
      (*search_result)->reference = new_pte->reference;
      free_PageTableEntry(new_pte);
      return 0;
    }
    // Si llega acá, ocurrió una colisión en la tabla hash
    collisions += 1;
    pos_idx = (pos_idx + 1) % pt->max_buckets;
  }
  fprintf(stderr, "Error en inserción de página de NPV %u\n", address_vpn);
  return -1;
}

unsigned int assign_frame(struct PageFrameList* frlist, struct PageTableEntry* entry) {
  // Buscar marco de pagina desocupado (claimed = 0)
  size_t idx = 0;
  while (1) {
    // Si claimed == 0, encontró un marco desocupado
    if (frlist->claimed_frames[idx] == 0) {
      frlist->claimed_frames[idx] = 1;
      frlist->pte_map[idx] = entry;
      entry->pfn = frlist->frames[idx];
      entry->valid = 1;
      entry->reference = 1;
      return entry->pfn;
    }
    // No se encontraron marcos desocupados en toda la memoria física, debe liberarse uno.
    if (idx == frlist->n_frames-1) {
      idx = evict_page(frlist);
      continue;
    }
    idx++;
  }  
}

/*
* Libera un marco de página expulsando a una P.V. según el algoritmo "Reloj", y retorna la posición apuntada por la mano de reloj que fue liberada.
*/
size_t evict_page(struct PageFrameList* frlist) {
  size_t clock_start = frlist->clock_idx;
  do {
    struct PageTableEntry* candidate = frlist->pte_map[frlist->clock_idx];
    if (candidate->reference == 0) {
      break;
    }
    else {
      if (DEBUG_MODE) fprintf(stdout, "Second chance triggered: VPN: %#x; PFN: %#x; Clock hand: %lu\n", candidate->vpn, candidate->pfn, (frlist->clock_idx));
      candidate->reference = 0; // segunda chance
    }

    // Mover mano de reloj (implementa "lista circular")
    frlist->clock_idx = (frlist->clock_idx + 1) % frlist->n_frames;
  } while (clock_start != frlist->clock_idx); // reloj dio un "loop" de vuelta a su posicion inicial. Si no liberó ningún marco, simplemente liberará el mismo con el que empezó
  struct PageTableEntry* victim = frlist->pte_map[frlist->clock_idx];
  size_t victim_idx = frlist->clock_idx;

  // Invalidar página expulsada
  if (victim != NULL) {
    victim->valid = 0;
  }
  // actualizar data de frame list
  frlist->claimed_frames[victim_idx] = 0;
  frlist->pte_map[victim_idx] = NULL;
  frlist->clock_idx = (frlist->clock_idx + 1) % frlist->n_frames;

  return victim_idx;
}

// Inicializaciones de struct

/*
* - `pt->array` es inicializado con todas las PTEs a punteros NULL.
*/
struct PageTable* init_PageTable(unsigned int page_size, size_t (*hash_function)(unsigned int, size_t)) {
  struct PageTable* pt = malloc(sizeof(struct PageTable));
  pt->page_size = page_size;
  pt->max_buckets = PT_INIT_SIZE;
  pt->used_buckets = 0;
  pt->array = calloc(pt->max_buckets, sizeof(struct PageTableEntry*));
  pt->hash_function = hash_function;
  return pt;
}

struct PageFrameList* init_PageFrameList(size_t n_frames, size_t page_size) {
  struct PageFrameList* list = malloc(sizeof(struct PageFrameList));
  list->n_frames = n_frames;
  list->page_size = page_size;
  list->claimed_frames = calloc(n_frames,sizeof(_Bool));

  // Reservar `n_frames` marcos de página, y reservar `page_size` direcciones físicas por cada marco de página
  list->pte_map = calloc(n_frames, sizeof(struct PageTableEntry*));
  list->frames = (unsigned int*) calloc(n_frames, sizeof(unsigned int));
  for (size_t i = 0; i < n_frames; i++) {
    list->frames[i] = (unsigned int) i; // valores de ejemplo para cada marco físico
  }
  // Asigna "mano de reloj" de clock algorithm a primer marco
  list->clock_idx = 0;

  return list;
}

struct PageTableEntry* init_PageTableEntry(unsigned int vpn, unsigned int pfn, _Bool valid, _Bool reference) {
  struct PageTableEntry* pte = malloc(sizeof(struct PageTableEntry));
  pte->vpn = vpn;
  pte->pfn = pfn;
  pte->valid = valid;
  pte->reference = reference;
  return pte;
}

// Manejo de memoria heap

void free_PageTableEntry(struct PageTableEntry* pte) {
  free(pte);
}

// Funciones auxiliares

unsigned int log2_uint(const unsigned int num) {
  unsigned int target_pow = 0;
  unsigned int copy = num;
	while (copy >>= 1) ++target_pow;
	return target_pow;
}