/**
 * @file icode.h
 * @author Joseph Coston & Saugat Sharma
 * @brief outlines logic for the generation of intermediate code from the parse tree and symbol table
 * @version HW6
 * @date 2022-04-22
 */
#ifndef ICODE_H
#define ICODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errcodes.h"
#include "termargs.h"
#include "tree.h"
#include "token.h"
#include "type.h"
#include "hash.h"
#include "symt.h"
#include "tac.h"

#define UNKNOWNTREE(...)                                                                                                                   \
    {                                                                                                                                      \
        PRINTDEBUG("WARNING %s:%s:%u: unable to handle tree structure without known implementation.\n", __FILE__, __FUNCTION__, __LINE__); \
        PRINTDEBUG(__VA_ARGS__);                                                                                                           \
        DEBUG(print_tree(t, 10));                                                                                                          \
    } // macro devs can use to warn themselves when the code encounteres unimplemented functionality in the syntax tree

// external global variables
extern char *yyfilename;
extern Tree *root;
extern struct SymbolTable *globalST;
extern struct SymbolTable *currentST;
extern stringRegion string_constant_region;

void generate_intermediate_code(void);


#endif
