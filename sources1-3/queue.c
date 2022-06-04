#define nullptr ((void*)0)
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    pthread_mutex_init(&Q->q_mutex, NULL);
    pthread_cond_init(&Q->q_cond, NULL);

    return Q;
}

void destroyQ(queue* Q) {
    /**
     * @brief For each value in Q, free it and then free Q itself.
     * 
     */
    while (Q->size!=0) { 
        /* make Q empty */
        deQ(Q);
    }
    if (Q->lastQuery!=NULL) {
        free(Q->lastQuery);
    }
    //Q->size = -1;
    pthread_cond_broadcast(&Q->q_cond);
    pthread_cond_destroy(&Q->q_cond);
    pthread_mutex_destroy(&Q->q_mutex);

    free(Q);
}

bool enQ(void* n, queue* Q) {
    /**
     * @brief insert a new item into Q safely using Q's mutex. 
     * Also, once an new item has been added, signal to all threads on condition
     */
    pthread_mutex_lock(&(Q->q_mutex));
    if (Q->size==0) {
        struct node* newNode = (struct node*)malloc(sizeof(struct node));
        newNode->value = malloc(1032);
        memcpy(newNode->value, n, 1032);
        newNode->next = newNode->prev = nullptr;
        Q->firstInLine = Q->lastInLine = newNode;
        Q->size++;
        pthread_mutex_unlock(&(Q->q_mutex));
        pthread_cond_signal(&Q->q_cond);
        return true;
    }
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->value = malloc(1032);
    memcpy(newNode->value, n, 1032);
    Q->lastInLine->prev = newNode;
    newNode->next = Q->lastInLine;
    newNode->prev = nullptr;
    Q->lastInLine = newNode;
    Q->size++;
    pthread_mutex_unlock(&(Q->q_mutex));
   // pthread_cond_broadcast(&Q->q_cond);

    return true; 
    
}
void* deQ(queue* Q) {
    /**
     * @brief deQ an item from the Queue.
     *  If the queue is empty, wait on condition. 
     */
    if (Q->size==-1) {
        return NULL;
    }
    pthread_mutex_lock(&(Q->q_mutex));
    if (Q->size==0) {
        // ####### WAIT ON COND ######
        pthread_cond_wait(&Q->q_cond, &Q->q_mutex);
    }
    if (Q->size==0) {
        pthread_exit(NULL);
    }
    if (Q->lastQuery!=NULL) {
        free(Q->lastQuery);
    }
    if (Q->size==1) {
        Q->lastQuery = malloc(1032);
        memcpy(Q->lastQuery, Q->firstInLine->value, 1032);
        //printf("q last query is %s\n",Q->firstInLine->value);
        free(Q->firstInLine->value);
        free(Q->firstInLine);
        Q->size--;
        pthread_mutex_unlock(&(Q->q_mutex));

        return Q->lastQuery;
    }
    Q->lastQuery = malloc(1032);
    memcpy(Q->lastQuery, Q->firstInLine->value, 1032);
    struct node* tempNode = Q->firstInLine->prev;
    tempNode->next = nullptr;
    free(Q->firstInLine->value);
    free(Q->firstInLine);
    Q->firstInLine = tempNode;
    Q->size--;

    pthread_mutex_unlock(&(Q->q_mutex));

    return Q->lastQuery;
}