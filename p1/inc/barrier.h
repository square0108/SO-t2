#pragma once

#include <pthread.h>

struct BarrierMonitor;

/**
 * struct BarrierMonitor - Barrera de hebras implementada con un monitor
 */
typedef struct BarrierMonitor {
    size_t N;
    unsigned int count;
    unsigned int stage;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} BarrierMonitor;

/**
 * @brief Función para inicializar un struct BarrierMonitor.
 *
 * @param monitor Dirección de memoria del struct BarrierMonitor a iniciar.
 * @param N Cantidad de hebras que deben esperar para pasar a la siguiente fase.
 */
void barrier_monitor_init(BarrierMonitor* monitor, size_t N);

/**
 * @brief Función que intenta acceder al monitor desde el hilo actual.
 *
 * @param monitor Dirección de memoria del struct BarrierMonitor al cual se quiere acceder.
 */
void wait(BarrierMonitor* monitor);

/**
 * @brief Función para destruir un struct BarrierMonitor.
 *
 * @param monitor Dirección de memoria del struct BarrierMonitor a destruir.
 */
void barrier_monitor_destroy(BarrierMonitor* monitor);
