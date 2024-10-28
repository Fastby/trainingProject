#pragma once
#include <cstdlib>

#define MAXLINE 100


template<typename type>
class Message{

private:
    type line[MAXLINE];
    size_t sizeOfLine;

public:
    void setLine(type input[MAXLINE], size_t inputSize){
        for(size_t i = 0; i < inputSize; i++){
            line[i] = input[i];
        }
        sizeOfLine = inputSize;
    }

    type* getLine(){return line;}
    size_t getSizeOfLine(){return sizeOfLine;}

    Message(){sizeOfLine = 0;}

    ~Message(){}

};