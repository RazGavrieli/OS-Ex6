#include <stdbool.h>
#include <pthread.h>

struct node {
    void* value;
    struct node *prev;
    struct node *next;
};


struct queue {
    struct node *firstInLine;
    struct node *lastInLine;
    unsigned int size;
    pthread_mutex_t q_mutex;
    void* lastQuery;
};

// supporting functions
struct queue* createQ();
void destroyQ(struct queue*);
bool enQ(void*, struct queue*);
void* deQ(struct queue*);


