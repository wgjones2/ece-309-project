// include/core/message.h
// this file defines the message class, 
//which represents a single message in a conversation. 
//
#pragma once // make sure it is only included once
//include basic libs:
#include <string>

// define roles which given in main.cpp 
enum class Role { // define the role of the message sender
    User, // the message is sent by a user
    Assistant, // the message is sent by an assistant
    System // the message is sent by the system
};

class Message {
    public:
        Message(): role_(Role::System), content_("") {} // default constructor
        Message(Role role, std::string content): role_(role), content_(content) {} // constructor with role and content
        Role role() const { return role_; } // get the role of the message sender
        const std::string& content() const noexcept { return content_; } // get the content of the message
    private:
        Role role_; // the role of the message sender
        std::string content_; // the content of the message
};