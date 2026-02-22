/*
 * interpret.cpp - Interpret byte code instructions
 *
 * Ben Cavanagh
 * 01-21-2026
 * Description: Use: ./interpret < [CODE]
 *              Input 64-bit word byte code
 *              Outputs program result
 *
 */

#include "interpreter.h"
#include "printer.h"

int main(int argc, char** argv) {
    Interpreter i;
    std::vector<uint64_t> code = code_from_stream(std::cin);

    std::string result = i.interpret(code);
    std::cout << "Return val (top of stack rn): \n" << result;

    exit(EXIT_SUCCESS);
}


