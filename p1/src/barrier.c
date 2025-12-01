#include "../inc/barrier.h"
#include <pthread.h>

void barrier_monitor_init(BarrierMonitor *monitor, size_t N) 
{
    // inicializar el monitor lol
    monitor->N = N;
    pthread_mutex_init(&monitor->mutex, NULL);
    pthread_cond_init(&monitor->cond, NULL);
    monitor->count = 0;
    monitor->stage = 0;
}

void wait(BarrierMonitor* monitor) 
{
    pthread_mutex_lock(&monitor->mutex);
    monitor->count++;

    // aunque mesa sea mas recomendado, aca sirve hoare porque es un problema simple, en donde se
    // puede asegurar facilmente que se liberen todos cuando la condicion se rompe y se devuelva todo al inicio
    if (monitor->count < monitor->N) {
        pthread_cond_wait(&monitor->cond, &monitor->mutex);
    }
    else {
        // aca entra cuando count == N, es decir, la barrier ya tiene todos los threads que necesita
        // se reinicia todo y se manda el broadcast para liberar los threads
        monitor->stage++;
        monitor->count = 0;
        pthread_cond_broadcast(&monitor->cond);
    }

    // aca se debe hacer todo lo que se queria hacer con el monitor, pero como es solo implementar
    // wait(), no se hace nada

    pthread_mutex_unlock(&monitor->mutex);
}

void barrier_monitor_destroy(BarrierMonitor *monitor)
{
    pthread_mutex_destroy(&monitor->mutex);
    pthread_cond_destroy(&monitor->cond);
}
