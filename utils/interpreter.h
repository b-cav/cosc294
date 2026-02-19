/*
 *
 * interpreter.h - Interpreter for Scheme compiler
 *
 * Ben Cavanagh
 * 01-09-2026
 * Description: 
 *
 */

#include "helpers.h"
#include "printer.h"

// Allocate 1KB for the heap
#define HEAPSIZE 1024

// ----------------------------------------------------------
// INTERPRETER CLASS
// ----------------------------------------------------------
class Interpreter {
public:
    Interpreter(void) :
        pc(0) {}

    // Top-level interpret function
    uint64_t interpret(std::vector<uint64_t> code);

    // Helpers
    void push(uint64_t val);
    uint64_t pop(void);

private:
    uint64_t pc = 0;
    std::vector<uint64_t> stack;
};

