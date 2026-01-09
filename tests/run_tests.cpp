/*
 * parse_tests.cpp - Unit tests for COSC294 parser
 *
 * Ben Cavanagh
 * 01-09-2026
 *
 */

// Doctest setup
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "compiler.h"

TEST_CASE("Test integer parsing") {
    CHECK(Parser("42").parse() == 42);
    CHECK(Parser("       42").parse() == 42);
    CHECK(Parser("4901231205").parse() == 4901231205);
}
