// include/core/sentinel_scanner.h
// this fiel defines the sentinal scanner class
//which finds the stop marker in a steam of text
// that arrives in pieces or chunks of anny size
// this is because AI models work in tokens,
// and pieces of data do not arrive in a single chunk
//
//we are looking for <|end_conversation|>

#pragma once // make sure it is only included once
#include <string>
#include <cstdef>
#include <string_view>

class SentinelScanner {
    public:
        // takes in the string to look for the end marker
        SentinelScanner(std::string sentinel);

        //define what feed and flush will return:
        struct Out {
            std::string normal_text;
            bool sentinel_found;
        };
        Out feed(std::string_view chunk);
        Out flush();
    private:
        std::string sentinel_;
}