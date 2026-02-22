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
#include "compiler.h"
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
        std::string output = i.interpret(recovered);
        CHECK(output == "42\n");
    }
}



TEST_CASE("Main test loop to input .scm and confirm outputs") {
    struct Case {
        std::string name;
        std::string scm;
        std::string expect;
    } ;

    std::vector<Case> tests = {
        // ------------------------------------------------------
        // PRIMITIVES
        // ------------------------------------------------------
        {"fixnum",                      "42", "42\n"},
        {"fixnum with surrounding wsp", "       \n   \t 42 \r\n   ", "42\n"},
        {"large fixnum",                "4901231205", "4901231205\n"},
        // ------------------------------------------------------
        // UNARY OPERATORS
        // ------------------------------------------------------
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
        {"integer? true",               "(integer? 1)", "#t\n"},
        {"integer? false",              "(integer? #t)", "#f\n"},
        {"boolean? true",               "(boolean? #t)", "#t\n"},
        {"boolean? false",              "(boolean? 1)", "#f\n"},
        // ------------------------------------------------------
        // BINARY OPERATORS
        // ------------------------------------------------------
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
        {"nested equal",                "(= (* 2 3) (+ 5 1))", "#t\n"},
        // ------------------------------------------------------
        // LET BINDINGS
        // ------------------------------------------------------
        {"bind one var",                "(let ((x 2)) x)", "2\n"},
        {"bind two var ret #1",         "(let ((x 2) (y 3)) x)", "2\n"},
        {"bind two var ret #2",         "(let ((x 2) (y 3)) y)", "3\n"},
        {"add two let bnds",            "(let ((x 2) (y 3)) (+ x y))", "5\n"},
        {"nested mult two let bnds",    "(let ((x 2) (y 3)) (* x (* x y)))", "12\n"},
        {"lots of lets",                "(let ((a 4)) (let ((a (let ((a 5)) a))) (let ((a 6)) a)))", "6\n"},
        {"shadowing after",             "(let ((x 2) (y 3)) (+ y (let ((y 4)) y)))", "7\n"},
        {"shadowing before",            "(let ((x 2) (y 3)) (+ (let ((y 4)) y) y))", "7\n"},
        {"more shadowing",              "(let ((y (let ((x 4)) x)) (x 2)) (+ x (+ y (let ((x 5)) x))))", "11\n"},
        // ------------------------------------------------------
        // IF EXPRESSIONS
        // ------------------------------------------------------
        {"simple if true",              "(if (< 1 2) (+ 4 5) (* 2 3))", "9\n"},
        {"simple if false",             "(if (< 3 2) (+ 4 5) (* 2 3))", "6\n"},
        {"nested if true true",         "(if (integer? 4) (if (< 1 2) (+ 4 5) (* 2 3)) (+ 6 7))", "9\n"},
        {"nested if true false",        "(if (integer? 4) (if (> 1 2) (+ 4 5) (* 2 3)) (+ 6 7))", "6\n"},
        {"nested if false true",        "(if (boolean? 4) (+ 6 7) (if (not #f) (+ 4 5) (* 2 3)))", "9\n"},
        {"nested if false false",       "(if (not 4) (+ 6 7) (if (zero? #f) (+ 4 5) (* 2 3)))", "6\n"},
        // ------------------------------------------------------
        // PAIRS
        // ------------------------------------------------------

        // ------------------------------------------------------
        // STRINGS
        // ------------------------------------------------------

        // ------------------------------------------------------
        // VECTORS
        // ------------------------------------------------------

        // ------------------------------------------------------
        // LAMBDAS
        // ------------------------------------------------------
        {"simple lambda",               "((lambda (x) (+ x 3)) 4)", "7\n"},
        {"reuse variable",              "((lambda (x) (* x x)) 4)", "16\n"},
        {"let lambda combo",            "(let ((y 3)) ((lambda (x) (+ x y)) 4))", "7\n"},

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
            std::string final_result = i.interpret(recovered);
            CHECK(final_result == t.expect);
        }
    }

}
