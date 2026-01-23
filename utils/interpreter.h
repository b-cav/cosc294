/*
 *
 * interpreter.h - Interpreter for Scheme compiler
 *
 * Ben Cavanagh
 * 01-09-2026
 * Description: 
 *
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include "compiler.h"

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

// ----------------------------------------------------------
// Detag and print
void print_value(uint64_t val, std::ostream &s);

// ----------------------------------------------------------
// Convert stream back to code vector
std::vector<uint64_t> code_from_stream(std::istream &s);

