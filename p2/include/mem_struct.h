#include <stddef.h>
/*
* Contiene ambos el Número de Página Virtual y el Número de Marco Físico para lograr una implementación de hashing para la tabla de páginas.
* Esto es descrito en la presentacion 9, "AdministraciónMemoria-parte2", diapositiva 15.
*
* @param  vpn : Número de página virtual, contenido sólo para hashing.
* @param  pfn : Número de marco físico, para mapear a lista de marcos de página.
* @param  valid : "bit" de validez, indica si la dirección virtual está asignada a un marco físico.
* @param  reference : "bit" de referencia, usado por algoritmo reloj para escoger víctimas. Actualizado en `assign_frame()` y en `evict_page()`.
*/
struct PageTableEntry {
  unsigned int vpn;
  unsigned int pfn;
  _Bool valid;
  _Bool reference;
};

/*
  @brief Lista de paginas presentes en memoria fisica simulada (y no escritas a disco)

  @param  n_frames : Número máximo de marcos de página que pueden reservarse
  @param  frames : Arreglo que contiene las direcciones físicas a marcos de página.
    Actualmente (23/11/25) implementado como arreglo de punteros unsigned int*, cada puntero reserva `page_size` direcciones de tipo `unsigned int`.
  @param  clock_idx : Puntero a marco de página, utilizado para implementar algoritmo de reloj de reemplazo de páginas.
  @param  claimed_frames : Arreglo que almacena estado de ocupación de cada marco:
    Si el marco i-ésimo está siendo ocupado por alguna PV, claimed_frames[i] == 1.
    En caso contrario, claimed_frames[i] == 0.
  @param  page_size : Determina cuántas direcciones serán reservadas por cada marco de página.
  @param  pte_map : Necesario también para algoritmo reloj, pues este debe examinar los Page Table Entry mapeados a cada marco.
*/
struct PageFrameList {
  size_t n_frames;
  unsigned int** frames;
  size_t clock_idx;
  _Bool* claimed_frames; // En realidad deberia usar una free list, pero no hay tiempo
  size_t page_size;
  struct PageTableEntry** pte_map;
};

/*
* @brief Tabla de páginas implementada con esquema de hash table para la búsqueda del PTE de una dirección virtual dada. 
* 
* @param array : Contenedor para los mapeos de N° página virtual -> N° marco físico. Las búsquedas se realizan sobre este contenedor.
*/
struct PageTable {
  struct PageTableEntry** array;
  size_t page_size;
  size_t max_buckets;
  size_t used_buckets;
  size_t (*hash_function)(unsigned int /*adr*/, size_t /*buckets*/);
};

// Inicializaciones de struct

struct PageTableEntry* init_PageTableEntry(unsigned int vpn, unsigned int pfn, _Bool valid, _Bool present);
struct PageFrameList* init_PageFrameList(size_t n_frames, size_t page_size);
struct PageTable* init_PageTable(unsigned int page_size, size_t (*hash_function)(unsigned int, size_t));

// Operaciones sobre tablas

int insert_to_pt(struct PageTable* pt, unsigned int address_vpn, struct PageTableEntry* new_pte);
struct PageTableEntry* search_page(struct PageTable* pt, unsigned int address_vpn);
unsigned int assign_frame(struct PageFrameList* frlist, struct PageTableEntry* entry);
size_t evict_page(struct PageFrameList* frlist);

// Auxiliares

/*
* Computa el logaritmo base 2 entero de un argumento `num`. Si `num` no es una potencia de 2, redondeará hacia la menor más cercana.
*/
unsigned int log2_uint(const unsigned int num);

// Manejo de heap

void free_PageTableEntry(struct PageTableEntry* pte);