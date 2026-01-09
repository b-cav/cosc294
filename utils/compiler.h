/*
 * compiler.h - Scheme compiler for COSC294
 *
 * Ben Cavanagh
 * 01-08-2026
 * Description: 
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdint>

// ----------------------------------------------------------
// PARSER CLASS
// ----------------------------------------------------------
class Parser {
public:
    Parser(std::string source) :
        source(source), pos(0), length(source.length()) {}

    // Top-level parsing function
    uint64_t parse(void);

    // Get next input character
    char peek(void);

    // Skip whitespace characters
    void skip_wsp(void);

    // Parse integers (no +/- signs)
    uint64_t parse_number(void);

private:
    std::string source;
    uint64_t pos;
    uint64_t length;
};

// ----------------------------------------------------------
// COMPILER CLASS
// ----------------------------------------------------------
class Compiler {
public:
    Compiler(void);

    void compile(std::string expr);

    void write_to_stream(FILE* f);

private:
    std::vector<std::string> code;
};

// ----------------------------------------------------------
// OPCODE ENUMS
// ----------------------------------------------------------
enum class I : uint64_t {
    Empty = 0
};

