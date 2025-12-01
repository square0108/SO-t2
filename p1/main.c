#include "inc/barrier.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static const unsigned int MICROSECS_SLEEP = 100e3; 
static const unsigned int MICROSECS_VARIANCE = 50e3;

struct ThreadArgs {
        BarrierMonitor* monitor;
        size_t id;
        unsigned int stage_count; 
};

// funcion que simula "hacer algo" esperando y luego prueba acceder al monitor
void* test_monitor(void* thread_args) 
{
    struct ThreadArgs* args = (struct ThreadArgs*) thread_args;
    BarrierMonitor* monitor = args->monitor;
    size_t id = args->id;
    unsigned int stage_count = args->stage_count;

    for (size_t stage = 0; stage < stage_count; stage++)
    {
        long rand_sleep = random();
        unsigned int sleep_time = MICROSECS_SLEEP + (MICROSECS_VARIANCE - 2*(rand_sleep & MICROSECS_VARIANCE));
        usleep(sleep_time);
        printf("[Thread %lu - ID %lu] esperando en etapa %u\n", id, pthread_self(), monitor->stage+1);
        wait(monitor);
        printf("[Thread %lu - ID %lu] pasó barrera en etapa %u\n", id, pthread_self(), monitor->stage);
    }

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


    for (size_t thrd = 0; thrd < thread_count; thrd++)
    {
        struct ThreadArgs *args = malloc(sizeof(struct ThreadArgs));
        args->monitor = &monitor;
        args->id = thrd;
        args->stage_count = stage_count;
        pthread_create(&threads[thrd], NULL, test_monitor, (void*) args);
    }

    for (size_t thrd = 0; thrd < thread_count; thrd++) {
        pthread_join(threads[thrd], NULL);
    }

    barrier_monitor_destroy(&monitor);
}
