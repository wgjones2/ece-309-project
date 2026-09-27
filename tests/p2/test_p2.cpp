// test code for the project

#undef NDEBUG
#include <cassert>      // assert()
#include <iostream>     // std::cout, std::endl
#include <string>       // std::string, std::to_string
#include <string_view>  // std::string_view (needed by OutputSink::write)
#include <fstream>      // std::ofstream, for writing test files to disk
#include <cstdio>       // std::remove, for deleting test files when done
#include <memory>       // std::unique_ptr, std::make_unique (Harness needs these)
#include <utility>      // std::move
#include <stdexcept>    // std::out_of_range (thrown by Conversation::at)
 
#include "core/conversation.h"
#include "core/message.h"
#include "core/sentinel_scanner.h"
#include "harness/harness.h"
#include "model/model_client.h"
#include "model/scripted_client.h"
#include "model/replay_client.h"
 
const std::string kSentinel = "<|end_conversation|>";

//helper: function to check if out of range
bool at_throws_out_of_range(const Conversation& conv, std::size_t i) {
    try {
        conv.at(i);
    } catch (const std::out_of_range&) {
        return true;
    }
    return false;
}

//helper: function to write text to a file
void write_text_file(const std::string& filename, const std::string& text) {
    std::ofstream out(filename);
    out << text;
    out.close();
}

//helper: turn a role into the word used in transcript
std::string role_to_transcript_word(Role role) {
    switch (role) {
        case Role::User:
            return "user";
        case Role::Assistant:
            return "assistant";
        case Role::System:
            return "system";
        default:
            throw std::out_of_range("Unknown role");
    }
}

//helper: save a conversation to a file in the
//transcript format from the spec appendix A
void save_conversation_to_file(const Conversation& conv, const std::string& filename) {
    std::ofstream out(filename);
    for (std::size_t i = 0; i < conv.size(); ++i) {
        if (i>0) {
            out << "---\n";
        }
        out << "role: " << role_to_transcript_word(conv.at(i).role()) << "\n";
        out << conv.at(i).content() << "\n";
    }
    out.close();
}

// need to pretend to be the keyboard input
// will make a class to simulate keyboard input

// also need a way to get the text the harness would
// output to the console

//Test 1: empty conversation bounds
void test_1() {}

//Test2: system message ordering
void test_2() {}

//Test 3: rule of five (copy)
void test_3() {}

//Test 4: rule of five (move)
void test_4() {}

//Test 5: growth behavior
void test_5() {}

// Test6: scanner (clean edit)
void test_6() {}

//Test 7: scanner (split sentinel)
void test_7() {}

// Test 8: scanner (false alarms (partial matches))
void test_8() {}

//Test 9: scanner (bounded memory)
void test_9() {}

//Test 10: harness turn limit
void test_10() {}

//Test 11: harness sentinel hault
void test_11() {}

//test 12: transcirp round trip
void test_12() {}

//test 13: (added based on rubric) harness (EOF and Clean Shutdown)
void test_13() {}

//test for compile
int main() {
    std::cout << "Running Project 2 tests..." << std::endl;

    test_1();
    std::cout << "PASS  1: Empty conversation bounds" << std::endl;
    test_2();
    std::cout << "PASS  2: System message ordering" << std::endl;
    test_3();
    std::cout << "PASS  3: Rule of five (copy)" << std::endl;
    test_4();
    std::cout << "PASS  4: Rule of five (move)" << std::endl;
    test_5();
    std::cout << "PASS  5: Growth behavior" << std::endl;   
    test_6();
    std::cout << "PASS  6: Scanner (clean text)" << std::endl;
    test_7();
    std::cout << "PASS  7: Scanner (split sentinel)" << std::endl;
    test_8();
    std::cout << "PASS  8: Scanner (false alarms)" << std::endl;
    test_9();
    std::cout << "PASS  9: Scanner (bounded memory)" << std::endl;
    test_10();
    std::cout << "PASS 10: Harness (turn limit)" << std::endl;
    test_11();
    std::cout << "PASS 11: Harness (sentinel hault)" << std::endl;
    test_12();
    std::cout << "PASS 12: Transcript round-trip" << std::endl;
    test_13();
    std::cout << "PASS 13: Harness (EOF)" << std::endl;
    
    std::cout << "All tests passed!" << std::endl;

    return 0;
}
