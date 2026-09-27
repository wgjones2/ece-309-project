// include/core/message.h
// this file defines the message class, 
//which represents a single message in a conversation. 
//
#pragma once // make sure it is only included once
//include basic libs:
#include <string> 

class Message {
    public:
        Message();// by default create an empty message
        // for memory allocation. 

        //constuctor:
        Message(Role role, std::string content);
        Role role() const noexcept; // Who sent the message
        const std::string& content() const noexcept; //the content of the message
        // rule of zero is applied here:
        // no other special member functions.
    private:
        Role role_; // the role of the message sender
        std::string content_; // the content of the message
};

