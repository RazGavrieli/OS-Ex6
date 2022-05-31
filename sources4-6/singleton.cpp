#include <iostream>
#include <mutex>
#include <pthread.h>


using namespace std;
pthread_mutex_t singletonLocker;

template <typename T>
class singleton
{
private:
    static singleton *self;
    T obj;

    singleton(T obj) {
        this->obj = obj;
    }

public:
    static singleton *instance(T tempobj) {
        if (pthread_mutex_trylock(&singletonLocker)) {
            return self;
        } else {
            self = new singleton(tempobj);
            return self;
        }   

    }
    void destroy() {
        obj = NULL;
        pthread_mutex_unlock(&singletonLocker);
    }
};
template <typename T> singleton<T> *singleton<T>::self = 0;

class person {
    public:
    std::string name;
    person(std::string newname) {
        name = newname;
    }

    person() {
        name = "deafult";
    }

    friend std::ostream& operator << (std::ostream& os, person const & self) {
    os << self.name;
    return os;
}

};

int main()
{

    person *p1; 
    singleton<person *> *inst = singleton<person *>::instance(p1);
    singleton<person *> *inst2 = singleton<person *>::instance(p1);
    if (inst != inst2) {
        printf("test case failed\n");
        exit(EXIT_FAILURE);
    }
    printf("test passed\n");
    exit(EXIT_SUCCESS);

    
}