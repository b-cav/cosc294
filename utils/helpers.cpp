/*
 * helpers.cpp - Enums, keywords, helpers for Scheme compiler
 *
 * Ben Cavanagh
 * 02-04-2026
 *
 */

#include "helpers.h"

// ----------------------------------------------------------
// OPCODE FUNCTIONS
// ----------------------------------------------------------
uint64_t box_fixnum(int64_t fnum_v) {
    return((fnum_v << FNUM_SHIFT) | FNUM_TAG);
}

uint64_t box_char(char char_v) {
    return((char_v << CHAR_SHIFT) | CHAR_TAG);
}

uint64_t box_bool(bool bool_v) {
    return((bool_v << BOOL_SHIFT) | BOOL_TAG);
}

uint64_t box_vect(size_t veclen) {
    uint64_t casted = static_cast<uint64_t>(veclen);
    return((casted << VECT_SHIFT) | VECT_TAG);
}

uint64_t box_strg(size_t strlen) {
    uint64_t casted = static_cast<uint64_t>(strlen);
    return((casted << STRG_SHIFT) | STRG_TAG);
}

