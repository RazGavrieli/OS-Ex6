#include "active_object.h"
#include "queue.h"

#include <stdlib.h>
#include <stdio.h>


void* runAO(void* newAO) {
    printf("runAO has been initialized! \n");
    AO *ao = (AO*)newAO;
    while (ao->running) { 
        void* handled_now = ao->func1(deQ(ao->Q)); // wait on condition inside deQ function
        void* result = ao->func2(handled_now);
    }

    free(ao->pid);
    free(ao);
    printf("active object terminated!\n");
}

AO* newAO(queue* Q, void* func1, void* func2) {
    AO *ao = (AO*)malloc(sizeof(AO));
    ao->func1 = func1;
    ao->func2 = func2;
    ao->Q = Q;
    ao->running = true;
    ao->pid = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(ao->pid,NULL, runAO, (void*)ao);
    printf("active object thread created\n");
    return ao;
}

void destroyAO(AO* ao) {
    printf("terminating active object\n");
    ao->running=false;
    free(ao->pid);
    free(ao);
}