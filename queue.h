#pragma once
#include <stdbool.h>
#include <pthread.h>



struct node {
    void* value;
    struct node *prev;
    struct node *next;
};


typedef struct queue {
    struct node *firstInLine;
    struct node *lastInLine;
    unsigned int size;
    pthread_mutex_t q_mutex;
    pthread_cond_t q_cond;
    void* lastQuery;
}queue;

// supporting functions
queue* createQ();
void destroyQ(queue*);
bool enQ(void*, queue*);
void* deQ(queue*);


