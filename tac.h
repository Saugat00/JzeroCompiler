/**
 * @file tac.h
 * @author Joseph Coston & Saugat Sharma
 * @brief Three Address Code - skeleton for CS 423 (course-provided code)
 * @version HW6
 * @date 2022-04-22
 */
#ifndef TAC_H
#define TAC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errcodes.h"
#include "termargs.h"

#define WORD_S sizeof(void*) // the size of a single word (may break on 32-bit machines, IDK)

/**
 * @brief icode memory address type
 * 
 */
typedef struct addr_t
{
    int region; // the region id
    int size;  // the size of the address in WORDs
    union
    {
        int offset; //offset from the region base pointer
        char *name; //name of the address (in the case of methods and classes?)
    } u;
} Addr;

/**
 * @brief 3-address icode instruction type
 * 
 */
typedef struct instr_t
{
    int opcode;
    struct addr_t *dest, *src1, *src2;
    struct instr_t *next;
} Instr;

/**
 * @brief String constant region struct
 * 
 */
typedef struct string_r
{
    int size;
    char *contents;
} stringRegion;

/**
 * @brief icode expression type
 *
 */
typedef struct expr_t
{
    struct instr_t *icode; // pointer to the first icode instuction
    struct addr_t addr;    // address location
} Exp;

/* Regions: */
#define R_GLOBAL 2001 /* can assemble as relative to the pc */
#define R_LOCAL  2002 /* can assemble as relative to the ebp */
#define R_CLASS  2003 /* can assemble as relative to the 'this' register */
#define R_LABEL  2004 /* pseudo-region for labels in the code region */
#define R_CONST  2005 /* pseudo-region for immediate mode constants */
#define R_NAME   2006 /* pseudo-region for source names */
#define R_NONE   2007 /* pseudo-region for unused addresses */
#define R_STRING 2008 /* pseudo-region for string literal constants */

/* Opcodes, per lecture notes */
#define O_ADD   3001
#define O_SUB   3002
#define O_MUL   3003
#define O_DIV   3004
#define O_MOD   3005
#define O_NEG   3006
#define O_ASN   3007
#define O_ADDR  3008
#define O_LCONT 3009
#define O_SCONT 3010
#define O_GOTO  3011
#define O_BLT   3012
#define O_BLE   3013
#define O_BGT   3014
#define O_BGE   3015
#define O_BEQ   3016
#define O_BNE   3017
#define O_BIF   3018
#define O_BNIF  3019
#define O_PARM  3020
#define O_CALL  3021
#define O_RET   3022

/* declarations/pseudo instructions */
#define D_GLOB  4001 // glob x,n:       declare a global named x that has n words of space 
#define D_PROC  4002 // proc x,n1,n2:   declare a procedure named x with n1 words of parameter space and n2 words of local variable space 
#define D_LOCAL 4003 // loc x,n:        use name x to refer to offset n in the local region (the procedure frame); replaces any prior definiton of x that may exist
#define D_LABEL 4004 // lab Ln:         designate that label Ln refers to the next instruction 
#define D_END   4005 // end :           declare the end of the current procedure 
#define D_PROT  4006 // prototype "declaration"

int getOffset(int i);
int nextOffset(int i, int s);
void resetOffset(int i);

Instr *gen(int, Addr *, Addr *, Addr *);
Instr *concat(Instr *, Instr *);
char *regionName(int i);
char *opcodeName(int i);
char *pseudoName(int i);
Addr *genlabel();
Addr *newAddr(int r, int n);
Addr *newStrAddr(char *s);
// Addr *newtmp(int n);
void remtmp(Addr *a);

void printInstr(Instr *i);
void printAddr(Addr *a);
void printStringRegion(void);


#endif
