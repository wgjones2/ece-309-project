// src/sentinel_scanner.cpp
// this file implements the sentinel scanner
//which looks for the stop marker in the steam 

#include "core/sentinel_scanner.h"

//Constructor: store thee sentinel string:
SentinelScanner::SentinelScanner(std::string sentinel): sentinel_(sentinel), pending_("") {}

//Process a chunk of text, looking for the sentinel:
SentinelScanner::Out SentinelScanner::feed(std::string_view chunk) {
    Out result;
    result.sentinel_found = false;

    // add chunck to pending text:
    std::string text_to_scan = pending_;
    text_to_scan += chunk;

    std::size_t sentinel_pos = text_to_scan.find(sentinel_);

    if (sentinel_pos != std::string::npos) {
        // Sentinel found, return text up to the sentinel
        result.safe_text = text_to_scan.substr(0, sentinel_pos);
        result.sentinel_found = true;
        pending_.clear(); // Clear pending since we found the sentinel
        return result;
    }

    std::size_t hold_back_size = (sentinel_.empty()) ? 0 : sentinel_.size() - 1;
    
    if (text_to_scan.size() <= hold_back_size) {
        // Not enough text to safely return, hold back
        pending_ = text_to_scan;
    } else {
        std::size_t safe_length = text_to_scan.size() - hold_back_size;
        // Return all but the last hold_back_size characters
        result.safe_text = text_to_scan.substr(0, safe_length);
        pending_ = text_to_scan.substr(safe_length);
    }

    return result;
}

SentinelScanner::Out SentinelScanner::flush() {
    Out result;
    result.safe_text = pending_;
    result.sentinel_found = false;
    pending_.clear();
    return result;
}
