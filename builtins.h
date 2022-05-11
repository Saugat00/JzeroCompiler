/**
 * @file builtins.c
 * @author Joseph Coston & Saugat Sharma
 * @brief contains logic for implementing and inserting symbols for j0 built-in functions
 * @version HW6
 * @date 2022-04-08
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symt.h"

#ifndef _BUILTINS_
#define _BUILTINS_

void convertPackageImports(void);

// loaders for builtin functions
void load__System_out_print(void);
void load__System_out_println(void);
void load__System_in_read(void);
void load__Character_toString(void);
void load__Integer_toString(void);

// weird String OOP-spoofing functions
void load__String_length(void);
void load__String_charAt(void);
void load__String_equals(void);
void load__String_compareTo(void);

typedef struct builtIn_t
{
    char *name;
    char **paramTypes;
    char *returnType;
} builtIn;

#endif