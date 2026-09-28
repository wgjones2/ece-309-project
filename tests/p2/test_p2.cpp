// tests/p2/test_p2.cpp
// test code for the project 2

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
 
//changed for "4MB stream one byte at a time" stress test
#define STREAM_BYTES (4 * 1024 * 1024)

const std::string Sentinel_Test_Value = "<|end_conversation|>";

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
    //open the file for writing, makes it if it doesnt exist
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
        // put a --- divider between messages but not before the first one
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
class ConsoleInputSimulator : public InputSource {
    public:
        //constructor:
        ConsoleInputSimulator() {
            // start with zeroed values
            count = 0;
            next = 0;
            eof_ = false;
        }
        // add a line to the simulated input
        void add_line(const std::string& line) {
            //only room for 10 lines, crash if we go over
            assert(count < 10);
            lines_[count] = line;
            ++count;
        }

        // get the next line of input
        std::string read_line() override {
            // no lines left so act like the user hit ctrl-d
            if (next >= count) {
                eof_ = true;
                return "";
            }
            std::string line = lines_[next];
            ++next;
            return line;
        }
        // check if end of input has been reached
        bool is_eof() const override {
            return eof_;
        }
    
    // initialization of private members
    private:
        int count;
        int next;
        bool eof_;
        std::string lines_[10];
};

// also need a way to get the text the harness would
// output to the console
class ConsoleOutputSimulator : public OutputSink {
    public:
        void write(std::string_view text) override {
            //just keep tacking the text onto one big string
            all_text += text;
        }
        std::string all_text;
};

//Test 1: empty conversation bounds
void test_1() {
    Conversation conv;
    assert(conv.size() == 0);
    // begin and end are the same spot when theres nothing in it
    assert(conv.begin() == conv.end());
    assert(at_throws_out_of_range(conv, 0));
    assert(at_throws_out_of_range(conv, 5));
}

//Test 2: system message ordering
void test_2() {
    Conversation conv;
    conv.append(Message(Role::System, "System message"));

    for (int i = 0; i < 20; ++i) {
        //even i is a user msg, odd i is assistant so they take turns
        if (i % 2 == 0) {
            conv.append(Message(Role::User, "User message"));
        } else {
            conv.append(Message(Role::Assistant, "Assistant message"));
        }
        // still pinned at the front after every append / regrow
        assert(conv.at(0).role() == Role::System);
        assert(conv.at(0).content() == "System message");
    }
    assert(conv.size() == 21);

    // only one system message
    for (std::size_t i = 1; i < conv.size(); ++i) {
        assert(conv.at(i).role() != Role::System);
    }
    bool first = true;
    for (const Message& msg : conv) {
        if (first) {
            assert(msg.role() == Role::System);
            first = false;
        } else {
            assert(msg.role() != Role::System);
        }
    }
    // make a copy and a moved version, system msg should still be first in both
    Conversation copy(conv);
    assert(copy.at(0).role() == Role::System);
    Conversation moved(std::move(copy));
    assert(moved.at(0).role() == Role::System);
    assert(moved.at(0).content() == "System message");
}

//Test 3: rule of five (copy)
void test_3() {
    Conversation original;
    original.append(Message(Role::User, "hello1"));
    original.append(Message(Role::Assistant, "hello2"));

    // copy constructor:
    Conversation copy (original);
    //different addresses means the copy got its own memory
    assert(copy.begin() != original.begin());
    assert(copy.size() == original.size());
    for (std::size_t i = 0; i < copy.size(); ++i) {
        assert(copy.at(i).role() == original.at(i).role());
        assert(copy.at(i).content() == original.at(i).content());
    }

    // adding to the copy shouldnt change the original
    copy.append(Message(Role::User, "hello copy side"));
    assert(copy.size() == 3);
    assert(original.size() == 2);

    //copy assignment operator:
    Conversation assigned;
    assigned.append(Message(Role::User, "hello assigned side"));
    assigned = original;
    assert(assigned.begin() != original.begin());
    assert(assigned.size() == 2);
    assert(assigned.at(0).content() == "hello1");
    assert(assigned.at(1).content() == "hello2");
}

//Test 4: rule of five (move)
void test_4() {
    Conversation original;
    original.append(Message(Role::User, "hello1"));
    original.append(Message(Role::Assistant, "hello2"));

    // Remember where original's array lives in memory.
    const Message* old_array = original.begin();

    // move constructor:
    Conversation moved (std::move(original));
    assert(moved.begin() == old_array);
    assert(moved.size() == 2);
    assert(moved.at(0).content() == "hello1");
    // original should be in a valid but empty state
    assert(original.size() == 0);
    assert(original.capacity() == 0);
    assert(original.begin() == nullptr);

    // move assignment operator:
    Conversation target;
    target.append(Message(Role::User, "hello target side"));
    //std::move hands over the memory instead of copying it
    target = std::move(moved);
    assert(target.begin() == old_array);
    assert(target.size() == 2);
    assert(moved.size() == 0);
    assert(moved.capacity() == 0);
    assert(moved.begin() == nullptr);

    // original should still work after being moved from
    original.append(Message(Role::User, "hello reused"));
    assert(original.size() == 1);
    assert(original.at(0).content() == "hello reused");
}

