/*
 * compiler.cpp - Scheme compiler for COSC294
 *
 * Ben Cavanagh
 * 01-08-2026
 * Description: 
 *
 */

//#define VERBOSEPARSER
//#define VERBOSECOMPILER
#include "compiler.h"

// ----------------------------------------------------------
// PARSER FUNCTIONS
// ----------------------------------------------------------
// Top-level parsing function
std::vector<Expr> Parser::parse(void) {
    #ifdef VERBOSEPARSER
    std::cerr << "parse\n";
    #endif
    std::vector<Expr> int_rep;
    while (pos < length) {
        int_rep.push_back(parse_one());
        skip_wsp();
    }
    return(int_rep);
}

// Get next input character
char Parser::peek(void) {
    #ifdef VERBOSEPARSER
    std::cerr << "    Peeked " << source[pos-1] << ">>" << source[pos] << "<<" << source[pos+1] << "\n";
    #endif
    return(source[pos]);
}

// Skip whitespace characters
void Parser::skip_wsp(void) {
    #ifdef VERBOSEPARSER
    std::cerr << "skip_wsp\n";
    #endif
    while(pos < length && wsp.find(peek()) != std::string::npos) {
        pos += 1;
    }
    #ifdef VERBOSEPARSER
    std::cerr << "END skip_wsp\n";
    #endif
}

// Parse one token
Expr Parser::parse_one(void) {
    #ifdef VERBOSEPARSER
    std::cerr << "parse_one\n";
    #endif
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
    } else if (curr == '"') {
        return(parse_string());
    } else if (isalpha(curr)) {
        return(parse_word());
    } else {
        std::cerr << "Unsupported token. Found <<" << curr << ">>\n";
        pos += 1;
        Expr expr; expr.type = EMPT; expr.empty = true;
        return(expr);
    }
}

// Go down a layer and parse inside "()"
Expr Parser::parse_nest(void) {
    #ifdef VERBOSEPARSER
    std::cerr << "parse_nest\n";
    #endif
    Expr expr;
    pos += 1;

    if (peek() == ')') {
        expr.type = EMPT;
        return(expr);
    } else {
        expr.type = NEST;
        expr.nest = new std::vector<Expr>();

        while (pos < length && peek() != ')') {
            expr.nest->push_back(parse_one());
            skip_wsp();
        }
    }
    pos += 1;
    #ifdef VERBOSEPARSER
    std::cerr << "END parse_nest\n";
    #endif

    // Check if first arg of the nest is a lambda
    // Then the nest is a procedure
    if (((*expr.nest)[0]).type == KEYW &&
        (*expr.nest)[0].keyw == I::LAMBDA) {
        expr.type = PROC;
    }
    return(expr);
}

// Parse integers (no +/- signs)
Expr Parser::parse_number(void) {
    #ifdef VERBOSEPARSER
    std::cerr << "parse_number\n";
    #endif
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

    if (keyws.find(opr) != keyws.end()) {
        expr.keyw = keyws.at(opr);
        pos += 1;
        return(expr);
    } else {
        std::cerr << "Unknown \"" << opr << "\"\n";
        throw std::logic_error("Invalid symbol\n");
    }
}

// Parse things starting with letters
Expr Parser::parse_word(void) {
    #ifdef VERBOSEPARSER
    std::cerr << "parse_word\n";
    #endif
    Expr expr;
    std::string word;

    // Accumulate non-wsp and non-')' chars into a word
    char next = peek();
    while(pos < length && wsp.find(next) == std::string::npos && next != ')') {
        word += next;
        pos += 1;
        next = peek();
    }
    if (next == ')') pos -= 1; // Correct to avoid skipping closer

    // Update src ptr; rest of fnc is figuring out what the word is
    pos += 1;

    // Check if word is keyword
    if (keyws.find(word) != keyws.end()) {
        expr.type = KEYW;
        expr.keyw = keyws.at(word);
        return(expr);
    }
    // Assume is user-defined
    else {
        expr.type = UVAR;
        expr.uvar = new std::string();
        *(expr.uvar) = word;
        return(expr);
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
    } else {
        throw std::logic_error("Invalid or unsupported token\n");
    }
}

