/*
 * compile.cpp - Compile a Scheme program to byte code
 *
 * Ben Cavanagh
 * 01-21-2026
 * Description: Use: ./compile < [INPUT] > [OUTPUT]
 *              Input Scheme program file
 *              Outputs bytecode file
 *
 */

#include <iostream>
#include <string>
#include "compiler.h"

int main(void) {
    std::string source, chunk;
    std::vector<Expr> program;
    Compiler compiler;

    // Read in multi-line input
    while (std::getline(std::cin, chunk)) {
        source += chunk + "\n";
    }

    std::cerr << "INPUT: <<" << source << ">>\n";
    // Parse
    std::cerr << "Parsing...\n";
    program = scheme_parse(source);

    // Compile
    std::cerr << "Compiling...\n";
    compiler.compile_function(program);

    // Output
    std::cerr << "Writing...\n";
    compiler.write_to_stream(std::cout);
}