//Test 5: growth behavior
void test_5() {
    Conversation conv;
    assert(conv.capacity() == 0);
    
    std::size_t expected_capacity = 0;
    
    for (std::size_t i = 0; i < 100; ++i) {
        conv.append(Message(Role::User, std::to_string(i)));
        //ran out of room so capacity should double (0 goes to 1)
        if (conv.size() > expected_capacity) {
            if (expected_capacity == 0) {
                expected_capacity = 1;
            } else {
                expected_capacity *= 2;
            }
        }
        assert(conv.size() == i + 1);
        assert(conv.capacity() == expected_capacity);
    }
    assert(conv.capacity() == 128); // 128 after 100 appends - smallest power of 2 >= 100

    for (std::size_t i= 0; i< conv.size(); ++i) {
        assert(conv.at(i).content() == std::to_string(i));
    }
    assert(at_throws_out_of_range(conv, 100));
}

// Test6: scanner (clean text)
void test_6() {
    SentinelScanner scanner(Sentinel_Test_Value);
    std::string input = "some test input without a stop marker";
    // feed gives back text thats safe to print, flush gives whatever was held back
    SentinelScanner::Out out1= scanner.feed(input);
    SentinelScanner::Out out2 = scanner.flush();

    assert(out1.sentinel_found == false);
    assert(out2.sentinel_found == false);

    assert(out1.safe_text + out2.safe_text == input);
}

//Test 7: scanner (split sentinel)
// copied from spec
void test_7() {
    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "Goodbye." + sentinel;
    //try cutting the text at every possible spot
    for (std::size_t split = 0; split <= text.size(); ++split) {
        SentinelScanner scanner(sentinel);
        SentinelScanner::Out out1 = scanner.feed(text.substr(0, split));
        SentinelScanner::Out out2 = scanner.feed(text.substr(split));
        assert((out1.sentinel_found || out2.sentinel_found) &&
               "sentinel must be caught regardless of split point");
        assert(out1.safe_text + out2.safe_text == "Goodbye.");
    }
}

// Test 8: scanner (false alarms (partial matches))
void test_8() {
    std::string text = "A <|end_nothing|> B <|end_conversation| C <|end_";
    SentinelScanner scanner(Sentinel_Test_Value);
    std::string safe_text = "";
    // feed it one character at a time
    for (std::size_t i = 0; i < text.size(); ++i) {
        SentinelScanner::Out out = scanner.feed(text.substr(i, 1));
        assert(out.sentinel_found == false);
        safe_text += out.safe_text;
    }

    SentinelScanner::Out out = scanner.flush();
    assert(out.sentinel_found == false);
    safe_text += out.safe_text;

    assert(safe_text == text);
}

//Test 9: scanner (bounded memory)
void test_9() {
    SentinelScanner scanner(Sentinel_Test_Value);
    std::size_t max_allowed = Sentinel_Test_Value.size() - 1;
    std::string piece = "<|end_";
    std::size_t total_emitted = 0;
    for (std::size_t i = 0; i < STREAM_BYTES; ++i) {
        char c = piece[i % piece.size()]; // cycle through the piece
        SentinelScanner::Out out = scanner.feed(std::string(1, c));
        assert(out.sentinel_found == false);
        //scanner should never hang on to more than sentinel length - 1 chars
        assert(scanner.pending_size() <= max_allowed);
        total_emitted += out.safe_text.size();
    }
    assert(total_emitted + scanner.pending_size() == STREAM_BYTES);

    //flush
    SentinelScanner::Out out = scanner.flush();
    assert(out.sentinel_found == false);
    assert(scanner.pending_size() == 0);
    assert(total_emitted + out.safe_text.size() == STREAM_BYTES);
}

