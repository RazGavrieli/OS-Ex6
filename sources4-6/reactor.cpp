#include "reactor.hpp"
#include <stdlib.h>
reactor::reactor() {
}

reactor::~reactor() {
    delete this;
}

void reactor::installHandler(void *(func)(void *), int* fd) {
    this->fd = *fd;
    this->func = func;
    pthread_create(&this->pid, NULL, func, (void*)fd);    
}

void reactor::RemoveHandler() {
    this->fd = 0;
    this->func = NULL;
    pthread_cancel(this->pid);
    this->~reactor();

}
/* HPP FILE:
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
*/
