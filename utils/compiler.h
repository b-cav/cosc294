/*
 * compiler.h - Scheme compiler for COSC294
 *
 * Ben Cavanagh
 * 01-08-2026
 * Description: 
 *
 */

#pragma once
#include <cctype>
#include <fstream>
#include "helpers.h"

// ----------------------------------------------------------
// PARSER CLASS
// ----------------------------------------------------------
class Parser {
public :
    Parser(std::string source) :
        source(source), pos(0), length(source.length()) {}

    // Top-level parsing function
    std::vector<Expr> parse(void);

    // Get next input character
    char peek(void);

    // Skip whitespace characters
    void skip_wsp(void);

    // Parse one token
    Expr parse_one(void);

    // Go down a layer and parse inside "()"
    Expr parse_nest(void);

    // Parse integers (no +/- signs)
    Expr parse_number(void);

    // Parse operator symbols
    Expr parse_opr(void);

    // Parse things starting with letters
    Expr parse_word(void);

    // Parse booleans and chars
    Expr parse_hash(void);

    // Parse quoted constants
    Expr parse_quote(void);

    // Parse strings
    Expr parse_string(void);

private :
    std::string source;
    uint64_t pos;
    uint64_t length;
    const std::string wsp = " \n\t\v\f\r";
    const std::string ops = "+-*<>=.";
};

// ----------------------------------------------------------
// Stand-alone parse
std::vector<Expr> scheme_parse(std::string source);

// ----------------------------------------------------------
// COMPILER CLASS
// ----------------------------------------------------------
class Compiler {
public :
    // Constructor
    // Adds an empty map as 0 scope depth for globals
    Compiler(void) :
        uvar_maps(1), lambda_cnt(0) {}

    // Top-level compile function
    void compile(std::vector<Expr> &expr_vec, std::size_t start);

    // Compile one expr
    void compile_one(Expr &expr);

    // Write byte code to file
    void write_to_stream(std::ostream &f);

    // Compile but append RETURN opcode
    void compile_function(std::vector<Expr> &int_rep);

    // Compile let bindings
    void compile_bindings(std::vector<Expr> &assignments);

    // Compile local variables outside of binding context
    void compile_uvar(std::string *uvar);

    // Compile lambda interior code
    void compile_code(std::vector<Expr> &code);

    // Getter
    const std::vector<uint64_t>& get_code() const {
        return code;
    }

private :
    std::vector<uint64_t> code;
    std::vector<std::unordered_map<std::string, uint64_t>> uvar_maps;
    uint64_t uvar_cnt;
    uint64_t lambda_cnt;
    std::unordered_map<uint64_t, uint64_t> lambda_locs;
};

