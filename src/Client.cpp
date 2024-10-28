#include <Buffer.h>
#include <FD_Connection.h>
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

template<typename type>
bool sendMessageToServer(Buffer<type> *sendbuf, FD_Connection *con);

template<typename type>
int getMessageFromServer(Buffer<type> *recvbuf, FD_Connection *con);

template<typename type>
void writeFirstMessage(Buffer<type> *buffer);

void readMessage(Buffer<char> *buffer);


int main() {

  char addr[] = "127.0.0.1";
  FD_Connection con = FD_Connection();

  if(con.Connect(addr, SERV_PORT) == EXIT_FAILURE) return EXIT_FAILURE;
  
  
  fd_set writefds;
  Buffer<char> sendbuf = Buffer<char>();
  Buffer<char> recvbuf = Buffer<char>();

  while (1) {
    FD_ZERO(&writefds);
    FD_SET(con.getSocket(), &writefds);
    
    int ret = select(con.getSocket() + 1, NULL, &writefds, NULL, NULL);
    if (ret == -1) {
      perror("select");
    } else if (ret == 0) {
      printf("Таймаут\n");
    } else {
      if (FD_ISSET(con.getSocket(), &writefds)) {
        readMessage(&sendbuf);
        sendMessageToServer(&sendbuf, &con);
        sendbuf.deleteFirstMessage();
        getMessageFromServer(&recvbuf, &con);
        writeFirstMessage(&recvbuf);
        recvbuf.deleteFirstMessage();
      }
    }
  }
  
  return 0;
}

template<typename type>
bool sendMessageToServer(Buffer<type> *sendbuf, FD_Connection *con){ //Возвращает количество успешно отправленных символов
  if(sendbuf->getLength() > 0 && send(con->getSocket(), sendbuf->getFirstMessage()->getLine(), sendbuf->getFirstMessage()->getSizeOfLine(), 0)>-1){
    return EXIT_SUCCESS;
  } else return EXIT_FAILURE;
}

template<typename type>
int getMessageFromServer(Buffer<type> *recvbuf, FD_Connection *con){//Возвращает количество символов в успешно полученном 
  type buff[MAXLINE];
  int n;
  if ((n = recv(con->getSocket(), buff, MAXLINE, 0)) > -1) {
    recvbuf->addMessage(buff,n);
  } 
  return n;
}

template<typename type>
void writeFirstMessage(Buffer<type> *buffer){
  type line[MAXLINE];
  memcpy(line, buffer->getFirstMessage()->getLine(), buffer->getFirstMessage()->getSizeOfLine() * sizeof(type));
  cout<<line<<endl;
}


void readMessage(Buffer<char> *buffer){
  char line[MAXLINE];
  fgets(line,MAXLINE,stdin);
  buffer->addMessage(line,strlen(line));
}
