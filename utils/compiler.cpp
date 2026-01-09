/*
 * compiler.cpp - Scheme compiler for COSC294
 *
 * Ben Cavanagh
 * 01-08-2026
 * Description: 
 *
 */

#include "compiler.h"

// ----------------------------------------------------------
// PARSER FUNCTIONS
// ----------------------------------------------------------
// Top-level parsing function
uint64_t Parser::parse(void) {
    this->skip_wsp();
    char curr = this->peek();

    if (curr == '\0') {
        throw std::logic_error("Unexpected EOF\n");
    } else if (isdigit(curr)) {
        return(this->parse_number());
    } else {
        std::cerr << "Only integers implemented. Found <<" << curr << ">>\n";
        return(0);
    }
}

// Get next input character
char Parser::peek(void) {
    return(source[pos]);
}

// Skip whitespace characters
void Parser::skip_wsp(void) {
    static const std::string wsp = " \n\t\v\f\r";

    while(pos < length && wsp.find(peek()) != std::string::npos) {
        this->pos += 1;
    }
}

// Parse integers (no +/- signs)
uint64_t Parser::parse_number(void) {
    std::string num;
    char curr = this->peek();

    while(pos < length && isdigit(curr)) {
        num += curr;
        this->pos += 1;
        curr = this->peek();
    }

    return(std::stod(num));
}

// ----------------------------------------------------------
// COMPILER FUNCTIONS
// ----------------------------------------------------------
void Compiler::compile(std::string expr) {
    std::cerr << "Not implemented\n";
}

void Compiler::write_to_stream(FILE* f) {
    std::cerr << "Not implemented\n";
}

// ----------------------------------------------------------
// OPCODE FUNCTIONS
// ----------------------------------------------------------

