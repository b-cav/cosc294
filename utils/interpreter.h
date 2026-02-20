/*
 *
 * interpreter.h - Interpreter for Scheme compiler
 *
 * Ben Cavanagh
 * 01-09-2026
 * Description: 
 *
 */

#pragma once
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
        pc(0), sptr(0), base(0) {}

    // Top-level interpret function
    std::string interpret(std::vector<uint64_t> code);

    // Helpers
    void push(uint64_t val);
    uint64_t pop(void);

private:
    uint64_t pc = 0;
    std::vector<uint64_t> stack;
    uint64_t sptr = 0;
    uint64_t base = 0;
    uint64_t base_dep = 0;
};

