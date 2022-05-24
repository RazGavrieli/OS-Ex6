#include "active_object.h"
#include "queue.h"

#include <stdlib.h>
#include <stdio.h>




void* runAO(void* newAO) {
    printf("runAO has been initialized! \n");
    AO *ao = (AO*)newAO;
    while (ao->running) { 
            void* handled_now = ao->func1(deQ(ao->Q)); 
            void* result = ao->func2(handled_now);
            
            //what to do with the result.
            printf("the result is here!: %d\n", *(int*)result);
        }

    free(ao->pid);
    free(ao);
    printf("active object terminated!\n");
    //pthread_cancel(*ao->pid);
}

AO* newAO(queue* Q, void* func1, void* func2) {
    AO *ao = (AO*)malloc(sizeof(AO));
    ao->func1 = func1;
    ao->func2 = func1;
    ao->Q = Q;
    ao->running = true;
    ao->pid = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(ao->pid,NULL, runAO, (void*)ao);
    printf("active object thread created on thread: %ln\n", ao->pid);
    return ao;
}

void destroyAO(AO* ao) {
    printf("terminating active object\n");
    ao->running=false;
}
