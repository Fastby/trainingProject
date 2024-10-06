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

char *get_delim(char *message);

int main() {


  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  struct sockaddr_in servaddr;
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(serv_PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
    cerr << ("Error: inet_pton error for %s", "127.0.0.1");
    exit(1);
  }

  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    cerr << "Error: problem in creating socket" << endl;
    exit(1);
  }

  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
    cerr << ("Error: problem in connecting to the server");
    exit(1);
  }
  cout << "CONNECTED\n";
  fd_set readfds, writefds;
  Buffer sendbuf = Buffer();
  Buffer recvbuf = Buffer();
  while (1) {
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_SET(sockfd, &readfds);
    FD_SET(sockfd, &writefds);
    char buff[MAXLINE + 1];
    int ret = select(sockfd + 1, &readfds, &writefds, NULL, NULL);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {
      printf("Таймаут\n");
    } else {
      if (FD_ISSET(sockfd, &writefds)) {
        cout << "Enter command\n";
        fgets(buff, MAXLINE, stdin);
        sendbuf.add_to_buff(buff, strlen(buff));
        while (get_delim(sendbuf.get_buff()) != NULL) {
          char *delim = get_delim(sendbuf.get_buff());
          char line[MAXLINE];
          strncpy(line, sendbuf.get_buff(), delim - sendbuf.get_buff() + 1);
          line[strlen(line)] = '\0';
          while (send(sockfd, line, strlen(line), 0) < 0) {
            cerr << "проблема с отправкой сообщения, повтор" << endl;
          }
          sendbuf.delete_from_buff(strlen(line));
          memset(line, 0, sizeof(line));
        }
        int n;
        if ((n = recv(sockfd, buff, MAXLINE, 0)) > 0) {
          recvbuf.add_to_buff(buff, n);
          while ((get_delim(recvbuf.get_buff())) != NULL) {
            char *delim = get_delim(recvbuf.get_buff());
            char line[MAXLINE];
            strncpy(line, recvbuf.get_buff(), delim - recvbuf.get_buff());
            line[delim - recvbuf.get_buff()] = '\0';
            cout << line << endl;
            recvbuf.delete_from_buff(strlen(line));
          }
        }
      }
    }
  }
  close(sockfd);
  return 0;
}

char *get_delim(char *message) {
  char *delim = strchr(message, '\n');
  return delim;
}