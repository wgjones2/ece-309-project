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
#include <cstddef>
#include <string_view>

// given from spec:
class SentinelScanner {
    public:
        // takes in the string to look for the end marker
        explicit SentinelScanner(std::string sentinel);

        //define what feed and flush will return:
        struct Out {
            std::string safe_text;
            bool sentinel_found;
        };
        Out feed(std::string_view chunk);
        Out flush();
        std::size_t pending_size() const noexcept;
    private:
        std::string sentinel_;
        std::string pending_;
};