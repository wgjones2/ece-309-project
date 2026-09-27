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