// Parse quoted constants
Expr Parser::parse_quote(void) {
    Expr expr;

    pos += 1;
    if (peek() == '#') {
        pos += 1;
        if (peek() == '(') {
            expr.type = VECT;
            pos += 1;
            while (pos < length && peek() != ')') {
                Expr c = parse_one();
                expr.vect->push_back(c);
                pos += 1;
            }
            pos += 1; // Skip the )
        } else {
            throw std::logic_error("Unknown/unsupported quoted constant\n");
        }
    } else {
        throw std::logic_error("Unknown/unsupported quoted constant\n");
    }

    return(expr);
}

// Parse strings
Expr Parser::parse_string(void) {
    Expr expr; expr.type = STRG;
    expr.strg = new std::vector<Expr>();

    pos += 1;
    while (pos < length && peek() != '"') {
        Expr c; c.type = CHAR;
        c.char_v = peek();
        expr.strg->push_back(c);
        pos += 1; // Skip the "
    }
    pos += 1;

    return(expr);
}

// ----------------------------------------------------------
// Stand-alone parse
std::vector<Expr> scheme_parse(std::string source) {
    #ifdef VERBOSEPARSER
    std::cerr << "scheme_parse\n";
    #endif
    return(Parser(source).parse());
}

// ----------------------------------------------------------
// COMPILER FUNCTIONS
// ----------------------------------------------------------
void Compiler::compile(std::vector<Expr> &expr_vec, std::size_t start) {
    if (start >= expr_vec.size()) {
        std::cerr << "size: " << expr_vec.size() << " start: " << start << "\n";
        throw std::logic_error("Bad compile start\n");
    }
    for (std::size_t i = start; i < expr_vec.size(); ++i) {
        compile_one(expr_vec[i]);
    }
}

