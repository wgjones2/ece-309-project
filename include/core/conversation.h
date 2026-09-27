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
        // will double check.

        //special member functions: rule of five used here
        Conversation(); // default constructor
        ~Conversation(); // destructor
        Conversation(const Conversation& other); // copy constructor
        Conversation& operator=(const Conversation& other); // copy assignment operator
        Conversation(Conversation&& other) noexcept; // move constructor
        Conversation& operator=(Conversation&& other) noexcept; // move assignment operator

        void append(Message message); // append a message to the conversation list

        // get the numver of messages and the capacity of the conversation
        std::size_t size() const noexcept; //get the number of messages in the conversation
        std::size_t capacity() const noexcept; // get the capacity of the conversation
        
        //Bound check: will throw std::out_of_range if index is out
        const Message& at(std::size_t index) const; //check bounds.
        
        // get pointers for first and last message in the conversation, for iteration
        const Message* begin() const noexcept; // get a pointer to the first message
        const Message* end() const noexcept; // get a pointer to one past the last message
    private:
        Message* data_ = nullptr; // pointer to the array of messages
        std::size_t size_ = 0; //number of messages in the conversation
        std::size_t capacity_ = 0; //capacity of the conversation
};
