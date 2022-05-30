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
#include <ctype.h>

#include "queue.h"
#include "active_object.h"
#include "server.h"


pthread_t thread_id[BACKLOG];
int new_fd[BACKLOG];
int sockfd; 
bool online;
queue* firstQueue;
queue* secondQueue;
queue* thirdQueue;
AO *firstAO;
AO *secondAO; 
AO *thirdAO;


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool isprintable(char* text) {
    for (size_t i = 0; i < strlen(text); i++)
    {
        if (!isprint(text[i])) {
            return false;
        }
    }
    return true;    
}

void *clientThread(void *newfd) {
    int new_fd = *(int*)newfd;
    pthread_detach(pthread_self());
    int numbytes;
    char buf[1024];
    bool connected = true;
    struct query* newTask;
    while (connected&&online) {
        numbytes = recv(new_fd, buf, sizeof(buf), 0);
        if (numbytes <=0) {
            perror("recv");
            connected = false;
            pthread_exit(NULL);
            break;
        }
        *(buf+numbytes) = '\0';
        if (!isprintable(buf)) {
            if (send(new_fd, "bad input", 1024, 0) == -1)  {
                perror("send");
            }
        } else {
            newTask = (struct query*)malloc(sizeof(struct query));
            newTask->fd = new_fd;
            memcpy(newTask->text, buf, sizeof(buf));
            printf("Got a new query: '' %s '' | from file descriptor: %d\n", newTask->text, newTask->fd);
            enQfirst(newTask);
        }
    }
    free(newTask);
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

        close(sockfd);
        for (size_t i = 0; i < BACKLOG; i++)
        {
            pthread_cancel(thread_id[i]);
        }
        printf("socket closed\n");
        destroyAO(firstAO);
        destroyAO(secondAO);
        destroyAO(thirdAO);
        printf("AOs closed\n");
        destroyQ(firstQueue);
        destroyQ(secondQueue);
        destroyQ(thirdQueue);
        printf("Qs closed\n");


        printf("program terminated gracefully");
        exit(EXIT_SUCCESS);
    }

}

void* enQfirst(void* i) {
    enQ(i, firstQueue);
    return i;
}

void* func1(void* i) {
    struct query* currTask = (struct query* )i;
    int len = strlen(currTask->text);
    for (size_t j = 0; j < len; j++)
    {
        if (currTask->text[j]>='a'&&currTask->text[j]<='z') {
            currTask->text[j] = (currTask->text[j]-96)%26+97;
        } 
        else if (currTask->text[j]>='A'&&currTask->text[j]<='Z') {
            currTask->text[j] = (currTask->text[j]-64)%26+65;
        }
    }
    return currTask;
}

void* enQsecond(void* i) {
    enQ(i, secondQueue);
    return i;
}

void* func2(void* i) {
    struct query *currTask = (struct query*)i;
    int len = strlen(currTask->text);
    for (size_t j = 0; j < len; j++)
    {
        if (currTask->text[j]>='a'&&currTask->text[j]<='z') {
            currTask->text[j] -= 32;
        } 
        else if (currTask->text[j]>='A'&&currTask->text[j]<='Z') {
            currTask->text[j] += 32;
        }
    }
    return i;
}

void* enQthird(void* i) {
    enQ(i, thirdQueue);
    return i;
}

void* func3(void* i) {
    // this function is currently used a passthrough for AO number 3. 
    return i;
}

void* send_data(void* i) {
    struct query* completedTask = (struct query*)i;
    int new_fd = completedTask->fd;
    if (send(new_fd, completedTask->text, 1024, 0) == -1)  {
        perror("send");
    }
    printf("sent: %s to file descriptor: %d\n", completedTask->text,new_fd);
    return NULL;
}

int main(void)
{
    firstQueue = createQ();
    secondQueue = createQ();
    thirdQueue = createQ();
    firstAO = newAO(firstQueue, func1,enQsecond);
    secondAO = newAO(secondQueue, func2 , enQthird);
    thirdAO = newAO(thirdQueue, func3, send_data);
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
        // need to fix connection to a new client
        inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_create(&thread_id[j%BACKLOG], NULL, clientThread, &new_fd[j]);
        j++;
    }

    return 0;
}




// int main() {
//     firstQueue = createQ();
//     secondQueue = createQ();
//     thirdQueue = createQ();
//     firstAO = newAO(firstQueue, func1,enQsecond);
//     secondAO = newAO(secondQueue, func2 , enQthird);
//     thirdAO = newAO(thirdQueue, func3, send_data);
//     char* t = "y is it always 8 bytes";
//     for (int i = 0; i < 50; i++) {
//         struct query* newTask = (struct query*)malloc(sizeof(struct query));
//         newTask->fd = i;
//         strcpy(newTask->text, t);
//         enQfirst(newTask);
//         sleep(0.6);
//     }

    
//     sleep(30);
// }