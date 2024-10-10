#pragma once
#include<iostream>
#include<cstring>
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
        if(buff_size <= message_size){
            buff = (char*)realloc(buff,sizeof(char));
            memset(buff,0,sizeof(buff));
            buff_size = 0;
        }
        else{
            char* new_buf = (char*)malloc(buff_size - message_size);
            memcpy(new_buf, buff + message_size, buff_size - message_size);
            free(buff);
            buff = (char*)malloc(buff_size - message_size);
            buff = new_buf;
            buff_size -= message_size;
            buff[buff_size] = '\0';
            free(new_buf);
        }
        
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
};