/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctime>

#include <iostream>
#include <string>
using namespace std;
#define BACKLOG 10
int sockfd;
#define PORT "1700" // the port client will be connecting to 

#define MAXDATASIZE 1024 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

string gen_random(const int len) {
    static const char alphanum[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}


int main(int argc, char *argv[])
{

    for (size_t i = 0; i < 4; i++)
    {
        if (fork()==0) {
            i = 6;
        }
    }
    int numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);

    freeaddrinfo(servinfo); // all done with this structure
    
    string line = gen_random((10*getpid()+3)%1000);
    char* c = const_cast<char*>(line.c_str());
    printf("SENDING: %s\n", c);
    if (send(sockfd, c, 1024, 0) == -1)  {
        perror("send");
    }

    numbytes = recv(sockfd, buf, sizeof(buf), 0);
    if (numbytes <=0) {
        perror("recv");
        exit(1);
    }
    int len = strlen(c);
    for (size_t j = 0; j < len; j++)
    {
        if (c[j]>='a'&&c[j]<='z') {
            c[j] = (c[j]-96)%26+97;
        } 
        else if (c[j]>='A'&&c[j]<='Z') {
            c[j] = (c[j]-64)%26+65;
        }
    }
    for (size_t j = 0; j < len; j++)
    {
        if (c[j]>='a'&&c[j]<='z') {
            c[j] -= 32;
        } 
        else if (c[j]>='A'&&c[j]<='Z') {
            c[j] += 32;
        }
    }
    if (strcmp(c, buf)) {
        printf("\n\n\t!\t%s != %s\t!\n\n", c, buf);
        throw runtime_error("THE TEST CASE FAILED\n");
    }

    while(true)

    close(sockfd);

    return 0;
}