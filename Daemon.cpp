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
#define serv_PORT 50152
#define SA struct sockaddr
#define LISTENQ 1024
#define FREE_ELEMENT -1

char *get_delim(char *message);

int main(int argc, char **argv) {

  int listenfd;

  //Создание сокета
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0) {
    perror("socket");
    return EXIT_FAILURE;
  }

  struct sockaddr_in servaddr, cliaddr;
  socklen_t clilen = sizeof(cliaddr);
  memset(&servaddr, 0, sizeof(servaddr)); // ← А нахуй вот это?
  //Привязка сокета к адресу и порту
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

  int maxfd = 0;
  int maxi = FREE_ELEMENT;
  int client[FD_SETSIZE];
  for (int i = 0; i < FD_SETSIZE; i++)
    client[i] = FREE_ELEMENT;

  Buffer recvbuf = Buffer();
  Buffer sendbuf = Buffer();
  fd_set connectionSet, allset;
  struct timeval timeout;
  timeout.tv_sec = 10;
  timeout.tv_usec = 0;
  int nready, connfd, sockfd;
  char line[MAXLINE];
  FD_ZERO(&allset);
  while (true) {
    FD_ZERO(&connectionSet);
    FD_SET(listenfd, &connectionSet);
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;


    int ret = select(listenfd + 1, &connectionSet, NULL, NULL, &timeout);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {
      for (int i = 0; i < FD_SETSIZE; i++) {
        if ((sockfd = client[i]) < 0)
          continue;
        if (FD_ISSET(sockfd, &allset)) {
          int n;
          if ((n = read(sockfd, line, MAXLINE)) == 0) {
            close(sockfd);
            FD_CLR(sockfd, &allset);
            client[i] = FREE_ELEMENT;
          } else
            write(sockfd, line, n);
        }
      }
    } else {
        connfd = accept(listenfd, (SA *)&cliaddr, &clilen);
        if (connfd == -1) {
          perror("accept");
          return EXIT_FAILURE;
        }
        cout << "connected with new client!\n";

        for (int i = 0; i < FD_SETSIZE; i++) {
          if (client[i] == FREE_ELEMENT) {
            client[i] = connfd; /*Сохранение дескриптора на последнюю позицию в массиве*/
            if (i > maxi)
              maxi = i;
            break;
          }

          if (i == FD_SETSIZE - 1) {
            cerr << "Error: too many clients\n";
            return EXIT_FAILURE;
          }
        }
        FD_SET(connfd,&allset); /*Добавление нового дескриптора к существующему набору*/

        if (connfd > maxfd)
          maxfd = connfd;
    }
  }
  return 0;
}

char *get_delim(char *message) {
  char *delim = strchr(message, '\n');
  return delim;
}

/*void Parse(char *buff, char *com,char *val) { //требуется дописать для
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