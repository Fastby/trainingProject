#include <arpa/inet.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define MAXLINE 4096
#define serv_PORT 34935
#define SA struct sockaddr
#define LISTENQ 1024

int loginUser(char login[MAXLINE], char password[MAXLINE]);
int logout(int *connfd);
int change_password(char new_password[MAXLINE]);
void handle_client();

int main(int argc, char **argv) {
  int listenfd, connfd;
  struct sockaddr_in servaddr;
  char buff[MAXLINE];
  time_t ticks;
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

  while (true) {
    connfd = accept(listenfd, (SA *)NULL, NULL);
    read(connfd, buff, MAXLINE);
    if (strcmp(buff, "login") == 0) {
      char login[MAXLINE], password[MAXLINE];
      read(connfd, login, MAXLINE);
      read(connfd, password, MAXLINE);
      if (loginUser(login, password) == 1) {
      }
    } else if (strcmp(buff, "logout") == 0) {
      logout(&connfd);
    } else if (strcmp(buff, "password") == 0) {
      char login[MAXLINE], new_password[MAXLINE];
      read(connfd, login, MAXLINE);
      read(connfd, new_password, MAXLINE);
      change_password(new_password);
    }
  }
}

int loginUser(char login[MAXLINE], char password[MAXLINE]) { return 1;}//Временное решение, надо дописать логику проверки логина и пароля

int logout(int *connfd) {
  int result = 0;
  if (close(*connfd) == -1)
    result = 1;
  return result;
}

int change_password(char new_password[MAXLINE]) { return 1; }