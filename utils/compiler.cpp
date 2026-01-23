/*
 * compiler.cpp - Scheme compiler for COSC294
 *
 * Ben Cavanagh
 * 01-08-2026
 * Description: 
 *
 */

#include "compiler.h"

// ----------------------------------------------------------
// PARSER FUNCTIONS
// ----------------------------------------------------------
// Top-level parsing function
std::vector<Expr> Parser::parse(void) {
    std::vector<Expr> int_rep;
    while (pos < length) {
        int_rep.push_back(parse_one());
        skip_wsp();
    }
    return(int_rep);
}

// Get next input character
char Parser::peek(void) {
    return(source[pos]);
}

// Skip whitespace characters
void Parser::skip_wsp(void) {
    while(pos < length && wsp.find(peek()) != std::string::npos) {
        pos += 1;
    }
}

// Parse one token
Expr Parser::parse_one(void) {
    skip_wsp();
    char curr = peek();
    if (curr == '(') {
        return(parse_nest());
    } else if (isdigit(curr)) {
        return(parse_number());
    } else if (ops.find(curr) != std::string::npos) {
        return(parse_opr());
    } else if (curr == '#') {
        return(parse_hash());
    } else if (isalpha(curr)) {
        return(parse_word());
    } else {
        std::cerr << "Unsupported token. Found <<" << curr << ">>\n";
        pos += 1;
        Expr expr; expr.type = NILL;
        return(expr);
    }
}

// Go down a layer and parse inside "()"
Expr Parser::parse_nest(void) {
    Expr expr;
    pos += 1;

    if (peek() == ')') {
        expr.type = EMPT;
    } else {
        expr.type = NEST;
        expr.nest = new std::vector<Expr>();

        while (pos < length && peek() != ')') {
            expr.nest->push_back(parse_one());
            skip_wsp();
        }
    }
    pos += 1;
    return(expr);
}

// Parse integers (no +/- signs)
 Expr Parser::parse_number(void) {
    Expr expr; expr.type = FNUM;
    std::string num;
    char curr = peek();

    while(pos < length && isdigit(curr)) {
        num += curr;
        pos += 1;
        curr = peek();
    }

    expr.fnum_v = std::stoll(num);
    return(expr);
}

// Parse operator symbols
Expr Parser::parse_opr(void) {
    Expr expr; expr.type = KEYW;
    std::string opr; opr += peek();

    if (keyws.at(opr)) {
        expr.keyw = keyws.at(opr);
        pos += 1;
        return(expr);
    } else {
        throw std::logic_error("Invalid keyword\n");
    }
}

// Parse things starting with letters
Expr Parser::parse_word(void) {
    Expr expr; expr.type = KEYW;
    std::string word;

    while(pos < length && wsp.find(peek()) == std::string::npos) {
        word += peek();
        pos += 1;
    }

    if (keyws.at(word)) {
        expr.keyw = keyws.at(word);
        pos += 1;
        return(expr);
    } else {
        std::cerr << "Unknown \"" << word << "\"\n";
        throw std::logic_error("Invalid keyword\n");
    }
}

// Parse booleans and chars
 Expr Parser::parse_hash(void) {
    Expr expr; expr.type = BOOL;
    static const std::string wsp = " \n\t\v\f\r";

    pos += 1;
    if (peek() == 't') {
        pos += 1;
        expr.bool_v = true;
        return(expr);
    } else if (peek() == 'f') {
        pos += 1;
        expr.bool_v = false;
        return(expr);
    } else if (peek() == '\\') {
        pos += 1;
        expr.type = CHAR;
        expr.char_v = peek();
        pos += 1;
        return(expr);
    }
    throw std::logic_error("Invalid or unsupported token\n");
}
// ----------------------------------------------------------
// Stand-alone parse
std::vector<Expr> scheme_parse(std::string source) {
    return(Parser(source).parse());
}

// ----------------------------------------------------------
// COMPILER FUNCTIONS
// ----------------------------------------------------------
void Compiler::compile(std::vector<Expr> &expr_vec, std::size_t start) {
    if (start >= expr_vec.size()) throw std::logic_error("Bad compile start\n");
    for (std::size_t i = start; i < expr_vec.size(); ++i) {
        switch (expr_vec[i].type) {
            case EMPT :
                code.push_back(I::LOAD64);
                code.push_back(EMPTY_LIST);
                break;
            case NILL :
                code.push_back(I::LOAD64);
                code.push_back(NULL_VAL);
                break;
            case FNUM :
                code.push_back(I::LOAD64);
                code.push_back(box_fixnum(expr_vec[i].fnum_v));
                break;
            case CHAR :
                code.push_back(I::LOAD64);
                code.push_back(box_char(expr_vec[i].char_v));
                break;
            case BOOL :
                code.push_back(I::LOAD64);
                code.push_back(box_bool(expr_vec[i].bool_v));
                break;
            case KEYW :
                code.push_back(expr_vec[i].keyw);
                break;
            case NEST : {
                std::vector<Expr> &inside = *(expr_vec[i].nest);
                // Recurse starting at first arg (pos 1)
                Expr &opr = inside[0];
                compile(inside, 1);

                // Add operator codes
                if (opr.type == KEYW && opr.keyw) {
                    code.push_back(opr.keyw);
                } else {
                    throw std::logic_error("Missing operator\n");
                }
                break;
            }
            default :
                break;
        }
    }
}

void Compiler::compile_function(std::vector<Expr> &int_rep) {
    compile(int_rep, 0);
    code.push_back(I::RETURN);
}

void Compiler::write_to_stream(std::ostream &f) {
    for (uint64_t& instr : code) {
        for (int i = 0; i < 8; i++) {
            // Little-endian 64-bit words
            char byte = static_cast<char>((instr >> 8*i) & 0xFF);
            f.write(&byte, sizeof(byte));
        }
    }
}

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
