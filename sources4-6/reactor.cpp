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