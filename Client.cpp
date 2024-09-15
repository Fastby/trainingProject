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

void Parse(char* buff, char* com, char* value);

int main(int argc, char **argv) {
  int sockfd, n;
  char readbuf[MAXLINE + 1];
  char sendbuf[MAXLINE + 1];
  char command[MAXLINE+1];
  char command_values[MAXLINE+1];
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

  int send_buf_size = MAXLINE;
  setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &send_buf_size,
             sizeof(send_buf_size)); //для буферизации
  int receive_buf_size = MAXLINE;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &receive_buf_size,
             sizeof(receive_buf_size));

  while (1) {
    fgets(sendbuf, MAXLINE, stdin);
    Parse(sendbuf,command,command_values);
    sendbuf[strlen(sendbuf) - 1] = '\0';

    if (strcmp(command, "login") == 0) {
      sendbuf[strlen(sendbuf) - 1] = '\0';
      write(sockfd, sendbuf, strlen(sendbuf));

    } else if (strcmp(command, "logout") == 0) {
      write(sockfd, command, strlen(command));
      
    } else if (strcmp(command, "password") == 0) {

      sendbuf[strlen(sendbuf) - 1] = '\0';
      write(sockfd, sendbuf, strlen(sendbuf));

    } else if (strcmp(command, "exit") == 0) {
      break;
    } else {
      printf("Wrong input\n");
    }
  }

  exit(0);
}

void Parse(char* buff, char* com, char* value){
  sscanf(buff, "%s %s", com, value);
}
