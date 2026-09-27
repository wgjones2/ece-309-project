// src/conversation.cpp
// this is the implementaation of the conversation
// a diy growable array to store messgaes. 

#include "core/conversation.h"
#include <stdexcept> // for std::out_of_range

//Constuctor:
Conversation::Conversation() : data_(nullptr), size_(0), capacity_(0) {}
//Destructor:
Conversation::~Conversation() { delete[] data_; }
//Copy consructor with deep copy:
Conversation::Conversation(const Conversation& other) : size_(other.size_), capacity_(other.capacity_) {
    if (capacity_ >0) {
        data_ = new Message[capacity_];
        for (std::size_t i = 0; i < size_; ++i) {
            data_[i] = other.data_[i];
        }
    } else {
        data_ = nullptr;
    }
}
//Copy assignment operator with deep copy:
Conversation& Conversation::operator=(const Conversation& other) {
    if (this != &other) {
        delete[] data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        if (capacity_ > 0) {
            data_ = new Message[capacity_];
            for (std::size_t i = 0; i < size_; ++i) {
                data_[i] = other.data_[i];
            }
        } else {
            data_ = nullptr;
        }
    }
    return *this;
}
//Move constructor:
Conversation::Conversation(Conversation&& other) noexcept : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
}
//Move assignment operator:
Conversation& Conversation::operator=(Conversation&& other) noexcept {
    if (this != &other) {
        return *this;
    }
    delete[] data_;
    data_ = other.data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
    return *this;
}
