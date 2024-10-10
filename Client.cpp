#include "headers/Buffer.h"
#include "headers/FD_Connection.h"
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
#define SERV_PORT 50152
#define SA struct sockaddr

char *get_delim(char *message);

int main() {


  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;

  FD_Connection con = FD_Connection();
  
  try{
    con.Connect("127.0.0.1", SERV_PORT);
  } catch(char* error_message){
    cout<<error_message<<endl;
    return EXIT_FAILURE;
  }
  
  fd_set writefds;
  Buffer sendbuf = Buffer();
  Buffer recvbuf = Buffer();

  while (1) {
    FD_ZERO(&writefds);
    FD_SET(con.getSocket(), &writefds);
    char buff[MAXLINE + 1];
    int ret = select(con.getSocket() + 1, NULL, &writefds, NULL, NULL);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {
      printf("Таймаут\n");
    } else {
      if (FD_ISSET(con.getSocket(), &writefds)) {
        cout << "Enter command\n";
        fgets(buff, MAXLINE, stdin);
        sendbuf.add_to_buff(buff, strlen(buff));
        while (get_delim(sendbuf.get_buff()) != NULL) {
          char *delim = get_delim(sendbuf.get_buff());
          char line[MAXLINE];
          strncpy(line, sendbuf.get_buff(), delim - sendbuf.get_buff() + 1);
          line[strlen(line)] = '\0';
          while (send(con.getSocket(), line, strlen(line), 0) < 0) {
            cerr << "проблема с отправкой сообщения, повтор" << endl;
          }
          sendbuf.delete_from_buff(strlen(line));
          memset(line, 0, sizeof(line));
        }
        int n;
        if ((n = recv(con.getSocket(), buff, MAXLINE, 0)) > 0) {
          recvbuf.add_to_buff(buff, n);
          while ((get_delim(recvbuf.get_buff())) != NULL) {
            char *delim = get_delim(recvbuf.get_buff());
            char line[MAXLINE];
            strncpy(line, recvbuf.get_buff(), delim - recvbuf.get_buff()+1);
            cout << line << endl;
            recvbuf.delete_from_buff(strlen(line));
            memset(line, 0, sizeof(line));
          }
        }
      }
    }
  }
  
  return 0;
}

char *get_delim(char *message) {
  char *delim = strchr(message, '\n');
  return delim;
}