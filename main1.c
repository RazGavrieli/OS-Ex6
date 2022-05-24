#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h> // threads

#include "queue.h"
#include "active_object.h"
#include "server.h"

#define nullptr ((void*)0)


#define PORT "1500"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold
pthread_t thread_id[BACKLOG];
int new_fd[BACKLOG];
int sockfd; 
bool online;
queue* inputQueue;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void *clientThread(void *newfd) {
    int new_fd = *(int*)newfd;
    pthread_detach(pthread_self());
    int numbytes;
    char buf[1024];
    bool connected = true;
    while (connected&&online) {
        numbytes = recv(new_fd, buf, sizeof(buf), 0);
        if (numbytes <=0) {
            perror("recv");
            connected = false;
            pthread_exit(NULL);
            break;
        }
        //*(buf+numbytes) = '\0';
        printf("got %s from %d\n", buf, new_fd);
        struct query* newTask = (struct query*)malloc(sizeof(struct query));
        newTask->fd = new_fd;
        memcpy(newTask->text, buf, sizeof(buf));
        printf("data is now in a struct! text is %s and fd is %d\n", newTask->text, newTask->fd);

        enQ(newTask, inputQueue);
    }
    close(new_fd);
}

void sig_handler(int signum)
{
    if (signum == SIGINT) {
        printf("program terminating\n");
        online = false;
        for (size_t i = 0; i < BACKLOG; i++)
        {
            close(new_fd[i]);
        }

        destroyQ(inputQueue);

        close(sockfd);
        printf("program terminated gracefully");
        exit(EXIT_SUCCESS);
    }

}

int main(void)
{
    inputQueue = createQ();
    AO *firstAO = newAO(inputQueue, 0/*a func that does encripytion*/,0/* a func that is enQing into the second Q */);
    // AO *secondAO = newAO(secondQueue,  flip caps lock , a func that is enQing into the third Q);
    // AO *thisAO = newAO(thirdQueue, send data to the fd, null or a func that just passes the value (only returns it));
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;

    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    int j = 0;
    signal (SIGINT,sig_handler);
    online = true;

        while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd[j] = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd[j] == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_create(&thread_id[j%BACKLOG], NULL, clientThread, &new_fd[j]);
        j++;
    }

    return 0;
}




// int main() {
//     queue* thisQ = createQ();
//     // for (int i = 0; i < 10; i++)
//     //     enQ(&i, thisQ);

//     AO *ao = newAO(thisQ, func_example_1, func_example_2);
    
//     sleep(1);
//     destroyAO(ao);
//     destroyQ(thisQ);
//     sleep(1);
// }