void Compiler::compile_one(Expr &expr) {
    switch (expr.type) {
        case EMPT :
            code.push_back(I::LOAD64);
            code.push_back(EMPTY_LIST);
            break;
        case FNUM :
            code.push_back(I::LOAD64);
            code.push_back(box_fixnum(expr.fnum_v));
            break;
        case CHAR :
            code.push_back(I::LOAD64);
            code.push_back(box_char(expr.char_v));
            break;
        case BOOL :
            code.push_back(I::LOAD64);
            code.push_back(box_bool(expr.bool_v));
            break;
        case UVAR :
            compile_uvar(expr.uvar);
            break;
        case KEYW :
            code.push_back(expr.keyw);
            break;
        case STRG : {
            std::vector<Expr> &chars = *(expr.strg);
            code.push_back(I::LOADPTR);
            code.push_back(box_strg(expr.strg->size()));
            for (size_t i = 0; i < chars.size(); ++i) {
                if (chars[i].type == CHAR) {
                    code.push_back(box_char(chars[i].char_v));
                } else {
                    throw std::logic_error("Non-char in string\n");
                }
            }
            break;
        } case VECT : {
            std::vector<Expr> &contents = *(expr.vect);
            code.push_back(I::LOADPTR);
            code.push_back(box_vect(expr.vect->size()));
            for (size_t i = 0; i < contents.size(); ++i) {
                compile_one(contents[i]);
            }
            break;
        } case NEST : {
            std::vector<Expr> &inside = *(expr.nest);
            Expr &opr = inside[0];

            // Handle let expressions: (let <bindings> <body>)
            // <bindings> : ((<variable1> <init1>) ...)
            if (opr.type == KEYW && opr.keyw == LET) {
                // Create a new scope base pointer
                code.push_back(I::SETBASE);

                // Deal with <bindings>
                if (inside[1].type == NEST) {
                    // Add new uvar map for this new scope
                    uvar_maps.push_back(std::unordered_map<std::string, uint64_t>());
                    compile_bindings(*(inside[1].nest));
                } else {
                    throw std::logic_error("Bad <bindings> in let expression\n");
                }

                // Deal with <body>
                if (inside.size() > 2) {
                    compile(inside, 2);
                    // Close let binding scope
                    uvar_maps.pop_back();
                    // Exit the let frame
                    code.push_back(I::REBASE);
                } else {
                    throw std::logic_error("Let <body> must have one or more expressions\n");
                }
            }
            // Handle if expressions: (if <test> <consequent> <alternate>)
            // where <alternate> is optional
            else if (opr.type == KEYW && opr.keyw == IF) {
                if (inside.size() < 3) {
                    throw std::logic_error("Too few args to if expression\n");
                } else if (inside.size() > 4) {
                    throw std::logic_error("Too many args to if expression\n");
                }

                // Push <test>
                compile_one(inside[1]);

                // Push JUMPIFFALSE and placeholder 0 offset
                code.push_back(I::JUMPIFFALSE);
                auto idx = code.size();
                code.push_back(0);
                // Push <conseq> and count # instructions
                compile_one(inside[2]);
                // Set jump target for when <test> is #f
                code[idx] = code.size() - idx + 1;
                // Two greater than offset below because false branch
                // also has to skip the JUMP instruction and offset

                // Push JUMP and placeholder 0 offset
                code.push_back(I::JUMP);
                idx = code.size();
                code.push_back(0);
                // Push <altern> and count # instructions
                if (inside.size() == 4) {
                    compile_one(inside[3]);
                }
                // Set jump target for when <test> is #t
                code[idx] = code.size() - idx - 1;
            }
            // Handle begin expressions: (begin <expr> <expr> ...)
            // Simply means discard return values for non-terminal exprs
            else if (opr.type == KEYW && opr.keyw == I::BEGIN) {
                for (size_t i = 1; i < inside.size() - 1; ++i) {
                    compile_one(inside[i]);
                    code.push_back(I::BEGINPOP);
                }
                compile_one(inside[inside.size() - 1]); // Last expr, whose retval not cleared
            }
            // Handle nested expressions calling procedures
            else if (opr.type == PROC) {
                // Save byte code loc of lambda
                uint64_t loc = code.size();
                // Compile the lambda/proc
                compile_one(opr);

                // Create new scope for proc call
                code.push_back(I::SETBASE);
                // Compile the args
                compile(inside, 1);

                // Push the "operator" which is just ref to the lambda
                code.push_back(LABELCALL);
                code.push_back(loc);

                // Exit the lambda frame
                code.push_back(I::REBASE);
            }
            // Handle simpler (<operator> <arg> <arg> ...) expressions
            else {
                // Recurse starting at first arg (pos 1)
                compile(inside, 1);

                // Add operator codes
                if (opr.type == KEYW && opr.keyw) {
                    code.push_back(opr.keyw);
                } else {
                    throw std::logic_error("Invalid operator\n");
                }

                // Exprs with variable number of arguments
                if (opr.keyw == I::STROP || opr.keyw == I::STRAPP ||
                    opr.keyw == I::VECOP || opr.keyw == I::VECAPP) {
                    code.push_back(inside.size() - 1);
                }
            }
            break;
        } case PROC : {
            std::vector<Expr> &inside = *(expr.nest);
            Expr &opr = inside[0];

            // Handle lambda expressions: (lambda <formals> <body>)
            // where <formals> is
            // (<var_1> ...)                     --> fixed num of args
            // <var>                             --> any num of args, as list
            // (<var_1> ... <var_n> . <var_n+1>) --> n or more args, n+1 onwards as list
            if (opr.type == KEYW && opr.keyw == I::LAMBDA) {
                // Create new map for lambda variables
                uvar_maps.push_back(std::unordered_map<std::string, uint64_t>());

                // Denote new anonymous procedure
                code.push_back(I::LABELS);
                // Bookmark spot to put jump counter
                uint64_t loc = code.size();
                code.push_back(0);

                lambda_cnt += 1;

                if (inside.size() == 3) {
                    compile_code(inside);
                } else {
                    throw std::logic_error("Lambda must match (lambda <formals> <body>)\n");
                }
                // Close lambda scope
                uvar_maps.pop_back();
                code.push_back(I::RETURN);
                // Fill in skip location
                code[loc] = code.size();
            } else {
                throw std::logic_error("Invalid procedure\n");
            }
            break;
        } default : {
            std::cerr << "Type error: " << type_lu.at(expr.type) << "\n";
            throw std::logic_error("Unknown parse object\n");
            break;
        }
    }
}

