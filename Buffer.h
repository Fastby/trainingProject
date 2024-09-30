#pragma once
#include<iostream>


class Buffer{
private: 
    char* buff;
    int buff_size;

public:
    void add_to_buff(char* message, int message_size){
        buff = (char *)realloc(buff, buff_size + message_size);
        memcpy(buff + buff_size, message, message_size);
        buff_size += message_size;
        buff[buff_size] = '\0';
    }

    void delete_from_buff(int message_size){
        char* new_buf = (char*)realloc(buff,buff_size - message_size + 1);
        memcpy(new_buf, buff + message_size + 1, buff_size - message_size);
        buff = new_buf;
        buff_size -= message_size + 1;
        buff[buff_size] = '\0';
    }

    char* get_delim(){
        char* delim = NULL;
        delim = strchr(buff,'\n');
        return delim;
    }

    Buffer(){
        buff = NULL;
        buff_size = 0;
    }

    ~Buffer(){
        free(buff);
    }

    char* get_buff(){
        return buff;
    }

    int get_buff_size(){
        return buff_size;
    }

    char* get_command(){ 
    }
};