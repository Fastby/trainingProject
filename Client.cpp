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

char *recvbuf = NULL;
int recvbuf_size = 0;
char *sendbuf = NULL;
int sendbuf_size = 0;

int main(int argc, char **argv) {
  int sockfd, n;

  char command[MAXLINE + 1];
  char buff[MAXLINE + 1];
  char command_values[MAXLINE + 1];
  struct sockaddr_in servaddr;

  if (argc != 2) {
    cerr << "Error: <IP address of the server>" << endl;
    exit(1);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    cerr << "Error: problem in creating socket" << endl;
    exit(1);
  }

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

    Parse(buff, command);

    if (strcmp(command, "login") == 0) {
      buff[strlen(buff) - 1] = '\0';
      add_to_sendbuf(buff, strlen(buff));

    } else if (strcmp(command, "logout") == 0) {
      buff[strlen(buff) - 1] = '\0';
      add_to_sendbuf(buff, strlen(buff));

    } else if (strcmp(command, "password") == 0) {
      buff[strlen(buff) - 1] = '\0';
      add_to_sendbuf(buff, strlen(buff));

    } else if (strcmp(command, "exit") == 0) {
      break;
    } else {
      printf("Wrong input\n");
    }

    /*
        Циклы для прохода по буферам и
        вывода на экран из буфера receive
        отправки серверу из буфера send
    */
    while (strchr(sendbuf, '\0') != NULL) {
      char *delim = strchr(sendbuf, '\0');
      char line[MAXLINE];
      strncpy(line, sendbuf, delim - sendbuf);
      line[delim - sendbuf] = '\0';
      int bytes_sent;
      while ((bytes_sent = send(sockfd, line, strlen(line), 0)) == 0) {
        cerr << "проблема с отправкой сообщения, повтор" << endl;
      }
      char *new_buf = (char *)realloc(sendbuf, sendbuf_size - bytes_sent + 1);
      memcpy(new_buf, sendbuf + bytes_sent + 1, sendbuf_size - bytes_sent);
      sendbuf = new_buf;
      sendbuf_size -= bytes_sent + 1;
      sendbuf[sendbuf_size] = '\0';
    }

    while ((n = recv(sockfd, buff, MAXLINE, 0)) > 0) {
      add_to_recvbuf(buff, n);
    }
    
    while (strchr(recvbuf, '\n') != NULL) {
      char *delim = strchr(recvbuf, '\n');
      char line[MAXLINE];
      strncpy(line, recvbuf, delim - recvbuf);
      line[delim - recvbuf] = '\0';
      int line_size = strlen(line);
      cout << line << endl;
      char *new_buf = (char *)realloc(recvbuf, recvbuf_size - line_size + 1);
      memcpy(new_buf, recvbuf + line_size + 1, recvbuf_size - line_size);
      recvbuf = new_buf;
      recvbuf_size -= line_size + 1;
      recvbuf[recvbuf_size] = '\0';
    }
  }
  close(sockfd);
  free(recvbuf);
  free(sendbuf);
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