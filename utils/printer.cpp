/*
 * printer.cpp - Pretty printer for compiler
 *
 * Ben Cavanagh
 * 01-27-2026
 * Description: Convert std::vector<Expr> (parser
 *              output) to formatted text and print.
 *
 *              Convert byte code file (compiler
 *              output) to formatted text and print.
 *
 */

#include "printer.h"

// ----------------------------------------------------------
// PRETTY PRINT PARSER OUTPUT
// ----------------------------------------------------------
void prog_print(std::vector<Expr> &program, int indent) {
    for (auto &expr : program) {
        print_expr(expr, indent);
    }
}

// ----------------------------------------------------------
// Print a single Expr
void print_expr(Expr &expr, int indent) {
    for (int i = 0; i < indent; i++) { fprintf(stderr, "  "); }
    switch (expr.type) {
        case EMPT :
            fprintf(stderr, "EMPTY LIST: ()\n");
            break;
        case FNUM :
            fprintf(stderr, "FIXNUM: \"%ld\"\n", expr.fnum_v);
            break;
        case CHAR :
            fprintf(stderr, "CHARACTER: \"%c\"\n", expr.char_v);
            break;
        case BOOL :
            fprintf(stderr, "BOOLEAN: \"%d\"\n", expr.bool_v);
            break;
        case UVAR :
            fprintf(stderr, "USER-DEFINED: Depth %lu, #%lu\n", expr.uvar_dep, expr.uvar_num);
            break;
        case KEYW :
            if (keyw_lu.find(expr.keyw) == keyw_lu.end()) {
                throw std::logic_error("Parser expr not found");
            }
            fprintf(stderr, "KEYWORD: \"%s\"\n", keyw_lu.at(expr.keyw).c_str());
            break;
        case NEST :
            fprintf(stderr, "NESTED EXPRESSION {\n");
            prog_print(*(expr.nest), indent + 1);
            for (int i = 0; i < indent; i++) { fprintf(stderr, "  "); }
            fprintf(stderr, "}\n");
            break;
        case STRG :
            fprintf(stderr, "STRING {\n");
            prog_print(*(expr.nest), indent + 1);
            for (int i = 0; i < indent; i++) { fprintf(stderr, "  "); }
            fprintf(stderr, "}\n");
            break;
        case VECT :
            fprintf(stderr, "VECTOR {\n");
            prog_print(*(expr.nest), indent + 1);
            for (int i = 0; i < indent; i++) { fprintf(stderr, "  "); }
            fprintf(stderr, "}\n");
            break;
        default :
            break;
    }
}

// ----------------------------------------------------------
// PRETTY PRINT COMPILER OUTPUT
// ----------------------------------------------------------
void bc_print(std::istream &stream) {
    std::vector<uint64_t> recovered = code_from_stream(stream);
    uint64_t i = 0;
    I instr;
    std::string text;

    auto readword = [&recovered, &i](void) {
        I instr = static_cast<I>(recovered[i]);
        i += 1;
        return(instr);
    };

    while (i < recovered.size()) {
        instr = readword();
        if (instr_lu.find(instr) != instr_lu.end()) {
            text = instr_lu.at(instr);
        } else if (keyw_lu.find(instr) != keyw_lu.end()) {
            text = keyw_lu.at(instr);
        } else {
            throw std::logic_error("Print err: Compiler instruction not found");
        }
        fprintf(stderr, "OPRN: %s\n", text.c_str());
        if (instr == I::LOAD64) {
            fprintf(stderr, "VALU: ");
            print_value(readword(), nullptr, std::cerr);
        } else if (instr == I::LOADPTR) {
            uint64_t len = (readword() >> VECT_SHIFT);
            fprintf(stderr, "LENG: %ld\n", len);
            for (uint64_t j = 0; j < len; ++j) {
                fprintf(stderr, "VALU: ");
                print_value(readword(), nullptr, std::cerr);
            }
        } else if (instr == I::STROP || instr == I::STRAPP ||
            instr == I::VECOP || instr == I::VECAPP) {
            uint64_t len = (readword());
            fprintf(stderr, "LENG: %ld\n", len);
        } else if (instr == I::LOADUV || instr == I::STOREUV) {
            fprintf(stderr, "UVAR: #%ld\n", readword());
        } else if (instr == I::JUMP || instr == I::JUMPIFFALSE) {
            fprintf(stderr, "STEP: %ld\n", readword());
        }
    }
}

// ----------------------------------------------------------
// Detag and print a uint64_t
void print_value(uint64_t val, uint64_t *heap, std::ostream &s) {
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
    } else if ((val & PAIR_MASK) == PAIR_TAG) {
        if (heap == nullptr) {
            s << "PAIR (heap offset " << (val >> PAIR_SHIFT) << " words)\n";
        } else {
            uint64_t* loc = heap + (val >> PAIR_SHIFT);
            s << "(";
            print_value(*loc, heap, s);
            s << " . ";
            print_value(*(loc + 1), heap, s);
            s << ")\n";
        }
    } else if ((val & STRG_MASK) == STRG_TAG) {
        if (heap == nullptr) {
            s << "STRING (heap offset " << (val >> STRG_SHIFT) << " words)\n";
        } else {
            uint64_t loc = (val >> STRG_SHIFT);
            uint64_t strlen = heap[loc];
            std::string heap_str;
            for (uint64_t i = strlen; i > 0; --i) {
                char c = (heap[loc + i]) >> CHAR_SHIFT;
                heap_str += c;
            }
            s << heap_str << "\n";
        }
    } else if ((val & VECT_MASK) == VECT_TAG) {
        if (heap == nullptr) {
            s << "VECTOR (heap offset " << (val >> VECT_SHIFT) << " words)\n";
        } else {
            uint64_t loc = (val >> VECT_SHIFT);
            uint64_t strlen = heap[loc];

            s << "#(";
            // TODO: Convert to stringstream with recursive print)val
            std::string heap_str;
            for (uint64_t i = strlen; i > 0; --i) {
                char c = (heap[loc + i]) >> CHAR_SHIFT;
                heap_str += c;
            }
            s << heap_str << ")\n";
        }
    } else if (val == EMPTY_LIST) {
        s << "()\n";
    } else {
        s << "invalid input to print_value()\n";
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