void Compiler::compile_function(std::vector<Expr> &int_rep) {
    compile(int_rep, 0);
    code.push_back(I::RETURN);
}

void Compiler::compile_bindings(std::vector<Expr> &bindings) {
    uint64_t uvar_idx = 1; // Count from 1 because offset from base anyways

    for (Expr &itr : bindings) {
        // Go through the bindings, which should be Expr vectors holding two Exprs
        // Add them to the correct variable map at the right scope
        if (itr.type == NEST && itr.nest->size() == 2) {
            std::vector<Expr> &bind = *(itr.nest);
            // The expression to which the local is bound is just stored
            // on the stack offset from the base pointer
            compile_one(bind[1]);
            if (bind[0].type == UVAR) {
                // Map var name to var count at current scope
                uvar_maps.back()[*(bind[0].uvar)] = uvar_idx;
                uvar_idx += 1;
            } else {
                throw std::logic_error("Bad variable in let binding\n");
            }
        } else {
            std::cerr << "Type: " << type_lu.at(itr.type) << "\n";
            if (itr.type == NEST) {
                std::cerr << "Size: " << itr.nest->size() << "\n";
            }
            throw std::logic_error("Bad binding in let <bindings>\n");
        }
    }
}

// Compile local variables outside of binding context
void Compiler::compile_uvar(std::string *uvar){
    // Search for var starting with most recent scope
    for (int i = uvar_maps.size()-1; i >= 0; --i) {
        auto it = uvar_maps[i].find(*uvar);
        if (it != uvar_maps[i].end()) {
            // Push variable as: LOADUV, scope depth, index/offset
            code.push_back(LOADUV);
            code.push_back(i);
            code.push_back(it->second);
            return;
        }
    }
    // Error handling; list current vars and scopes
    for (int i = uvar_maps.size()-1; i >= 0; --i) {
        std::cerr << "Map " << i << " has " << uvar_maps[i].size() << " vars\n";
        for (auto& pair : uvar_maps[i]) {
            std::cerr << pair.first << "\n";
        }
    }
    throw std::logic_error("Unbound variable " + *uvar + " at current scope depth " + std::to_string(uvar_maps.size()-1) + "\n");
}

// Compile lambda interior code
void Compiler::compile_code(std::vector<Expr> &lambda) {
    // -----------------------------------------------------------------------
    // Map the lvars
    uint64_t lvar_idx = 1;
    int flag = 0;

    // Deal with <formals> following the lambda arg
    // Case where lambda takes any number of args as list
    if (lambda[1].type == UVAR) {
        Expr &formals = lambda[1];
        // Use 0 to indicate list argument
        uvar_maps.back()[*(formals.uvar)] = 0;
    }
    // Other cases (set num or "n or more" args)
    else if (lambda[1].type == NEST) {
        std::vector<Expr> &formals = *(lambda[1].nest);
        // Loop through args and map them to an index at this scope
        for (size_t i = 0; i < formals.size(); ++i) {
            if (formals[i].type == UVAR) {
                if (flag == 1 && i == formals.size() - 1) {
                    // Use 0 to indicate list argument
                    uvar_maps.back()[*(formals[i].uvar)] = 0;
                } else if (uvar_maps.back().find(*(formals[0].uvar)) == uvar_maps.back().end()) {
                    uvar_maps.back()[*(formals[i].uvar)] = lvar_idx;
                    lvar_idx += 1;
                } else {
                    throw std::logic_error("Repeated arg in lambda <formals>\n");
                }
            } else if (formals[i].type == KEYW && formals[i].keyw == I::LEFTOVER) {
                // Period operator should be second to last arg, followed by one more UVAR
                if (i + 2 != formals.size()) {
                    throw std::logic_error("Invalid period operator in lambda <formals>\n");
                }
                flag = 1;
            } else {
                throw std::logic_error("Invalid arg in lambda <formals>\n");
            }
        }

    } else {
        throw std::logic_error("Invalid <formals> in lambda expression\n");
    }

    // -----------------------------------------------------------------------
    // Deal with function code itself

    // Store the location of the function in the bytecode
    lambda_locs[lambda_cnt] = code.size();
    compile_one(lambda[2]);

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

