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

int check_login(char login[MAXLINE]);

int check_password(char login[MAXLINE], char* password_input); 

int logout();

int calculate(char* expression);

void Parse(char *buff, char *com);

void add_to_sendbuf(char *message, int message_size);

void add_to_recvbuf(char *message, int message_size);

char *recvbuf = NULL;
char *sendbuf = NULL;
int sendbuf_size = 0, recvbuf_size = 0;

int main(int argc, char **argv) {

  int listenfd, connfd, n = 0;
  struct sockaddr_in servaddr;
  char buff[MAXLINE], login[MAXLINE];
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
    while ((n = recv(connfd, buff, MAXLINE, 0)) > 0) {
      add_to_recvbuf(buff, n);
    }

    while (strchr(recvbuf, '\n') != NULL) { //Дописать, реализовать выбор из буфера команды и вызов функции под команду
      char *delim = strchr(recvbuf, '\n');
      char command[MAXLINE + 1];
      strncpy(buff, recvbuf, delim - recvbuf);
      buff[delim - recvbuf] = '\0';
      int line_size = strlen(buff);
      Parse(buff, command);

      if (strcmp(command, "login") == 0) {
        check_login();

      } else if (strcmp(command, "logout") == 0) {
        logout();

      } else if (strcmp(command, "password") == 0) {
        check_password();

      } else if (strcmp(command, "calc") == 0) {
        calculate();
      }

      char *new_buf = (char *)realloc(recvbuf, recvbuf_size - line_size + 1);
      memcpy(new_buf, recvbuf + line_size + 1, recvbuf_size - line_size);
      recvbuf = new_buf;
      recvbuf_size -= line_size + 1;
      recvbuf[recvbuf_size] = '\0';
    }

    while (strchr(sendbuf, '\0') != NULL) {
      char *delim = strchr(sendbuf, '\0');
      char line[MAXLINE];
      strncpy(line, sendbuf, delim - sendbuf);
      line[delim - sendbuf] = '\0';
      int bytes_sent;
      while ((bytes_sent = send(connfd, line, strlen(line), 0)) == 0) {
        cerr << "проблема с отправкой сообщения, повтор" << endl;
      }
      char *new_buf = (char *)realloc(sendbuf, sendbuf_size - bytes_sent + 1);
      memcpy(new_buf, sendbuf + bytes_sent + 1, sendbuf_size - bytes_sent);
      sendbuf = new_buf;
      sendbuf_size -= bytes_sent + 1;
      sendbuf[sendbuf_size] = '\0';
    }
  }
}

int check_login(char login_input[MAXLINE]) {}/*Проверяет наличие введенного логина в бд*/

int check_password(char login[MAXLINE], char* password_input) {} /*Сверяет пароль для введенного логина из бд с введенным паролем*/

int logout() {}//Сбрасывает login и password

void Parse(char *buff, char *com) {//требуется дописать для получения значения команды
  char *space = strchr(buff, ' ');
  if (space != NULL) {
    strncpy(com, buff, space - buff);
    com[space - buff] = '\0';
  } else {
    strcpy(com, buff);
    com[strlen(com) - 1] = '\0';
  }
}
void add_to_sendbuf(char *message, int message_size) {
  sendbuf = (char *)realloc(sendbuf, sendbuf_size + message_size);
  memcpy(sendbuf + sendbuf_size, message, message_size);
  sendbuf_size += message_size;
  sendbuf[sendbuf_size] = '\0';
}

void add_to_recvbuf(char *message, int message_size) {
  recvbuf = (char *)realloc(recvbuf, recvbuf_size + message_size);
  memcpy(recvbuf + recvbuf_size, message, message_size);
  recvbuf_size += message_size;
  recvbuf[recvbuf_size] = '\0';
}