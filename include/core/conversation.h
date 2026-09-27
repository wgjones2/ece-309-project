// include/core/conversation.h
// this file defines the conversation class,
//the conversation is a full history of chat
//by the project spec, we have to make our 
// own growable array tostore as on ordered list
// of messages. 

#pragma once // make sure it is only included once
#include "core/message.h"
#include <cstddef> // for size_t

class Conversation {
    public:
        //note: VScodes autocomplete is doing a lot of this unintentionally.
        Conversation(); // default constructor
        ~Conversation(); // destructor
        Conversation(const Conversation& other); // copy constructor
        Conversation& operator=(const Conversation& other); // copy assignment operator
        Conversation(Conversation&& other) noexcept; // move constructor
        Conversation& operator=(Conversation&& other) noexcept; // move assignment operator

        void append(Message message); // append a message to the conversation

        std::size_t size() const noexcept; //get the number of messages in the conversation
        std::size_t capacity() const noexcept; // get the capacity of the conversation
        const Message& at(std::size_t index) const; //check bounds.
        const Message* begin() const noexcept; // get a pointer to the first message
        const Message* end() const noexcept; // get a pointer to one past the last message
    private:
        Message* data_; // pointer to the array of messages
        std::size_t size_; //number of messages in the conversation
        std::size_t capacity_; //capacity of the conversation
};
