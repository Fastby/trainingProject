#include "../headers/Buffer.h"
#include "../headers/ClientSet.h"
#include "../headers/FD_Listener.h"
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

#define MAXLINE 1024
#define FREE_ELEMENT -1

void acceptConnection(int* connfd, ClientSet* clientset, int* listenfd);
int readFD(int sockfd, int pos, ClientSet* cliset);

int main(int argc, char **argv) {
  FD_Listener listener = FD_Listener();

  listener.InitializeSocket();

  int flags = fcntl(*listener.getFD(), F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl");
    return EXIT_FAILURE;
  }

  flags |= O_NONBLOCK;
  if (fcntl(*listener.getFD(), F_SETFL, flags) == -1) {
    perror("fcntl");
    return EXIT_FAILURE;
  }

  struct sockaddr_in  cliaddr;
  socklen_t clilen = sizeof(cliaddr);
  
  listener.BindSocket();

  ClientSet cliset = ClientSet();
  fd_set connectionSet, allset;
  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  int connfd, sockfd;

  while (true) {
    FD_ZERO(&connectionSet);
    FD_SET(*listener.getFD(), &connectionSet);

    int ret = select(*listener.getFD() + 1, &connectionSet, NULL, NULL, &timeout);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {}
    else {
      if (FD_ISSET(*listener.getFD(), &connectionSet)) {
        acceptConnection(&connfd, &cliset, (int*) listener.getFD());
      }
    }

    FD_ZERO(&allset);
    for (int i = 0; i <= cliset.getMaxi(); i++) {
      int sockfd = cliset.getFD(i);
      if (sockfd >= 0) {
        FD_SET(sockfd, &allset);
      }
    }

    ret = select(cliset.getMaxFD() + 1, &allset, NULL, NULL, &timeout);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {} 
    else {
      for (int i = 0; i <= cliset.getMaxi(); i++) {
        int sockfd = cliset.getFD(i);
        if (sockfd >= 0 && FD_ISSET(sockfd, &allset)) {
          int bytes_read = readFD(sockfd, i, &cliset);
          if (bytes_read < 0) {
            
            if (errno == EWOULDBLOCK) {
              continue;
            } else {
              perror("readFD");
              exit(1);
            }

          } else if (bytes_read == 0) {
            close(sockfd);
            cliset.deleteClient(i);
          }
        }
      }
    }
  }

  return 0;
}


int readFD(int sockfd, int pos, ClientSet* cliset) {
  char buffer[MAXLINE];
  int bytes_read = recv(sockfd, buffer, MAXLINE, 0);
  if (bytes_read < 0) {
    return bytes_read;
  } else {
    write(sockfd,buffer,bytes_read);
  }
  return bytes_read;
}

void acceptConnection(int* connfd, ClientSet* clientset, int* listenfd){
  *connfd = accept(*listenfd, (SA *)NULL, NULL);
  if (*connfd == -1) {
    throw "error: accept";
  }
  try{
    (*clientset).addClient(*connfd);
  }
  catch(const char* error_message){
    cout<<error_message<<endl;
  }
}