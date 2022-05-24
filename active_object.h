#include "queue.h" 
#include <pthread.h>

typedef struct active_object {

    void* (*func1)(void*);
    void* (*func2)(void*);
    queue* Q;
    pthread_t *pid;
    bool running;

}AO, active_object;

AO *newAO(queue*, void*, void*);
void* runAO(void* /*AO*/);
void destroyAO(AO*);