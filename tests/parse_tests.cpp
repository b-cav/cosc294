/*
 * parse_tests.cpp - Unit tests for COSC294 parser
 *
 * Ben Cavanagh
 * 01-09-2026
 * Description: 
 *
 */

#include "parse_tests.h"

int main(void) {
    ParseTests tester;

    if (tester.ptest_fixnum()) {
        printf("[PASS] ptest_fixnum\n");
    } else {
        printf("[FAIL] ptest_fixnum\n");
    }
    if (tester.ptest_skip_sp_fixnum()) {
        printf("[PASS] ptest_skip_sp_fixnum\n");
    } else {
        printf("[FAIL] ptest_skip_sp_fixnum\n");
    }
    if (tester.ptest_long_fixnum()) {
        printf("[PASS] ptest_long_fixnum\n");
    } else {
        printf("[FAIL] ptest_long_fixnum\n");
    }
}

uint64_t ParseTests::_parse(std::string source) {
    return(Parser(source).parse());
}

bool ParseTests::_assert_eq(uint64_t result, uint64_t expected){
    return(result == expected);
}

bool ParseTests::ptest_fixnum(void) {
    return(_assert_eq(this->_parse("42"), 42));
}

bool ParseTests::ptest_skip_sp_fixnum(void) {
    return(_assert_eq(this->_parse("         42"), 42));
}

bool ParseTests::ptest_long_fixnum(void) {
    return(_assert_eq(this->_parse("4901231205"), 4901231205));
}
