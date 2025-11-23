#include "hash.h"

size_t modulo_hash(unsigned int key, size_t max_buckets) {
  return (key % max_buckets);
}