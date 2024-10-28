#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SA struct sockaddr



class FD_Connection{
private:
    struct sockaddr_in servaddr;
    int sockfd;

public:

    FD_Connection(){
        memset(&servaddr,0,sizeof(servaddr));
        sockfd = 0;
    }

    unsigned short int Connect(char* addr, int port){
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(addr);
        servaddr.sin_port = htons(port);

        if (inet_pton(AF_INET, addr, &servaddr.sin_addr) <= 0)
            return EXIT_FAILURE;

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            return EXIT_FAILURE;

        if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0)
            return EXIT_FAILURE;
        
        return EXIT_SUCCESS;
    }

    void closeConnection(){close(sockfd);}

    int getSocket(){return sockfd;}



};