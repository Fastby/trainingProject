#pragma once
#include<iostream>
#include<cstring>

#define MAXLINE_BUFF 4096

class Buffer{
private: 
    char buff[MAXLINE_BUFF] = "\0";
public:
    void add_to_buff(char* message, int message_size){
        memcpy(buff + strlen(buff), message, message_size);
    }

    void delete_from_buff(int message_size){
        char new_buf[strlen(buff)-message_size];
        memcpy(new_buf,buff+message_size,strlen(buff)-message_size);
        strcpy(buff,new_buf);
    }

    Buffer(){
    }

    ~Buffer(){
    }

    char* get_buff(){
        return buff;
    }

    int get_buff_size(){
        return strlen(buff);
    }
};