/**
 * @file errcodes.h
 * @author Joseph Coston & Saugat Sharma
 * @brief contains error code definitions for exiting and error handling macros
 * @version Hw6
 * @date 2022-04-02 
 */
#ifndef ERRCODES_H
#define ERRCODES_H

#include <signal.h>

#define SUCCESSFUL      0
#define ERROR_LEXICAL   1
#define ERROR_SYNTAX    2
#define ERROR_SEMANTIC  3
#define ERROR_MEMORY    4
#define ERROR_TODO      10

extern FILE *op;

#define PRINTERR(...)                  \
    {                                  \
        fprintf(stderr, "\033[31;1m"); \
        fprintf(stderr, __VA_ARGS__);  \
        fprintf(stderr, "\033[0m");    \
    } // macro for error printing

#define MEMCHECK(ptr, ...)                                                     \
    if ((ptr) == NULL)                                                         \
    {                                                                          \
        PRINTERR("MEMORY ERROR %s:%s:%u: ", __FILE__, __FUNCTION__, __LINE__); \
        PRINTERR(__VA_ARGS__);                                                 \
        exit(ERROR_MEMORY);                                                    \
    } // macro to exit with memory error on null memory pointer

#define NEWMEM(dest, type)             \
    type *dest = malloc(sizeof(type)); \
    MEMCHECK(dest, "out of memory") // macro for allocating and then confirming the allocation of a new pointer of passed the type in the passed destination

#define GENERR(...)                                                     \
    {                                                                   \
        PRINTERR("ERROR %s:%s:%u: ", __FILE__, __FUNCTION__, __LINE__); \
        PRINTERR(__VA_ARGS__);                                          \
        exit(ERROR_TODO);                                               \
    } // macro to generate a generic error and exit

// signal handing:
void (*signal(int signal, void (*signalHandler)(int)))(int);

#endif