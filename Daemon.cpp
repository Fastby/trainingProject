#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include "Buffer.h"

using namespace std;

#define MAXLINE 4096
#define serv_PORT 34935
#define SA struct sockaddr
#define LISTENQ 1024

int check_login(char login_input[MAXLINE]);

int check_password(char* password_input); 

int logout();

int calculate(char* expression);

void Parse(char *buff, char *com);

Buffer recvbuf = Buffer();
Buffer sendbuf = Buffer();
char* login = NULL;
char* password = NULL;

int main(int argc, char **argv) {
  fd_set readfds, writefds;
  struct timeval timeout;
  

  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  int listenfd, connfd, n = 0;
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_SET(connfd, &readfds);
  FD_SET(connfd, &writefds);
  struct sockaddr_in servaddr;
  char buff[MAXLINE];
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    perror("socket");
  }
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

  connfd = accept(listenfd, (SA *)NULL, NULL);
  
  while (true) {
    if(connfd = NULL)break;
    int ret = select(connfd + 1, &readfds, &writefds, NULL, &timeout);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {
      printf("Таймаут\n");
    } else {
      
      if (FD_ISSET(connfd, &readfds)) {
        
        while ((n = recv(connfd, buff, MAXLINE, 0)) > 0)
          recvbuf.add_to_buff(buff, n);
        while (recvbuf.get_delim() != NULL) {
          char line[MAXLINE];
          strncpy(line, recvbuf.get_buff(),recvbuf.get_delim() - recvbuf.get_buff());
          line[recvbuf.get_delim() - recvbuf.get_buff()] = '\0';
          cout << line << endl;
          recvbuf.delete_from_buff(strlen(line));
        }
      }
    }
    if (FD_ISSET(connfd, &writefds)) {
      while (sendbuf.get_delim() != NULL) {
        char line[MAXLINE];
        strncpy(line, sendbuf.get_buff(),sendbuf.get_delim() - sendbuf.get_buff());
        line[sendbuf.get_delim() - sendbuf.get_buff()] = '\0';
        int bytes_sent;
        while ((bytes_sent = send(connfd, line, strlen(line), 0)) == 0) {
          cerr << "проблема с отправкой сообщения, повтор" << endl;
        }
        sendbuf.delete_from_buff(strlen(line));
      }
    }
  }

  exit(0);
}

int check_login(char* login_input) {
  login = (char*)realloc(login,strlen(login_input));
  memcpy(login, login_input,strlen(login_input));
}/*Проверяет наличие введенного логина в бд*/

int check_password(char* password_input) {
  password = (char*)realloc(password,strlen(password_input));
  memcpy(password,password_input,strlen(password_input));
} /*Сверяет пароль для введенного логина из бд с введенным паролем*/

int logout() {
  login = NULL;
  password = NULL;
}//Сбрасывает login и password

void Parse(char *buff, char *com, char *val) {//требуется дописать для получения значения команды
  char *space = strchr(buff, ' ');
  if (space != NULL) {
    strncpy(com, buff, space - buff);
    com[space - buff] = '\0';
    strncpy(val,space + 1,strlen(space + 1));
    val[strlen(space+1)] = '\0';
  } else {
    strcpy(com, buff);
    com[strlen(com)] = '\0';
    val[0] = '\0';
  }
}
