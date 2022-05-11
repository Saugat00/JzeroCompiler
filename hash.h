/**
 * @file hash.h
 * @author Saugat Sharma & Joseph Coston
 * @brief headder for hash table functionality
 * @version HW6
 * @date 2022-04-06
 */
#ifndef HASH_H
#define HASH_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symt.h"
#include "tac.h"

typedef struct SymbolTable
{
   int nBuckets;                  /* # of buckets */
   int nEntries;                  /* # of symbols in the table */
   int nSubscopes;                /* # of scopes held within this one */
   struct SymbolTable *parent;    /* enclosing scope, superclass etc. */
   struct SymbolTable **children; /* the array of symbol tables held within this scope */
   struct SymbolTableEntry **tbl; /* the array of symbol entries */
   char *name;                    /*name of the current scope*/
   /* more per-scope/per-symbol-table attributes go here */
} * SymbolTable;

/*
 * Entry in symbol table.
 */
typedef struct SymbolTableEntry
{
   SymbolTable table;    /* what symbol table do we belong to*/
   char *s;              /* string */
   struct typeptr *type; /* the entry's type pointer (contains type and initialization info) */
   Addr *address;        /* the address used to store the entry during code generation */
   /* more symbol attributes go here for code generation */
   struct SymbolTableEntry *next;

} * SymbolTableEntry;



/*
 * Prototypes
 */
SymbolTable new_st(int size, SymbolTable getParent);        /* create symbol(hash) table */
int insert_sym(SymbolTable st, char *s, struct typeptr *t); /* enter symbol into table */
int hash(SymbolTable st, char *s);                          /* hash function for the symbol table */
SymbolTableEntry lookup_st(SymbolTable, char *);            /* lookup symbol in the passed symbol table */
SymbolTableEntry search_st(SymbolTable, char *);            /* search symbol in all symbol tables visible from the passed one */
int print_symbol_entry(SymbolTableEntry ste);               /* print a single symbol table entry */
void delete_st(SymbolTable st);                             /* destroy symbol table */
extern void printsymbols(SymbolTable st);                   /* printer function for the symbol table */

#endif