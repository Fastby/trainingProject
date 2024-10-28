#pragma once
#include "Message.h"
#include <cstdlib>
#include<cstring>
#include <vector>

template<typename type>

class Buffer{
private: 
    std::vector<Message<type>> buffOfMessages;


public:
    void addMessage(type message[], size_t sizeOfMessage){
        Message<type> newMessage;
        newMessage.setLine(message, sizeOfMessage);
        buffOfMessages.push_back(newMessage);
        newMessage.~Message();
    }

    void deleteFirstMessage(){
        if(buffOfMessages.empty() == false)
            buffOfMessages.erase(buffOfMessages.begin());
    }

    Message<type>* getFirstMessage(){
        if(buffOfMessages.empty() == false){
            return &buffOfMessages[0];
        } else return nullptr;
    }

    size_t getLength(){
        return buffOfMessages.size();
    }

};



