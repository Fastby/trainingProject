#pragma once
#include "sys/socket.h"
#include <stdio.h>
#include <unistd.h>

#define FREE_ELEMENT -1
#define MAXLINE_BUF 1000

class ClientSet {
private:
  int clients[FD_SETSIZE];
  fd_set clientset;
  int maxi, maxfd;

public:
  void addClient(int connfd) {
    for (int i = 0; i < FD_SETSIZE; i++) {
      if (clients[i] == FREE_ELEMENT) {
        clients[i] =
            connfd; /*Сохранение дескриптора на последнюю позицию в массиве*/
        if (i > maxi)
          maxi = i;
        break;
      }

      if (i >= FD_SETSIZE - 1) {
        throw "too many clients";
      }
    }
    FD_SET(connfd,
           &clientset); /*Добавление нового дескриптора к существующему набору*/

    if (connfd > maxfd)
      maxfd = connfd;
  }

  void deleteClient(int pos) { clients[pos] = FREE_ELEMENT; }

  int getFD(int pos) { return clients[pos]; }

  int getMaxi(){return maxi;}

  fd_set *getSet() { return &clientset; }

  ClientSet() {
    maxi = 0;
    for (int i = 0; i < FD_SETSIZE; i++)
      clients[i] = FREE_ELEMENT;
    maxfd = 0;
    FD_ZERO(&clientset);
  }

  ~ClientSet() {}
};