#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "queue.h"
#define nullptr ((void*)0)

struct queue* createQ() {
    /**
     * @brief Construct a new queue named Q, initialize memory for it, and return it.
     * 
     */
    struct queue* Q = (struct queue*)malloc(sizeof(struct queue));
    Q->firstInLine = Q->lastInLine = nullptr;
    Q->lastQuery = nullptr;
    Q->size = 0;
    
    return Q;
}

void destroyQ(struct queue* Q) {
    /**
     * @brief For each value in Q, free it and then free Q itself.
     * 
     */
    if (Q->lastQuery!=NULL) {
        free(Q->lastQuery);
    }
    while (Q->size!=0) { 
        /* empty q */
        deQ(Q);
    }
    free(Q);
}

bool enQ(void* n, struct queue* Q) {
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
    return true; 
    
}
void* deQ(struct queue* Q) {
    if (Q->size==0) {
        // ####### WAIT ON COND OR SOMETHING ######
    }

    pthread_mutex_lock(&(Q->q_mutex));
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

int main() {
    struct queue* thisQ = createQ();
    for (size_t i = 0; i < 10; i++)
    {
        enQ(&i, thisQ);
    }
    
    for (size_t i = 0; i < 10; i++)
    {
       printf("%d ", *(int*)deQ(thisQ));
    }


    destroyQ(thisQ);
}