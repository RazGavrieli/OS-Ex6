#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "queue.h"
#include "active_object.h"

#define nullptr ((void*)0)


// ######################################## QUEUE ######################################## //

queue* createQ() {
    /**
     * @brief Construct a new queue named Q, initialize memory for it, and return it.
     * 
     */
    queue* Q = (queue*)malloc(sizeof(queue));
    Q->firstInLine = Q->lastInLine = nullptr;
    Q->lastQuery = nullptr;
    Q->size = 0;
    
    return Q;
}

void destroyQ(queue* Q) {
    /**
     * @brief For each value in Q, free it and then free Q itself.
     * 
     */
    if (Q->lastQuery!=NULL) {
        free(Q->lastQuery);
    }
    while (Q->size!=0) { 
        /* make Q empty */
        deQ(Q);
    }
    free(Q);
}

bool enQ(void* n, queue* Q) {
    pthread_mutex_lock(&(Q->q_mutex));
    if (Q->size==0) {
        struct node* newNode = (struct node*)malloc(sizeof(struct node));
        newNode->value = malloc(sizeof(n));
        memcpy(newNode->value, n, sizeof(n));
        newNode->next = newNode->prev = nullptr;
        Q->firstInLine = Q->lastInLine = newNode;
        Q->size++;
        pthread_mutex_unlock(&(Q->q_mutex));
        return true;
    }
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->value = malloc(sizeof(n));
    memcpy(newNode->value, n, sizeof(n));
    Q->lastInLine->prev = newNode;
    newNode->next = Q->lastInLine;
    newNode->prev = nullptr;
    Q->lastInLine = newNode;
    Q->size++;
    pthread_mutex_unlock(&(Q->q_mutex));
    pthread_cond_signal(&Q->q_cond);

    
    return true; 
    
}
void* deQ(queue* Q) {
    pthread_mutex_lock(&(Q->q_mutex));
    if (Q->size==0) {
        // ####### WAIT ON COND OR SOMETHING ######
        printf("WAITING FOR NEW VALUE\n");
        pthread_cond_wait(&Q->q_cond, &Q->q_mutex);
        printf("GOT NEW VALUE\n");
    }

    if (Q->lastQuery!=NULL) {
        free(Q->lastQuery);
    }
    if (Q->size==1) {
        Q->lastQuery = malloc(sizeof(Q->firstInLine->value));
        memcpy(Q->lastQuery, Q->firstInLine->value, sizeof(Q->firstInLine->value));
        free(Q->firstInLine->value);
        free(Q->firstInLine);
        Q->size--;
        pthread_mutex_unlock(&(Q->q_mutex));

        return Q->lastQuery;
    }
    Q->lastQuery = malloc(sizeof(Q->firstInLine->value));
    memcpy(Q->lastQuery, Q->firstInLine->value, sizeof(Q->firstInLine->value));
    struct node* tempNode = Q->firstInLine->prev;
    tempNode->next = nullptr;
    free(Q->firstInLine->value);
    free(Q->firstInLine);
    Q->firstInLine = tempNode;
    Q->size--;

    pthread_mutex_unlock(&(Q->q_mutex));

    return Q->lastQuery;
}


// ########################################### AO ########################################### //
/*
typedef struct active_object {

    void* func1;
    void* func2;
    queue* Q;
}AO, active_object;

void newAO(queue*, void*, void*);
void destroyAO(AO);
*/

void* runAO(void* newAO) {
    AO *ao = (AO*)newAO;
    while (ao->running) { // chagne to while not destroyed!
        void* handled_now = ao->func1(deQ(ao->Q));
        void* result = ao->func2(handled_now);
        
        //what to do with the result.
        printf("the result is here!: %d\n", *(int*)result);
        }
    printf("active object terminated!\n");
    pthread_cancel(*ao->pid);
}

pthread_t newAO(queue* Q, void* func1, void* func2) {
    AO *ao = (AO*)malloc(sizeof(AO));
    ao->func1 = func1;
    ao->func2 = func1;
    ao->Q = Q;
    ao->running = true;
    ao->pid = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(ao->pid,NULL, runAO, (void*)ao);
    printf("active object thread created on thread: %ln\n", ao->pid);
    return *ao->pid;
}

void destroy(AO* ao) {
    printf("terminating active object on thread: %ln\n", ao->pid);
    ao->running=false;
    free(ao->pid);
    free(ao);
}

// ########################################### UTILITIES ########################################### //
void* func_example_1(void* i) {
    int* ii = (int*)i;
    *ii+=10;
    return ii;
}

void* func_example_2(void* i) {
    int* ii = (int*)i;
    *ii*=3;
    return ii;
}

int main() {
    queue* thisQ = createQ();
    for (int i = 0; i < 20; i++)
        enQ(&i, thisQ);

    //pthread_t aoid = 
    newAO(thisQ, func_example_1, func_example_2);
    while (true) {
        sleep(1);
        for (size_t i = 0; i < 3; i++)
        {
            enQ(&i, thisQ);
        }
        
    }
   // pthread_join(aoid, NULL);

    destroyQ(thisQ);
}