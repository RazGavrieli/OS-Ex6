#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
#include <arpa/inet.h>

#include <iostream>

#define PORT 9034
#define IP "127.0.0.1"

int main()
{
    int sock_fd;
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("ERROR");
        return -1;
    }
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(IP);
    serverAddress.sin_port = htons(PORT); 

    // connect to a server
    int sock_client = connect(sock_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (sock_client == -1)
    {
        printf("listen failed");
        close(sock_fd);
        return -1;
    }

    int bytes = 0;
    bool connected = true;
    while (connected) {
        std::string line;
        getline(std::cin, line);
        char *buf = const_cast<char*>(line.c_str());
        if (send(sock_fd, buf, 1024, 0) == -1)
        {
            perror("send");
        }
        printf("trying to recieve..\n");
        if ((bytes = recv(sock_fd, buf, 1024, 0)) != -1)
        {
            if (!bytes)
            {
                connected = 0;
                break;
            }
            printf("recv: %s\n",buf);

        }


}
    close(sock_fd);
    return 0;
}