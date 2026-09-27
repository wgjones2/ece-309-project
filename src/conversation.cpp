// src/conversation.cpp
// this is the implementaation of the conversation
// a diy growable array to store messgaes. 
#include "core/conversation.h"
#include <stdexcept> // for std::out_of_range

#define GROWTH_FACTOR 2 // growth factor for the dynamic array

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

//Adding Messages:

void Conversation::append(Message message) {
    if (size_ == capacity_) {// if full
        // grow the array via growth factor defined - 2 for this project
        std::size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * GROWTH_FACTOR; // growth factor of 2
        Message* new_data = new Message[new_capacity]; // allocate new array
        for (std::size_t i = 0; i < size_; ++i) { // copy old data to new array
            new_data[i] = data_[i];
        }
        delete[] data_; //free the old data
        data_ = new_data; //update member variables
        capacity_ = new_capacity; // up
    }
    data_[size_] = message; //add the new message once size is updated if applicable
    ++size_; // increment the size of the conversation
}

//Reading Messages:
//find how many messages are currently stored
std::size_t Conversation::size() const noexcept {
    return size_;
}
// returns how many spaces are allocated for messages
std::size_t Conversation::capacity() const noexcept {
    return capacity_;
}
//Bound check: will throw std::out_of_range if index is out
const Message& Conversation::at(std::size_t i) const {
    if (i >= size_) {
        throw std::out_of_range("Index out of range");
    }
    return data_[i];
}
// get the pointer to first message
//used for range based for loops and iterators
const Message* Conversation::begin() const noexcept {
    return data_;
}
// returns pointer to one past the last message
const Message* Conversation::end() const noexcept {
    if (data_ == nullptr) { // null if no data
        return nullptr;
    }
    return data_ + size_;
}
