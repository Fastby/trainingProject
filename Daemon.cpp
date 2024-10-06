#include "Buffer.h"
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
#define serv_PORT 34935
#define SA struct sockaddr
#define LISTENQ 1024
#define FREE_ELEMENT -1


char *get_delim(char *message);

int main(int argc, char **argv) {

  // int listenfd, connfd, n = 0;

  // struct sockaddr_in servaddr;
  // char buff[MAXLINE];
  int listenfd;
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    perror("socket");
  }

  struct sockaddr_in servaddr,cliaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(34935);
  if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
    cerr << "Error: bind error" << endl;
    return 1;
  }

  if (listen(listenfd, LISTENQ) > 0) {
    cerr << "Error: listen error" << endl;
    return 1;
  }

  int maxfd = listenfd;
  int maxi = FREE_ELEMENT;
  int client[FD_SETSIZE];
  for(int i = 0; i < FD_SETSIZE; i++) client[i] = FREE_ELEMENT;

  /*
  int connfd;
  connfd = accept(listenfd, (SA *)NULL, NULL);
  if (connfd == -1) {
    cerr << "Error: accept error\n";
  }
  cout << "connected\n";
  */

  Buffer recvbuf = Buffer();
  Buffer sendbuf = Buffer();
  fd_set rset, allset;
  socklen_t clilen;
  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
  int nready, connfd, sockfd;
  char line[MAXLINE];
  FD_ZERO(&allset);
  FD_SET(listenfd,&allset);

  while (true) {
    rset = allset;
    nready = select(maxfd+1,&rset,NULL,NULL,NULL);

    if(FD_ISSET(listenfd, &rset)){
      clilen = sizeof(cliaddr);
      connfd = accept(listenfd, (SA *)NULL, NULL);
      if (connfd == -1) {
        cerr << "Error: accept error\n";
      }
      cout << "connected with new client!\n";

      for(int i = 0; i < FD_SETSIZE; i++){
        if(i == FD_SETSIZE){
          cerr<<"Error: too many clients\n";
          exit(EXIT_FAILURE);
        }

        if(client[i] < 0){
          client[i] = connfd; /*Сохранение дескриптора на последнюю позицию в массиве*/
          break;
        }

        if(i>maxi)
          maxi = i;
      }
      FD_SET(connfd, &allset); /*Добавление нового дескриптора к существующему набору*/
      
      if(connfd > maxfd)
        maxfd = connfd;

      if(--nready <=0)
        continue;
    }

    for(int i = 0; i <= maxi; i++){
      if((sockfd = client[i])<0)
        continue;
      if(FD_ISSET(sockfd,&rset)){
        int n;
        if((n = read(sockfd,line,MAXLINE)) == 0){
          close(sockfd);
          FD_CLR(sockfd,&allset);
          client[i] = FREE_ELEMENT;
        } else 
          write(sockfd,line,n);

          if(--nready <=0)
            break; /*Больше нет дескрипторов, готовых для чтения*/
      }
    }
/*
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(connfd, &readfds);
    FD_SET(connfd, &writefds);
    int ret = select(connfd + 1, &readfds, &writefds, NULL, &timeout);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {
      printf("Таймаут\n");
    } else {

      if (FD_ISSET(connfd, &readfds)) {
        char buff[MAXLINE + 1];
        int n = recv(connfd, buff, MAXLINE, 0);
        if (n > 0) {
          recvbuf.add_to_buff(buff, n);
          while (get_delim(recvbuf.get_buff()) != NULL) {
            char line[MAXLINE];
            strncpy(line, recvbuf.get_buff(),get_delim(recvbuf.get_buff()) - recvbuf.get_buff() + 1);
            cout <<"got message: "<< line;
            sendbuf.add_to_buff(line, strlen(line));
            recvbuf.delete_from_buff(strlen(line));
            memset(line,0,sizeof(line));
            memset(buff,0,sizeof(buff));
          }
        }
      }
      if (FD_ISSET(connfd, &writefds)) {
        while (get_delim(sendbuf.get_buff()) != NULL) {
          char line[MAXLINE];
          strncpy(line, sendbuf.get_buff(),get_delim(sendbuf.get_buff()) - sendbuf.get_buff()+1);
          send(connfd, line,strlen(line),0);
          sendbuf.delete_from_buff(strlen(line));
          memset(line,0,sizeof(line));
        }
      }
    }
*/
  }
//  close(connfd);
  return 0;
}

char *get_delim(char *message){
    char *delim = strchr(message, '\n');
    return delim;
  }
  /* int check_login(char* login_input) {
    login = (char*)realloc(login,strlen(login_input));
    memcpy(login, login_input,strlen(login_input));
  }
  
   int check_password(char* password_input) {
    password = (char*)realloc(password,strlen(password_input));
    memcpy(password,password_input,strlen(password_input));
  } 
  
   int logout() {
    login = NULL;
    password = NULL;
  }*/

  /*void Parse(char *buff, char *com,char *val) { //требуется дописать для получения значения команды
    char *space = strchr(buff, ' ');
    if (space != NULL) {
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