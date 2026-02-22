/*
 * interpreter.cpp - Interpreter for Scheme compiler
 *
 * Ben Cavanagh
 * 01-09-2026
 * Description: 
 *
 */

//#define VERBOSE
#include "interpreter.h"

// ----------------------------------------------------------
// INTERPRETER FUNCTIONS
// ----------------------------------------------------------
// Top-level interpret function
std::string Interpreter::interpret(std::vector<uint64_t> code) {
    auto readword = [this, &code](void) {
        uint64_t instr = code[this->pc];
        this->pc += 1;
        return(instr);
    };

    // Allocate heap
    uint64_t* heap = new uint64_t[HEAPSIZE];
    uint64_t hptr = 0; // Always point to next available heap slot

    // Interpret loop
    while (true) {
        uint64_t instr = readword();
        switch (instr) {
            // ----------------------------------------------
            // PUSH VALUES (DIRECTLY OR HEAP PTR)
            // ----------------------------------------------
            case I::LOAD64 : {
                push(readword());
                break;
            } case I::LOADPTR : {
                // Expects <tagged len> <byte> <byte> ...
                uint64_t len = readword(); Type type;
                uint64_t start = hptr;

                std::cerr << "len?? read " << len << "\n";
                std::cerr << "prev?? " << code[pc-2] << "\n";
                std::cerr << "next?? " << code[pc] << "\n";
                // Get the complex object length and type
                if ((len & VECT_MASK) == VECT_TAG) {
                    type = VECT;
                } else if ((len & STRG_MASK) == STRG_TAG) {
                    type = STRG;
                } else {
                    throw std::logic_error("Unknown length field");
                }
                // Length was tagged by compiler to share type
                // Transfering that info to the heap ptr
                len = len >> VECT_SHIFT;
                heap[hptr] = len;               // Push raw length onto heap
                hptr += 1;

                // Push values onto the heap
                for (uint64_t i = 0; i < len; ++i) {
                    heap[hptr] = readword();
                    hptr += 1;
                }

                // Aligns on 8-byte boundaries automatically
                // because 64-bit words

                // Push tagged pointer onto the stack
                if (type == VECT) {
                    push((start << VECT_SHIFT) | VECT_TAG);
                } else {
                    push((start << STRG_SHIFT) | STRG_TAG);
                }
                break;
            }
            // ----------------------------------------------
            // UNARY OPERATORS
            // ----------------------------------------------
            case I::ADD1 : {
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
                if (e0 == EMPTY_LIST) {
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
            }
            // ----------------------------------------------
            // BINARY OPERATORS
            // ----------------------------------------------
            case I::ADD : {
                // Tag is LSB 00 so just add
                uint64_t e1 = pop();
                uint64_t e0 = pop();
                #ifdef VERBOSE
                std::cerr << "Adding " << e1 << ", " << e0 << "\n";
                #endif
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
            }
            // ----------------------------------------------
            // LOCALS
            // ----------------------------------------------
            case I::LOADUV : {
                // Get base depth, offset
                uint64_t uvar_dep = readword();
                uint64_t uvar_offset = readword();
                #ifdef VERBOSE
                std::cerr << "dep " << uvar_dep << "; offset " << uvar_offset << "\n";
                #endif
                // Get pointer to base of the variable
                uint64_t var_base = base;
                #ifdef VERBOSE
                std::cerr << "var's base at " << var_base << "; have to go up " << base_dep - uvar_dep << " times; ";
                #endif
                for (uint64_t i = 0; i < base_dep - uvar_dep; ++i) {
                    var_base = stack[var_base];
                    #ifdef VERBOSE
                    std::cerr << "var's base at " << var_base << "; ";
                    #endif
                }
                // Add offset from the var's base
                #ifdef VERBOSE
                std::cerr << "Loading var from loc " << var_base + uvar_offset << "; ";
                #endif
                push(stack[var_base + uvar_offset]);
                break;
            }
            // ----------------------------------------------
            // CONDITIONAL LOGIC
            // ----------------------------------------------
            case I::JUMP : {
                uint64_t offset = readword();
                pc += offset;
                break;
            } case I::JUMPIFFALSE : {
                uint64_t result = pop();
                uint64_t offset = readword();
                if ((result & BOOL_MASK) != BOOL_TAG || (result >> BOOL_SHIFT) == 0) {
                    pc += offset;
                }
                break;
            }
            // ----------------------------------------------
            // PAIRS, LISTS
            // ----------------------------------------------
            case I::CONS : {
                uint64_t cdr = pop();
                uint64_t car = pop();
                heap[hptr] = car;
                heap[hptr + 1] = cdr;

                uint64_t loc_tag = (hptr << PAIR_SHIFT) | PAIR_TAG;
                hptr += 2;
                push(loc_tag);
                break;
            } case I::CAR : {
                uint64_t loc_tag = pop();
                if ((loc_tag & PAIR_MASK) == PAIR_TAG) {
                    uint64_t car = heap[loc_tag >> PAIR_SHIFT];
                    push(car);
                } else {
                    throw std::logic_error("car of non-pair\n");
                }
                break;
            } case I::CDR : {
                uint64_t loc_tag = pop();
                if ((loc_tag & PAIR_MASK) == PAIR_TAG) {
                    uint64_t cdr = heap[(loc_tag >> PAIR_SHIFT) + 1];
                    push(cdr);
                } else {
                    throw std::logic_error("cdr of non-pair\n");
                }
                break;
            }
            // ----------------------------------------------
            // STRINGS
            // ----------------------------------------------
            // TODO: Reduce to char size data
            case I::STROP : {
                uint64_t strlen = readword();
                uint64_t start = hptr;

                heap[hptr] = strlen;
                hptr += 1;
                for (uint64_t i = 0; i < strlen; ++i) {
                    uint64_t c = pop();
                    if ((c & CHAR_MASK) != CHAR_TAG) {
                        throw std::logic_error("string opr expected char\n");
                    }
                    heap[hptr] = (c >> CHAR_SHIFT);
                    hptr += 1;
                }

                push((start << STRG_SHIFT) | STRG_TAG);
                break;
            } case I::STRLEN : {
                uint64_t sptr = pop();
                if ((sptr & STRG_MASK) != STRG_TAG) {
                    throw std::logic_error("string-length expected string target\n");
                }
                sptr = sptr >> STRG_SHIFT;
                push(box_fixnum(heap[sptr]));
                break;
            } case I::STRREF : {
                uint64_t idx = pop();
                uint64_t sptr = pop();
                if ((sptr & STRG_MASK) != STRG_TAG) {
                    throw std::logic_error("string-ref expected string target\n");
                } else if ((idx & FNUM_MASK) != FNUM_TAG) {
                    throw std::logic_error("string-ref expected numeric index\n");
                }

                sptr = sptr >> STRG_SHIFT;
                idx = idx >> FNUM_SHIFT;
                if (heap[sptr] <= idx) {
                    throw std::logic_error("string-ref out of range\n");
                }
                push(heap[sptr + idx + 1]); // Skip length word
                break;
            } case I::STRSET : {
                uint64_t c = pop();
                uint64_t idx = pop();
                uint64_t sptr = pop();
                if ((sptr & STRG_MASK) != STRG_TAG) {
                    throw std::logic_error("string-set! expected string target\n");
                } else if ((idx & FNUM_MASK) != FNUM_TAG) {
                    throw std::logic_error("string-set! expected numeric index\n");
                } else if ((c & CHAR_MASK) != CHAR_TAG) {
                    throw std::logic_error("string-set! expected char setval\n");
                }

                sptr = sptr >> STRG_SHIFT;
                idx = idx >> FNUM_SHIFT;
                if (heap[sptr] <= idx) {
                    throw std::logic_error("string-set! out of range\n");
                }

                heap[sptr + idx + 1] = c;
                push(box_strg(sptr)); // Unspecified ret; chose to ret the str
                break;
            } case I::STRAPP : {
                uint64_t argct = readword();
                uint64_t new_sptr = hptr, new_len = 0;
                uint64_t sptr, len;

                hptr += 1;
                for (uint64_t i = 0; i < argct ; ++i) {
                    sptr = pop();
                    if ((sptr & STRG_MASK) != STRG_TAG) {
                        throw std::logic_error("string-append expected string\n");
                    }
                    sptr = sptr >> STRG_SHIFT;
                    len = heap[sptr]; new_len += len;
                    for (uint64_t j = 0; j < len; ++j) {
                        heap[hptr] = heap[sptr + j + 1];
                        hptr += 1;
                    }
                }
                heap[new_sptr] = new_len;
                push(box_strg(new_sptr)); // Unspecified ret; chose to ret the str
                break;
            }
            // ----------------------------------------------
            // VECTORS
            // ----------------------------------------------
            case I::VECOP : {
                uint64_t veclen = readword();
                uint64_t start = hptr;
                uint64_t val;

                heap[hptr] = veclen;
                hptr += 1;
                for (uint64_t i = 0; i < veclen; ++i) {
                    val = pop();
                    heap[hptr] = (val >> VECT_SHIFT);
                    hptr += 1;
                }

                push((start << VECT_SHIFT) | VECT_TAG);
                break;
            } case I::VECLEN : {
                uint64_t sptr = pop();
                if ((sptr & VECT_MASK) != VECT_TAG) {
                    throw std::logic_error("vector-length expected vector target\n");
                }
                sptr = sptr >> VECT_SHIFT;
                // Expect sptr word contains raw length
                push(box_fixnum(heap[sptr]));
                break;
            } case I::VECREF : {
                uint64_t idx = pop();
                uint64_t sptr = pop();
                if ((sptr & VECT_MASK) != STRG_TAG) {
                    throw std::logic_error("vector-ref expected vector target\n");
                } else if ((idx & FNUM_MASK) != FNUM_TAG) {
                    throw std::logic_error("vector-ref expected numeric index\n");
                }

                sptr = sptr >> VECT_SHIFT;
                idx = idx >> FNUM_SHIFT;
                if (heap[sptr] <= idx) {
                    throw std::logic_error("vector-ref out of range\n");
                }
                push(heap[sptr + idx + 1]); // Skip length word
                break;
            } case I::VECSET : {
                uint64_t val = pop();
                uint64_t idx = pop();
                uint64_t sptr = pop();
                if ((sptr & VECT_MASK) != VECT_TAG) {
                    throw std::logic_error("vector-set! expected vector target\n");
                } else if ((idx & FNUM_MASK) != FNUM_TAG) {
                    throw std::logic_error("vector-set! expected numeric index\n");
                }

                sptr = sptr >> VECT_SHIFT;
                idx = idx >> FNUM_SHIFT;
                if (heap[sptr] <= idx) {
                    throw std::logic_error("vector-set! out of range\n");
                }

                heap[sptr + idx + 1] = val;
                push(box_strg(sptr)); // Unspecified ret; chose to ret the vec
                break;
            } case I::VECAPP : {
                uint64_t argct = readword();
                uint64_t new_sptr = hptr, new_len = 0;
                uint64_t sptr, len;

                hptr += 1;
                for (uint64_t i = 0; i < argct ; ++i) {
                    sptr = pop();
                    sptr = sptr >> VECT_SHIFT;
                    len = heap[sptr]; new_len += heap[sptr];
                    for (uint64_t j = 0; j < len; ++j) {
                        heap[hptr] = heap[sptr + j + 1];
                        hptr += 1;
                    }
                }
                heap[new_sptr] = new_len; // Store raw len
                push(box_vect(new_sptr)); // Unspecified ret; chose to ret the vec
                break;

                break;
            }
            // ----------------------------------------------
            // SIDE EFFECTS
            // ----------------------------------------------
            case I::BEGINPOP : {
                // Clear results of expressions where only side effects matter
                pop();
                break;
            }
            // ----------------------------------------------
            // FRAME MANAGEMENT
            // ----------------------------------------------
            case I::SETBASE : {
                // Start frame with reference to beginning of prev frame
                if (stack.size() != 0) {
                    #ifdef VERBOSE
                    std::cerr << "Setting base\n  ";
                    #endif
                    push(base);
                    base = sptr - 1;
                    #ifdef VERBOSE
                    std::cerr << "  base set to " << base << "\n";
                    #endif
                } else {
                    #ifdef VERBOSE
                    std::cerr << "Setting base\n  ";
                    #endif
                    push(0);
                    base = 0;
                    #ifdef VERBOSE
                    std::cerr << "  base set to " << base << "\n";
                    #endif
                }
                base_dep += 1;
                #ifdef VERBOSE
                std::cerr << "  depth is " << base_dep << "\n";
                #endif
                break;
            } case I::REBASE : {
                // Reset the base pointer to beginning of prev frame
                if (base != 0 && stack.size() != 0) {
                    #ifdef VERBOSE
                    std::cerr << "Rebasing\n  ";
                    #endif
                    // Grab expression result
                    uint64_t result = pop();
                    // Set stack top to where base points (ref to prev)
                    sptr = base;
                    // Grab ref to prev; set as new base
                    base = stack[base];
                    // Overwrite previous ref with result
                    #ifdef VERBOSE
                    std::cerr << "  ";
                    #endif
                    push(result);
                    base_dep -= 1;
                } else {
                    if (base_dep != 0) {
                        base_dep = 0;
                    } else {
                        throw std::logic_error("No base to clear\n");
                    }
                }
                #ifdef VERBOSE
                std::cerr << "  rebased to " << base << "\n";
                #endif
                break;
            }
            // ----------------------------------------------
            // LAMBDAS
            // ----------------------------------------------
            case I::LABELS : {
                // Skip through fxn definition
                uint64_t end = readword();
                pc = end;
                break;
            } case I::LABELCALL : {
                // Store current pc, jump to the function
                uint64_t fxn_loc = readword();
                push(pc);
                pc = fxn_loc + 2; // Plus 2 to skip LABELS and id
                break;
            }
            // ----------------------------------------------
            // OTHER
            // ----------------------------------------------
            case I::RETURN : {
                uint64_t result = pop();
                std::stringstream output;
                print_value(result, heap, output);
                delete[] heap;
                return(output.str());
                break;
            } default : {
                delete[] heap;
                throw std::logic_error("OPCODE ERROR\n");
                break;
            }
        }
    }
    std::cerr << "Fell off the end\n";
}

// ----------------------------------------------------------
// Helpers
void Interpreter::push(uint64_t val) {
    if (sptr < stack.size()) {
        stack[sptr] = val;
    } else {
        stack.push_back(val);
    }
    #ifdef VERBOSE
    std::cerr << "Pushed " << val << " to loc " << sptr << "\n";
    #endif
    sptr += 1;
}

uint64_t Interpreter::pop(void) {
    if (stack.size() == 0) {
        throw std::logic_error("Popped empty stack\n");
    }
    sptr -= 1;
    uint64_t val = stack[sptr];
    #ifdef VERBOSE
    std::cerr << "Popped " << val << " from loc " << sptr << "\n";
    #endif
    return(val);
}

