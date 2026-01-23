/*
 * interpreter.cpp - Interpreter for Scheme compiler
 *
 * Ben Cavanagh
 * 01-09-2026
 * Description: 
 *
 */

#include "interpreter.h"

// ----------------------------------------------------------
// INTERPRETER FUNCTIONS
// ----------------------------------------------------------
// Top-level interpret function
uint64_t Interpreter::interpret(std::vector<uint64_t> code) {
    auto readword = [this, &code](void) {
        uint64_t instr = code[this->pc];
        this->pc += 1;
        return(instr);
    };

    while (true) {
        uint64_t instr = readword();
        switch (instr) {
            case I::LOAD64 : {
                push(readword());
                break;
            } case I::ADD1 : {
                uint64_t e0 = pop();
                push(e0 + 4); // Because of two bit tag
                break;
            } case I::SUB1 : {
                uint64_t e0 = pop();
                push(e0 - 4); // Because of two bit tag
                break;
            } case I::INT2CH : {
                uint64_t e0 = pop();
                push(box_char(e0 >> FNUM_SHIFT));
                break;
            } case I::CH2INT : {
                uint64_t e0 = pop();
                push(box_fixnum(e0 >> CHAR_SHIFT));
                break;
            } case I::ISNULL : {
                uint64_t e0 = pop();
                if (e0 == NULL_VAL) {
                    push(box_bool(true));
                } else {
                    push(box_bool(false));
                }
                break;
            } case I::ISZERO : {
                uint64_t e0 = pop();
                if ((e0 & FNUM_MASK) == FNUM_TAG && (e0 >> FNUM_SHIFT) == 0) {
                    push(box_bool(true));
                } else if ((e0 & CHAR_MASK) == CHAR_TAG && (e0 >> CHAR_SHIFT) == 0) {
                    push(box_bool(true));
                } else {
                    push(box_bool(false));
                }
                break;
            } case I::NOT : {
                uint64_t e0 = pop();
                if ((e0 & BOOL_MASK) == BOOL_TAG && (e0 >> BOOL_SHIFT) == 0) {
                    push(box_bool(true));
                } else {
                    push(box_bool(false));
                }
                break;
            } case I::ISINT : {
                uint64_t e0 = pop();
                if ((e0 & FNUM_MASK) == FNUM_TAG) {
                    push(box_bool(true));
                } else {
                    push(box_bool(false));
                }
                break;
            } case I::ISBOOL : {
                uint64_t e0 = pop();
                if ((e0 & BOOL_MASK) == BOOL_TAG) {
                    push(box_bool(true));
                } else {
                    push(box_bool(false));
                }
                break;
            } case I::ADD : {
                // Tag is LSB 00 so just add
                uint64_t e1 = pop();
                uint64_t e0 = pop();
                push(e0 + e1);
                break;
            } case I::SUB : {
                // Tag is LSB 00 so just add
                uint64_t e1 = pop();
                uint64_t e0 = pop();
                push(e0 - e1);
                break;
            } case I::MULT : {
                uint64_t e1 = pop();
                uint64_t e0 = pop();
                push((((e0 >> FNUM_SHIFT) * (e1 >> FNUM_SHIFT)) << FNUM_SHIFT) | FNUM_TAG);
                break;
            } case I::LESSER : {
                uint64_t e1 = pop();
                uint64_t e0 = pop();
                if ((e0 >> FNUM_SHIFT) < (e1 >> FNUM_SHIFT)) {
                    push(box_bool(true));
                } else {
                    push(box_bool(false));
                }
                break;
            } case I::GREATER : {
                uint64_t e1 = pop();
                uint64_t e0 = pop();
                if ((e0 >> FNUM_SHIFT) > (e1 >> FNUM_SHIFT)) {
                    push(box_bool(true));
                } else {
                    push(box_bool(false));
                }
                break;
            } case I::EQUAL : {
                uint64_t e1 = pop();
                uint64_t e0 = pop();
                if ((e0 >> FNUM_SHIFT) == (e1 >> FNUM_SHIFT)) {
                    push(box_bool(true));
                } else {
                    push(box_bool(false));
                }
                break;
            } case I::RETURN :
                return(pop());
                break;
            default :
                std::cerr << "ERROR\n";
                break;
        }
    }
    std::cerr << "Fell off the end\n";
}

// Helpers
void Interpreter::push(uint64_t val) {
    stack.push_back(val);
}

uint64_t Interpreter::pop(void) {
    uint64_t val = std::move(stack.back());
    stack.pop_back();
    return(val);
}

// ----------------------------------------------------------
// Detag and print
void print_value(uint64_t val, std::ostream &s) {
    if ((val & FNUM_MASK) == FNUM_TAG) {
        s << (val >> FNUM_SHIFT) << "\n";
    } else if ((val & CHAR_MASK) == CHAR_TAG) {
        s << reinterpret_cast<unsigned char>(static_cast<uint8_t>(val >> CHAR_SHIFT)) << "\n";
    } else if ((val & BOOL_MASK) == BOOL_TAG) {
        if ((val >> BOOL_SHIFT) == 1) {
            s << "#t\n";
        } else {
            s << "#f\n";
        }
    } else if (val == EMPTY_LIST) {
        s << "()\n";
    }
}

// ----------------------------------------------------------
// Convert stream back to code vector
std::vector<uint64_t> code_from_stream(std::istream &s) {
    std::vector<uint64_t> code;

    uint8_t buf[8];
    // Cast b/c want to read to unsigned type
    // o/w had overflow issue with breaking at 2016
    while (s.read(reinterpret_cast<char*>(buf), 8)) {
        uint64_t instr = 0;
        for (int i = 0; i < 8; i++) {
            instr |= static_cast<uint64_t>(buf[i]) << 8*i;
        }
        code.push_back(instr);
    }
    return(code);
}

