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

    std::string text_to_scan = pending_;
    text_to_scan += chunk;
    