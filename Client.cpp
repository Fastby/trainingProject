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

#define MAXLINE 4096
#define serv_PORT 34935
#define SA struct sockaddr

void Parse(char *buff, char *com);
void add_to_sendbuf(char *message, int message_size);
void add_to_recvbuf(char *message, int message_size);

Buffer recvbuf = Buffer();
Buffer sendbuf = Buffer();
int sendbuf_size = 0;

int main(int argc, char **argv) {
  int sockfd, n;
  fd_set readfds, writefds;

  char command[MAXLINE + 1];
  char buff[MAXLINE + 1];
  char command_values[MAXLINE + 1];

  struct sockaddr_in servaddr;
  struct timeval timeout;

  if (argc != 2) {
    cerr << "Error: <IP address of the server>" << endl;
    exit(1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    cerr << "Error: problem in creating socket" << endl;
    exit(1);
  }
  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_SET(sockfd, &readfds);
  FD_SET(sockfd, &writefds);

  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(argv[1]);
  servaddr.sin_port = htons(serv_PORT);

  if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
    cerr << ("Error: inet_pton error for %s", argv[1]);
    exit(1);
  }

  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
    cerr << ("Error: problem in connecting to the server");
    exit(1);
  }

  while (1) {

    fgets(buff, MAXLINE, stdin);
    add_to_sendbuf(buff, strlen(buff));

    int ret = select(sockfd + 1, &readfds, &writefds, NULL, &timeout);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {
      printf("Таймаут\n");
    } else {
      
      if (FD_ISSET(sockfd, &readfds)) {
        printf("Сокет 1 готов для чтения\n");
        while ((n = recv(sockfd, buff, MAXLINE, 0)) > 0)
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
    if (FD_ISSET(sockfd, &writefds)) {
      printf("Сокет 2 готов для записи\n");
      while (sendbuf.get_delim() != NULL) {
        char line[MAXLINE];
        strncpy(line, sendbuf.get_buff(),sendbuf.get_delim() - sendbuf.get_buff());
        line[sendbuf.get_delim() - sendbuf.get_buff()] = '\0';
        int bytes_sent;
        while ((bytes_sent = send(sockfd, line, strlen(line), 0)) == 0) {
          cerr << "проблема с отправкой сообщения, повтор" << endl;
        }
        sendbuf.delete_from_buff(strlen(line));
      }
    }
  }

  close(sockfd);

  exit(0);
}

void Parse(char *buff, char *com) {
  char *space = strchr(buff, ' ');
  if (space != NULL) {
    strncpy(com, buff, space - buff);
    com[space - buff] = '\0';
  } else {
    strcpy(com, buff);
    com[strlen(com) - 1] = '\0';
  }
}
