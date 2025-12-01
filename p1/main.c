#include "inc/barrier.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static const unsigned int MICROSECS_SLEEP = 100e3; 
static const unsigned int MICROSECS_VARIANCE = 50e3;

// funcion que simula "hacer algo" esperando y luego prueba acceder al monitor
void* test_monitor(void* monitor_ref) 
{
    BarrierMonitor* monitor = (BarrierMonitor*) monitor_ref;

    long rand_sleep = random();
    unsigned int sleep_time = MICROSECS_SLEEP + (MICROSECS_VARIANCE - 2*(rand_sleep & MICROSECS_VARIANCE));

    usleep(sleep_time);

    printf("[%lu] esperando en etapa %u\n", pthread_self(), monitor->stage+1);
    wait(monitor);
    printf("[%lu] paso barrera en etapa %u\n", pthread_self(), monitor->stage);

    return NULL;
}

int main(int argc, char* argv[]) 
{
    // parseo
    size_t thread_count = (argc < 2) ? 5 : atoi(argv[1]);
    unsigned int stage_count = (argc < 3) ? 4 : atoi(argv[2]);

    pthread_t threads[thread_count];

    BarrierMonitor monitor;
    barrier_monitor_init(&monitor, thread_count);

    for (unsigned int stage = 0; stage < stage_count; stage++) 
    {
        // por cada etapa, se crean hilos y se ejecuta el test
        for (size_t thrd = 0; thrd < thread_count; thrd++) 
        {
            pthread_create(&threads[thrd], NULL, test_monitor, &monitor);
        }
        // despues se espera que terminen todas
        for (size_t thrd = 0; thrd < thread_count; thrd++) 
        {
            pthread_join(threads[thrd], NULL);
        }
    }
}
