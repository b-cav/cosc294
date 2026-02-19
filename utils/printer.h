/*
 * printer.h - Pretty printer and I/O for Scheme compiler
 *
 * Ben Cavanagh
 * 01-27-2026
 * Description: Convert std::vector<Expr> (parser
 *              output) to formatted text and print.
 *
 *              Convert byte code file (compiler
 *              output) to formatted text and print.
 *
 */

#include <cstdio>
#include "helpers.h"

// Print parser output
void prog_print(std::vector<Expr> &program, int indent);

// Print a single Expr
void print_expr(Expr &expr, int indent);

// Print compiler output
void bc_print(std::istream &stream);

// Detag and print a uint64_t
void print_value(uint64_t val, uint64_t *heap, std::ostream &s);

// Convert stream back to code vector
std::vector<uint64_t> code_from_stream(std::istream &s);

