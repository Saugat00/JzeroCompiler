/**
 * @file main.c
 * @author Saugat Sharma & Joseph Coston
 * @brief Main launcher for the J0 compiler writen for NMT CSE 423 Compiler Design.
 * @version HW6
 * @date 2022-04-06
 *
 * 
 * @todo:
 *      -Create a struct for working with lists of 3-address instructions. 
 *          -Define integers for each opcode. 
 *          -Implement operations to create a new list with one instruction, and list concatenation.
 *      -Include separate instructions/opcodes for each operator, for each operand type combination that it supports.
 *      -Define a data type for "memory address". A memory address is a <region,offset> pair, where region is one of GLOBAL, STRUCT, PARAM, LOCAL, CONST, or IMMED. Offsets start with 0 in each region. You can define more regions if you wish.
 *      -Write out "declarations" (pseudocode instructions) for functions.
 *      -For each variable in each symbol table, assign it a memory address. Compute offsets assuming ints, pointers, and array/map/struct references require 8 bytes.
 *      -Compute a synthesized attribute "location" (or .place) for every expression.
 *      -Allocate a "temporary variable" out of the LOCAL region for each value computed by an operator or function invocation.
 *      -Compute a synthesized attribute "code" that builds a link list of 3-address instructions that compute a value for each expression. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include "tree.h"
#include "symt.h"
#include "type.h"
#include "termargs.h"
#include "builtins.h"
#include "tac.h"
#include "icode.h"

// global external variables used by Flex/Bison-generated logic
extern FILE *yyin;
extern int yylex();
extern int yyparse();
extern char *yytext;
extern int yylineno;
char *yyfilename = NULL;

extern Token *t;
extern Tree *root;
extern struct SymbolTable *globalST;
extern struct SymbolTable *currentST;

// terminal argument
arguments args;
int debug; // global debug flag set in terminal with -d


/**
 * @brief catch segfaults before they are passed to the system with this function
 *
 * @param signal the system signal id
 */
void exit_handler(int signal)
{
    switch (signal)
    {
    case SIGSEGV:
        PRINTERR("Memory Error (segfault)\n");
        // void *bt[10];
        // backtrace(bt,10);
        // printf("%s\n", backtrace_symbols(bt,10)[0]);
        exit(ERROR_MEMORY);
        break;
    default:
        PRINTERR("SIGNAL %d RECEIVED\n", signal);
        exit(signal);
        break;
    }
}

/**
 * @brief frees the memory consumed by the program
 *
 */
void freemem(void)
{
    // delete_st(globalST); // free the symbol table
    free_tree(root); // free the parse tree
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV, exit_handler); // the system level signal monitor
    FILE *fp;                      // file pointer for currently compiling java file
    int length = 0;                // length of the filename
    char *filename = NULL;         // filename pointer fr the current java file
    args = get_args(argc, argv);   // parse arguments from terminal
    globalST = NULL;               // initialize both symbol tables to NULL
    currentST = NULL;

    // loop over all provided filenames
    for (int i = 0; i < args.filecount; i++)
    {
        filename = args.filenames[i];
        length = strlen(filename);

        // forcing program just to accept files with java extension
        if (strcmp(((filename + length) - 5), ".java") != 0)
        {
            char *tmpfilename = filename;
            filename = (char *)malloc(length + 6);
            strncpy(filename, tmpfilename, length + 6);
            strcat(filename, ".java");
        }
        // verbose output if -v flag set in terminal
        PRINTVERBO("Loading source code: %s\n", filename);

        // attempt to use the passed command line arguments to open specified files
        yyfilename = filename;
        if ((fp = fopen(yyfilename, "r")) != NULL)
        {
            yyin = fp;
            yyparse();
            PRINTDEBUG("PARSE PASSED for %s\n", yyfilename);

            // print the parse tree if -t flag set in terminal
            if (args.tree)
            {
                PRINTVERBO("Parse tree for %s:\n", yyfilename);
                if (args.verbose)
                    print_vibrant_tree(root, 0);
                else
                    print_tree(root, 0);
            }

            // generate the symbol tables for the parse tree
            if (globalST)
            { // if the global symbol table is not NULL, contribute to the existing global table
                contGlobalST(root);
            }
            else
            { // if root is uninitialized, this is the first java file
                initGlobalST(root);
            }
            PRINTDEBUG("SYMTAB POPULATION PASSED for %s\n", yyfilename);

            // attempt to fulfill package import needs with j0 builtin functions
            // convertPackageImports();
            // printsymbols(globalST); //for debug!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

            // perform semantic analysis
            // TypeCheck_InitGlobal(root);
        }
        else
        {
            PRINTERR("SYSTEM ERROR: Unable to open %s: No such file or directory.\n", yyfilename);
            exit(-1);
        }

        // print the symbol table if -s flag set in terminal
        if (args.symtab)
        {
            PRINTVERBO("Symbol Tables:\n");
            printsymbols(globalST);
        }

        generate_intermediate_code(); /** @todo finish implementing this function for generating  *.ic files **/

        // free the pointers and file streams when finished
        // free(filename);
        fclose(fp);
        PRINTDEBUG("\n");
    }


    // free up all the used memory
    // if (debug) freemem();
    PRINTVERBO("Passed!\n")
    return 0;
}