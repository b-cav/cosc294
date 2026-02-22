/*
 * helpers.h - Enums, keywords, helpers for Scheme compiler
 *
 * Ben Cavanagh
 * 02-04-2026
 *
 */

#pragma once
#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>

// ----------------------------------------------------------
// ENUMS
// ----------------------------------------------------------
// Stuff for bytecode
#define EMPTY_LIST  0b00101111

#define FNUM_TAG    0b00
#define FNUM_MASK   0b11
#define FNUM_SHIFT  2

#define CHAR_TAG    0b00001111
#define CHAR_MASK   0b11111111
#define CHAR_SHIFT  8

#define BOOL_TAG    0b0011111
#define BOOL_MASK   0b1111111
#define BOOL_SHIFT  7

#define PAIR_TAG    0b001
#define PAIR_MASK   0b111
#define PAIR_SHIFT  3

#define VECT_TAG     0b010
#define VECT_MASK    0b111
#define VECT_SHIFT   3

#define STRG_TAG     0b011
#define STRG_MASK    0b111
#define STRG_SHIFT   3

#define SYMB_TAG    0b101
#define SYMB_MASK   0b111
#define SYMB_SHIFT  3

#define CLOS_TAG    0b110
#define CLOS_MASK   0b111
#define CLOS_SHIFT  3

enum I : uint64_t {
    LOAD64, LOADPTR, RETURN,
    ADD1, SUB1, INT2CH, CH2INT, ISNULL, ISZERO, NOT, ISINT, ISBOOL,
    ADD, SUB, MULT, LESSER, GREATER, EQUAL,
    LET, STOREUV, LOADUV,
    IF, JUMP, JUMPIFFALSE,
    CONS, CAR, CDR,
    STROP, STRLEN, STRREF, STRSET, STRAPP,
    VECOP, VECLEN, VECREF, VECSET, VECAPP,
    BEGIN, BEGINPOP,
    SETBASE, REBASE,
    LAMBDA, LEFTOVER,
    LABELS, CODE, PRIMCALL, LABELCALL,
};

const std::map<std::string, I> keyws = {
    {"add1", ADD1}, {"sub1", SUB1}, {"integer->char", INT2CH}, {"char->integer", CH2INT},
    {"null?", ISNULL}, {"zero?", ISZERO}, {"not", NOT}, {"integer?", ISINT},
    {"boolean?", ISBOOL}, {"+", ADD}, {"-", SUB}, {"*", MULT},
    {"<", LESSER}, {">", GREATER}, {"=", EQUAL},
    {"let", LET}, {"if", IF},
    {"cons", CONS}, {"car", CAR}, {"cdr", CDR},
    {"string", STROP}, {"string-length", STRLEN}, {"string-ref", STRREF},
    {"string-set!", STRSET}, {"string-append", STRAPP},
    {"vector", VECOP}, {"vector-length", VECLEN}, {"vector-ref", VECREF},
    {"vector-set!", VECSET}, {"vector-append", VECAPP},
    {"begin", BEGIN},
    {"lambda", LAMBDA}, {".", LEFTOVER},
};

// For printing parser/compiler outputs
const std::map<I, std::string> keyw_lu = {
    {ADD1, "add1"}, {SUB1, "sub1"}, {INT2CH, "integer->char"}, {CH2INT, "char->integer"},
    {ISNULL, "null?"}, {ISZERO, "zero?"}, {NOT, "not"}, {ISINT, "integer?"},
    {ISBOOL, "boolean?"}, {ADD, "+"}, {SUB, "-"}, {MULT, "*"},
    {LESSER, "<"}, {GREATER, ">"}, {EQUAL, "="},
    {LET, "let"}, {IF, "if"},
    {CONS, "cons"}, {CAR, "car"}, {CDR, "cdr"},
    {STROP, "string"}, {STRLEN, "string-length"}, {STRREF, "string-ref"},
    {STRSET, "string-set!"}, {STRAPP, "string-append"},
    {VECOP, "vector"}, {VECLEN, "vector-length"}, {VECREF, "vector-ref"},
    {VECSET, "vector-set!"}, {VECAPP, "vector-append"},
    {BEGIN, "begin"},
    {LAMBDA, "lambda"}, {LEFTOVER, "."},
};

// Things that should only be compiler output instructions
const std::map<I, std::string> instr_lu = {
    {LOAD64, "LOAD64"}, {RETURN, "RETURN"},
    {STOREUV, "STOREUV"}, {LOADUV, "LOADUV"},
    {JUMP, "JUMP"}, {JUMPIFFALSE, "JUMPIFFALSE"}, {LOADPTR, "LOADPTR"},
    {BEGINPOP, "BEGINPOP"},
    {SETBASE, "SETBASE"}, {REBASE, "REBASE"},
    {LABELS, "LABELS"}, {CODE, "CODE"},
    {PRIMCALL, "PRIMCALL"}, {LABELCALL, "LABELCALL"},
};

/* Stuff for internal representation
 *
 * From R5RS:
 * No object satisfies more than one of the following predicates:
 * boolean? pair? symbol? number?
 * char? string? vector? port? procedure?
 * The empty list is a special object of its own type; it satisfies none of the above predicates.
 *
 * Also hold:
 * UVAR for user-defined objects
 * NEST for nested expressions
 */
enum Type : int {
    BOOL, PAIR, KEYW, FNUM,         // Called symbols keywords (KEYW) early on
    CHAR, STRG, VECT, PORT, PROC,
    EMPT,
    UVAR, NEST
};

static std::map<Type, std::string> type_lu = {
    {BOOL, "boolean"}, {PAIR, "pair"}, {KEYW, "keyword"},
    {FNUM, "fixnum"}, {CHAR, "char"}, {STRG, "string"},
    {VECT, "vector"}, {PORT, "port"}, {PROC, "procedure"},
    {EMPT, "empty nest"},
    {UVAR, "user variable"}, {NEST, "nested expression"},
};

// ----------------------------------------------------------
// STRUCTS
// ----------------------------------------------------------
typedef struct Expr {
    Type type;
    union {
        bool empty;
        int64_t fnum_v;
        char char_v;
        bool bool_v;
        I keyw;
        std::string* uvar;
        std::vector<Expr>* nest;    // Different labels just for code clarity
        std::vector<Expr>* strg;
        std::vector<Expr>* vect;
    };
    // Constructor, default to empty list
    Expr() : type(Type::EMPT), empty(true) {}
} Expr;

// ----------------------------------------------------------
// OPCODE FUNCTIONS
// ----------------------------------------------------------
uint64_t box_fixnum(int64_t fnum_v);
uint64_t box_char(char char_v);
uint64_t box_bool(bool bool_v);
uint64_t box_vect(size_t veclen);
uint64_t box_strg(size_t strlen);
