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
#include <sstream>
#include "compiler.h"
#include "printer.h"

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
    program = scheme_parse(source);

    std::cerr << "\nPARSER OUTPUT:\n";
    prog_print(program, 0);

    // Compile
    compiler.compile_function(program);

    std::cerr << "\nCOMPILER OUTPUT:\n";
    std::stringstream output;
    compiler.write_to_stream(output);
    output.seekg(0);
    bc_print(output);

    // Output
    std::cerr << "\nWriting byte code...\n";
    compiler.write_to_stream(std::cout);
}

