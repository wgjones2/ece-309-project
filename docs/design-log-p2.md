# Design Log - Project 2

## Growth factor and amortized cost
Conversation stores messages in one heap array (data_). When append() finds that the array is full, it allocates a new array with twice the capacity (growth factor of two, set by GROWTH_FACTOR in conversation.cpp so it can be changed). This growth factor means capacity goes 0, 1, 2, 4, 8, 16, etc. Whenever the array doubles, it allocates a new array entirely and deletes the old one. Test 5 checks the capacity after each of 100 appends, and doubling never leaves more than half the array unused.

Here is the proof that a dynamic array takes O(1) time on average. This is the amortized constant time. When an array with capacity c is full, it doubles to 2c. Resizing takes a * c steps for some constant a, which allocates 2c new slots, copies c items (O(1) each), and then frees the old memory.
For n total appends starting from empty, a resize happens each time the array is full, at capacities 0, 1, 2, 4, ..., 2^m, where 2^m <= n - 1. The sum of all resizing work is a * (1 + 1 + 2 + ... + 2^m) = a * 2^(m+1) <= 2a * (n - 1) < 2a * n steps (the first 1 is the allocation going from 0 to 1). 
Each append also takes 1 step to store its own message, so the total is under (2a + 1) * n. Divided by n appends, the average cost per operation is under 2a + 1 steps, which is a fixed constant, proving O(1) time.

## Rule of Five evidence
Conversation owns raw memory, so it needs all special member functions and defines all five. Meanwhile, Message needs none because its std::string member already copies and cleans up, so it uses the Rule of Zero and implements no special member functions.
Uses in Conversation:

- Destructor: delete[] data_ is an example. A moved-from object has data_ == nullptr, and delete[] on a null pointer does nothing, so destroying an empty or moved-from Conversation is safe.
- Copy constructor: Conversation allocates its own new Message[capacity_] and copies every message into it.
- Copy assignment: Builds the full copy in new_data before touching this, then deletes the old buffer and switches to the new one. If copying throws, *this keeps its old contents.
- Move constructor: noexcept takes other's pointer, size, and capacity and sets other to nullptr/0/0.
- Move assignment: noexcept checks for self-assignment, frees its own buffer, takes other's, and then zeros other.

The tests check these, and the whole suite runs under -fsanitize=address,undefined with no leaks or double-frees reported.

## Sentinel scanner: bounded pending_ proof
Here is the proof by induction. Let s be the length of the sentinel (20 for <|end_conversation|>), h = s - 1 (19), and P be the pending buffer. We want to prove that the length of P is at most h after every call.
The base case has P initially empty, length 0, which is at most h. 
For the sake of induction, assume P has at most h characters before feeding a new chunk. The function combines P and the chunk into a temporary string t and searches for the sentinel. Three cases can occur:

1. Sentinel found and P is cleared, so the length becomes 0.
2. Sentinel not found and the length of t is at most h: P becomes t, so its length is at most h.
3. Sentinel not found and the length of t is greater than h, so the first |t| - h characters are emitted as safe text and P holds only the last h characters, so its length is exactly h. The flush clears P, so the buffer size never exceeds h. The retained memory stays bounded at O(s) regardless of stream length. Test 9 confirms this by feeding a 4 MB stream one byte at a time and verifying that P never exceeds 19 bytes.

## What I would change differently
In hindsight, I would change how append() grows the array. Right now, when the array is full, it copies every message into the new array and then deletes the old one. That means the text of every message gets copied even though the old array is about to be deleted. I would use std::move to move each message instead. Moving only takes over the string's pointer, so no text is copied. The growth would still be amortized O(1), but each resize would do less work.

## Project feedback
The test program was the hardest part of this project. Ideally, it would have try-catch blocks and more advanced testing that we have not covered yet, so it would fit better later on.