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
#include <map>
#include <string>
#include <cctype>
#include <cstdint>
#include <fstream>

// ----------------------------------------------------------
// ENUMS
// ----------------------------------------------------------
// Stuff for bytecode
#define FNUM_TAG   0b00
#define FNUM_MASK  0b11
#define FNUM_SHIFT 2

#define CHAR_TAG   0b00001111
#define CHAR_MASK  0b11111111
#define CHAR_SHIFT 8

#define BOOL_TAG   0b0011111
#define BOOL_MASK  0b1111111
#define BOOL_SHIFT 7

#define EMPTY_LIST 0b00101111
#define NULL_VAL   0b01001111

enum I : uint64_t {
    LOAD64, RETURN,
    ADD1, SUB1, INT2CH, CH2INT,
    ISNULL, ISZERO, NOT, ISINT, ISBOOL,
    ADD, SUB, MULT, LESSER, GREATER, EQUAL
};

const std::map<std::string, I> keyws = {
    {"add1", ADD1}, {"sub1", SUB1}, {"integer->char", INT2CH}, {"char->integer", CH2INT},
    {"null?", ISNULL}, {"zero?", ISZERO}, {"not", NOT}, {"integer?", ISINT},
    {"boolean?", ISBOOL}, {"+", ADD}, {"-", SUB}, {"*", MULT},
    {"<", LESSER}, {">", GREATER}, {"=", EQUAL}};

// Stuff for internal representation
enum Type : int {
    EMPT, FNUM, CHAR, BOOL,
    KEYW, NEST, NILL
};

typedef struct Expr {
    Type type;
    union {
        int64_t fnum_v;
        char char_v;
        bool bool_v;
        std::vector<Expr>* nest;
        I keyw;
    };
    // Constructor
    Expr() : type(Type::EMPT), fnum_v(0) {}
} Expr;

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

private :
    std::string source;
    uint64_t pos;
    uint64_t length;
    const std::string wsp = " \n\t\v\f\r";
    const std::string ops = "+-*<>=";
};

// ----------------------------------------------------------
// Stand-alone parse
std::vector<Expr> scheme_parse(std::string source);

// ----------------------------------------------------------
// COMPILER CLASS
// ----------------------------------------------------------
class Compiler {
public :
    Compiler(void) :
        max_locals_count(0) {}

    // Top-level compile function
    void compile(std::vector<Expr> &expr_vec, std::size_t start);

    // Write byte code to file
    void write_to_stream(std::ostream &f);

    // Append RETURN opcode
    void compile_function(std::vector<Expr> &int_rep);

    // Getter
    const std::vector<uint64_t>& get_code() const {
        return code;
    }

private :
    std::vector<uint64_t> code;
    uint64_t max_locals_count;
};

// ----------------------------------------------------------
// OPCODE FUNCTIONS
// ----------------------------------------------------------
uint64_t box_fixnum(int64_t fnum_v);
uint64_t box_char(char char_v);
uint64_t box_bool(bool bool_v);
