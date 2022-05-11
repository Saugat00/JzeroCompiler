/**
 * @author Joseph Coston & Saugat Sharma
 * @version CSE 423 Homework 6
 * @brief used to parse more complex command line arguments 
 *      and contains macros for more efficiently generating 
 *      more advanced printer functions that are reused often
 * @date 2022-04-06
 * adapted from the example at https://www.linuxtopia.org/online_books/programming_books/gnu_c_programming_tutorial/argp-example.html
 */

#ifndef _TERMARGS_
#define _TERMARGS_
#define MAX_FNAME_LEN 64
#include <argp.h>

extern FILE *op;  // global output file pointer
extern int debug; // global debug flag

#define PRINTDEBUG(...)      \
    if (debug)               \
    {                        \
        printf("\033[33m");  \
        printf(__VA_ARGS__); \
        printf("\033[0m");   \
    } // macro for printing single debug messages (use like a conditional printf that only works when the -d flag is applied)
#define PRINTVERBO(...)      \
    if (args.verbose)        \
    {                        \
        printf("\033[32m");  \
        printf(__VA_ARGS__); \
        printf("\033[0m");   \
    } // macro for printing verbose output messages (use like a conditional printf that only works when the -v flag is applied)
#define PRINTOFILE(...)           \
    {                             \
        if (debug)                \
        {                         \
            printf("\033[36m");   \
            printf(__VA_ARGS__);  \
            printf("\033[0m");    \
        }                         \
        fprintf(op, __VA_ARGS__); \
    } // macro for printing to output file, printing to terminal only when debug active
#define DEBUG(code)         \
    if (debug)              \
    {                       \
        printf("\033[33m"); \
        code;               \
        printf("\033[0m");  \
    } // macro for debugging blocks of code

/* This structure is used by main to communicate with parse_opt. */
typedef struct arguments_t
{
    char **filenames;  /* array of atring pointers */
    int firstfilename; /* index of the first filename in the array of arguments */
    int filecount;     /* the number of filenames parsed */
    int tree;          /* the --tree flag */
    int symtab;        /* the --symtab flag */
    int verbose;       /* the verbose output flag */
} arguments;
extern arguments args; // the global arguments struct

// functions
error_t parse_opt(int key, char *arg, struct argp_state *state);
arguments get_args(int argc, char *argv[]);

#endif