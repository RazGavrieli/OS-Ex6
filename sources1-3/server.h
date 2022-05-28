struct query {
    char text[1024];
    int fd;
};

void* enQfirst(void* );
void* func1(void* );
void* enQsecond(void* );
void* func2(void* );
void* enQthird(void* );
void* send_data(void* );

#define nullptr ((void*)0)
#define PORT "1700"  // the port users will be connecting to
#define BACKLOG 30   // how many pending connections queue will hold