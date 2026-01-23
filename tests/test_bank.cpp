/*
 * run_tests.cpp - Unit tests for COSC294 parser
 *
 * Ben Cavanagh
 * 01-09-2026
 *
 */

// Doctest setup
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "interpreter.h"

TEST_CASE("Test integer parsing") {
    // Fixnum
    CHECK(Parser("42").parse()[0].fnum_v == 42);
    // Leading wsp
    CHECK(Parser("       42").parse()[0].fnum_v == 42);
    // Long fixnum
    CHECK(Parser("4901231205").parse()[0].fnum_v == 4901231205);
}


TEST_CASE("Test full input -> output of single integer") {
    std::vector<Expr> int_rep;
    Expr expr; expr.type = FNUM; expr.fnum_v = 42;

    int_rep.push_back(expr);
    Compiler c; c.compile_function(int_rep);

    SUBCASE("See that opcodes are properly loaded") {
        std::vector<uint64_t> code = c.get_code();
        CHECK(code[0] == I::LOAD64);
        CHECK(code[1] == ((42 << FNUM_SHIFT) | FNUM_TAG));
        CHECK(code[2] == I::RETURN);
    }

    SUBCASE("Check output") {
        Interpreter i;

        // Output byte code
        std::stringstream stream;
        c.write_to_stream(stream);
        stream.seekg(0);

        // Read byte code back in
        std::vector<uint64_t> recovered = code_from_stream(stream);
        uint64_t result = i.interpret(recovered);
        CHECK(result == ((42 << FNUM_SHIFT) | FNUM_TAG));

        std::stringstream output;
        print_value(result, output);
        CHECK(output.str() == "42\n");
    }
}



TEST_CASE("Main test loop to input .scm and confirm outputs") {
    struct Case {
        std::string name;
        std::string scm;
        std::string expect;
    } ;

    std::vector<Case> tests = {
        {"fixnum",                      "42", "42\n"},
        {"fixnum with surrounding wsp", "       \n   \t 42 \r\n   ", "42\n"},
        {"large fixnum",                "4901231205", "4901231205\n"},
        {"add1",                        "(add1 42)", "43\n"},
        {"sub1",                        "(sub1 42)", "41\n"},
        {"integer->char",               "(integer->char 42)", "*\n"},
        {"char->integer",               "(char->integer #\\*)", "42\n"},
      //{"null?",                       "'()", "#t\n"},
        {"yes zero?",                   "(zero? 0)", "#t\n"},
        {"no zero?",                    "(zero? 1)", "#f\n"},
        {"not true",                    "(not #t)", "#f\n"},
        {"not false",                   "(not #f)", "#t\n"},
        {"0 is truthy",                 "(not 0)", "#f\n"},
        {"1 is truthy",                 "(not 1)", "#f\n"},
        {"yes integer?",                "(integer? 1)", "#t\n"},
        {"no integer?",                 "(integer? #t)", "#f\n"},
        {"yes boolean?",                "(boolean? #t)", "#t\n"},
        {"no boolean?",                 "(boolean? 1)", "#f\n"},
        {"addition",                    "(+ 2 3)", "5\n"},
        {"nested addition",             "(+ (+ 2 4) 3)", "9\n"},
        {"other way nested addition",   "(+ 2 (+ 4 3))", "9\n"},
        {"subtraction",                 "(- 3 2)", "1\n"},
      //{"subzero subtraction",         "(- 3 4)", "-1\n"},
      //{"nested subzero subtraction",  "(- (- 5 6) 4)", "-5\n"},
        {"multiplication",              "(* 2 3)", "6\n"},
        {"nested multiplication",       "(* (* 2 4) 3)", "24\n"},
        {"lesser",                      "(< 1 2)", "#t\n"},
        {"not lesser",                  "(< 2 1)", "#f\n"},
        {"nested lesser",               "(< (* 2 8) (+ 5 1))", "#f\n"},
        {"greater",                     "(> 2 1)", "#t\n"},
        {"not greater",                 "(> 1 2)", "#f\n"},
        {"nested greater",              "(> (* 2 8) (+ 5 1))", "#t\n"},
        {"equal",                       "(= 2 2)", "#t\n"},
        {"not equal",                   "(= 2 3)", "#f\n"},
        {"nested equal",                "(= (* 2 3) (+ 5 1))", "#t\n"}
    };

    for (auto& t : tests) {
        SUBCASE(t.name.c_str()) {
            Compiler c; Interpreter i;
            std::stringstream intermed, output;

            // Parse
            std::vector<Expr> program = Parser(t.scm).parse();
            // Compile
            c.compile_function(program);
            // I/O
            c.write_to_stream(intermed);
            intermed.seekg(0);
            std::vector<uint64_t> recovered = code_from_stream(intermed);
            // Interpret
            uint64_t result = i.interpret(recovered);
            // I/O
            print_value(result, output);
            CHECK(output.str() == t.expect);
        }
    }

}
