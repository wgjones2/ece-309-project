// test code for the project

#undef NDEBUG
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>

#include "core/conversation.h"
#include "core/message.h"
#include "core/sentinel_scanner.h"
#include "harness/harness.h"
#include "model/model_client.h"
#include "model/scripted_client.h"
#include "model/replay_client.h"

namespace {
    const std::string kSentinel = "<|end_conversation|>";

    //helper function to check if out of range
    bool at_throws_out_of_range(const Conversation& conv, std::size_t i) {
        try {
            conv.at(i);
        } catch (const std::out_of_range&) {
            return true;
        }
        return false;
    }
}
