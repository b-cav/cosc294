/*
 * parse_tests.h - 
 *
 * Ben Cavanagh
 * 01-09-2026
 * Description: 
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <cstdint>
#include <cassert>
#include "compiler.h"

class ParseTests {
public:
    uint64_t _parse(std::string source);

    bool _assert_eq(uint64_t result, uint64_t expected);

    bool ptest_fixnum(void);

    bool ptest_skip_sp_fixnum(void);

    bool ptest_long_fixnum(void);
};
