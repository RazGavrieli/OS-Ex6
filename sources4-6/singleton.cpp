#include <threads.h>
#include <iostream>

template <typename T> 

pthread_mutex_t locker = PTHREAD_MUTEX_INITIALIZER;
template <typename T> 
class singleton
{
private:
    T obj;
    singleton() //constructor
    {
        this->obj = new singleton;
    }
public:
    singleton instance()
    {
        //mtx_lock(&locker);
        if (obj == NULL){
            singleton();
            return obj;
        }
    }
    void destroy()
    {
        this->obj = NULL;
    }
};
