#pragma once
#include <sys/socket.h>
#include <iostream>
#include <netinet/in.h>
#include <string.h>

#define serv_PORT 50152
#define LISTENQ 1024
#define SA struct sockaddr

class FD_Listener{
private:
    struct sockaddr_in servaddr;
    int listenfd;
    void initializeSocket(){
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd < 0) {
          throw "Error: creating socket";
        }
    }

    void bindSocket(){
        if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
          throw "Error: bind error";
        }
        if (listen(listenfd, LISTENQ) < 0) {
          throw "Error: listen";
        }
    }

public:

    int InitializeSocket(){
        try{
            initializeSocket();
            return EXIT_SUCCESS;
        }
        catch(char* exc){
            return EXIT_FAILURE;
        }
    }
    
    int* getFD(){return &listenfd;}

    int BindSocket(){
        try{
            bindSocket();
            return EXIT_SUCCESS;
        }
        catch(char* exc){
            return EXIT_FAILURE;
        }
    }

    FD_Listener(){
        listenfd = -1;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(serv_PORT);
    }


};