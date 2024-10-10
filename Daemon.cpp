#include "headers/Buffer.h"
#include "headers/ClientSet.h"
#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define MAXLINE 100
#define serv_PORT 50152
#define SA struct sockaddr
#define LISTENQ 1024
#define FREE_ELEMENT -1

void acceptConnection(int* connfd, ClientSet* clientset, int* listenfd);
void readFD(int sockfd, int pos, ClientSet* cliset);

int main(int argc, char **argv) {

  int listenfd;

  
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }

  struct sockaddr_in servaddr, cliaddr;
  socklen_t clilen = sizeof(cliaddr);
  memset(&servaddr, 0, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(serv_PORT);
  if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    return EXIT_FAILURE;
  }

  if (listen(listenfd, LISTENQ) < 0) {
    perror("listen");
    return EXIT_FAILURE;
  }

  Buffer recvbuf = Buffer();
  Buffer sendbuf = Buffer();
  ClientSet cliset = ClientSet();
  fd_set connectionSet, allset;
  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;
  int connfd, sockfd;
  while (true) {
    FD_ZERO(&connectionSet);
    FD_SET(listenfd, &connectionSet);
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    int ret = select(listenfd + 1, &connectionSet, NULL, NULL, &timeout);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {

      for (int i = 0; i < cliset.getMaxi() + 1; i++) 
        readFD(sockfd, i, &cliset);
      
    } else {
        acceptConnection(&connfd,&cliset,&listenfd);
    }
  }
  return 0;
}


void acceptConnection(int* connfd, ClientSet* clientset, int* listenfd){
  *connfd = accept(*listenfd, (SA *)NULL, NULL);
  if (*connfd == -1) {
    throw "error: accept";
  }
  cout << "connected with new client!\n";
  try{
    (*clientset).addClient(*connfd);
  }
  catch(const char* error_message){
    cout<<error_message<<endl;
  }
}

void readFD(int sockfd, int pos, ClientSet* cliset){
  if ((sockfd = (*cliset).getFD(pos)) < 0)
      return;
    if (FD_ISSET(sockfd, (*cliset).getSet())) {
      int n;
      char line[MAXLINE];
      if ((n = read(sockfd, line, MAXLINE)) == 0) {
        close(sockfd);
        (*cliset).deleteClient(pos);
      } else
        write(sockfd, line, n);
    }
}

/*
char *get_delim(char *message) {
  char *delim = strchr(message, '\n');
  return delim;
}

void Parse(char *buff, char *com,char *val) { //требуется дописать для
получения значения команды char *space = strchr(buff, ' '); if (space != NULL) {
    strncpy(com, buff, space - buff);
    com[space - buff] = '\0';
    strncpy(val, space + 1, strlen(space + 1));
    val[strlen(space + 1)] = '\0';
  } else {
    strcpy(com, buff);
    com[strlen(com)] = '\0';
    val[0] = '\0';
  }
}
*/