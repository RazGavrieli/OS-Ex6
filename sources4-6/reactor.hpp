#include <pthread.h>

class reactor {
    int fd;
    pthread_t pid;
    void*(*func)(void*);

    public:
    friend void* newReactor();
    void installHandler(void *(func)(void *), int* fd);
    void RemoveHandler();

    private:
    reactor();
    ~reactor();

};

// void* newReactor() {
//     reactor* newReactor = new reactor();
//     return newReactor;
// }