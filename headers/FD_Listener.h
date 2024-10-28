#pragma once
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>

#define serv_PORT 50152
#define LISTENQ 1024
#define SA struct sockaddr

class FD_Listener{
private:
    struct sockaddr_in servaddr;
    int listenfd;
    

public:

    unsigned short int initializeSocket(){
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd < 0) {
          perror("Failed to create socket\n");
          return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    unsigned short int bindSocket(){
      if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0 || listen(listenfd, LISTENQ) < 0) {
        return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
    }

    int* getFD(){return &listenfd;}

    FD_Listener(){
        listenfd = -1;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(serv_PORT);
    }


};