//Test 10: harness turn limit
void test_10() {
    std::string script = "test_turn_limit.script";

    write_text_file(script, 
        "role: assistant\n"
        "Reply one.\n"
        "---\n"
        "role: assistant\n"
        "Reply two.\n"
        "---\n"
        "role: assistant\n"
        "Reply three.\n");

    HarnessConfig config;
    config.max_turns= 2;  // Example turn limit for the test
    // unique_ptr owns the model and deletes it for us when done
    std::unique_ptr<ModelClient> model = std::make_unique<ScriptedModelClient>(script);
    Harness harness(std::move(model), config);
    ConsoleInputSimulator input;
    input.add_line("one");
    input.add_line("two");
    input.add_line("three");
    input.add_line("four");
    ConsoleOutputSimulator output;

    //runs the whole chat loop until something makes it stop
    StopReason result = harness.run(input, output);
    assert(result.kind == StopReason::Kind::TurnLimit);
    // 2 user msgs + 2 replies = 4
    assert(harness.conversation().size() == 4);
    assert(harness.conversation().at(3).content() == "Reply two.");
    std::remove(script.c_str());  // delete the temporary file
}

//Test 11: harness sentinel halt
void test_11() {
    std::string script_file = "test_sentinel.script";
    write_text_file(script_file,
        "role: assistant\n"
        "First reply.\n"
        "---\n"
        "chunk: 3\n"
        "role: assistant\n"
        "Goodbye.<|end_conversation|>\n"
        "---\n"
        "role: assistant\n"
        "This reply should never be used.\n");

    HarnessConfig config;//default max_turns = 20
    std::unique_ptr<ModelClient> model = std::make_unique<ScriptedModelClient>(script_file);
    Harness harness(std::move(model), config);
    ConsoleInputSimulator input;
    input.add_line("hello");
    input.add_line("bye");
    input.add_line("are you still there?");
    ConsoleOutputSimulator output;
    StopReason result = harness.run(input, output);
    assert(result.kind == StopReason::Kind::Sentinel);
    assert(result.detail == "stop sentinel after 2 turns");
    assert(harness.conversation().size() == 4);
    assert(harness.conversation().at(3).content() == "Goodbye." + Sentinel_Test_Value);
    assert(output.all_text.find("Goodbye.") != std::string::npos);
    //npos means find didnt find it anywhere
    assert(output.all_text.find(Sentinel_Test_Value) == std::string::npos);
    assert(output.all_text.find("never be used") == std::string::npos);
    std::remove(script_file.c_str());
}

//test 12: transcript round trip
void test_12() {
    std::string transcript_file = "test_round_trip.txt";

    // Step 1: build and save the mock conversation
    Conversation original;
    original.append(Message(Role::System, "Be concise."));
    original.append(Message(Role::User, "hello"));
    original.append(Message(Role::Assistant, "Hi! What can I do for you today?"));
    original.append(Message(Role::User, "nothing, bye"));
    original.append(Message(Role::Assistant, "Goodbye." + Sentinel_Test_Value));
    save_conversation_to_file(original, transcript_file);

    // Step 2: load it with ReplayModelClient.
    // -> is how you call a function through a pointer
    std::unique_ptr<ReplayModelClient> replay = std::make_unique<ReplayModelClient>(transcript_file);
    assert(replay->system_message() == "Be concise.");

    // Step 3: run the Harness. The system message goes in the config so
    HarnessConfig config;
    config.system_message = replay->system_message();
    Harness harness(std::move(replay), config);

    // The fake user types the same lines as the original conversation.
    ConsoleInputSimulator input;
    input.add_line("hello");
    input.add_line("nothing, bye");
    ConsoleOutputSimulator output;
    StopReason result = harness.run(input, output);
    assert(result.kind == StopReason::Kind::Sentinel);

    // Step 4: compare every message, role and text.
    const Conversation& replayed = harness.conversation();
    assert(replayed.size() == original.size());
    for (std::size_t i = 0; i < original.size(); i++) {
        assert(replayed.at(i).role() == original.at(i).role());
        assert(replayed.at(i).content() == original.at(i).content());
    }
    std::remove(transcript_file.c_str());
}

//test 13: (added based on rubric) harness (EOF and Clean Shutdown)
void test_13() {
    std::string script_file = "test_eof.script";
    write_text_file(script_file,
        "role: assistant\n"
        "Reply one.\n"
        "---\n"
        "role: assistant\n"
        "Reply two.\n");

    HarnessConfig config; //default max_turns = 20
    std::unique_ptr<ModelClient> model = std::make_unique<ScriptedModelClient>(script_file);
    Harness harness(std::move(model), config);
    ConsoleInputSimulator input;
    input.add_line("hello"); // one line, then the simulator reports EOF (Ctrl-D)
    ConsoleOutputSimulator output;
    StopReason result = harness.run(input, output);
    assert(result.kind == StopReason::Kind::UserExit);
    //one user msg plus one reply
    assert(harness.conversation().size() == 2);
    assert(harness.conversation().at(1).content() == "Reply one.");
    std::remove(script_file.c_str());
}

//sorry if this was not the intended layout
// as the orignal template said to put the tests in main
// but this is a lot better for organization
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
