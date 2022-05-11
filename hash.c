/**
 * @file hash.c
 * @author Saugat Sharma & Joseph Coston
 * @brief defines functions for operating on the symbol table structure and its included hash table
 * @version HW6
 * @date 2022-04-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "hash.h"
#include "errcodes.h"
#include "symt.h"
#include "termargs.h"
#include "tac.h"

char *alloc(int n); /* calloc + check for NULL */
extern int debug;
extern arguments args;

/*
 * new_st - construct symbol (hash) table (UPDATED to meet hw4 requirement).
 *  Allocate space first for the structure, then
 *  for the array of buckets.
 */

SymbolTable new_st(int nbuckets, SymbolTable getParent)
{
    // int h;
    SymbolTable rv;

    int get_size = sizeof(struct SymbolTableEntry *) * nbuckets;

    rv = (SymbolTable)alloc(sizeof(struct SymbolTable));
    rv->tbl = (SymbolTableEntry *)alloc(nbuckets * sizeof(SymbolTableEntry));
    rv->nSubscopes = 0;
    rv->children = NULL;

    if (nbuckets < 1)
    {
        fprintf(stderr, "ERROR!! SymbolTable size can't be less then 1\n");
        return NULL;
    }

    if (getParent != NULL)
    {
        rv->parent = getParent;
        // insert this ST into the scope of its parent
        int idx = getParent->nSubscopes++;
        getParent->children = realloc(getParent->children, sizeof(SymbolTable *) * (idx + 1));
        getParent->children[idx] = rv;
    }
    else
    {
        rv->parent = NULL;
    }

    if ((rv->tbl = malloc(get_size)) == NULL)
    {
        fprintf(stderr, "Error mallocing new SymbolTable\n");
        return NULL;
    }

    for (int i = 0; i < nbuckets; i++)
    {
        rv->tbl[i] = NULL;
    }

    rv->nBuckets = nbuckets;
    rv->nEntries = 0;
    return rv;
}

/*
 * delete_st - destruct symbol table.
 */

/*
 * Compute hash value.
 */
int hash(SymbolTable st, char *s)
{
    register int h = 0;
    register char c;

    while ((c = *s++))
    {
        h += c & 0377;
        h *= 37;
    }
    if (h < 0)
        h = -h;
    return h % st->nBuckets;
}

/*
 * Insert a symbol into a symbol table.
 */
int insert_sym(SymbolTable st, char *s, struct typeptr *t)
{
    // register int i=0;
    int h;
    SymbolTableEntry ste;
    // int l=0;

    h = hash(st, s);

    for (ste = st->tbl[h]; ste != NULL; ste = ste->next)
        if (!strcmp(s, ste->s))
        {
            /*
             * A copy of the string is already in the table.
             */
            return 0;
        }

    /*
     * The string is not in the table. Add the copy from the
     *  buffer to the table.
     */

    ste = (SymbolTableEntry)alloc((unsigned int)sizeof(struct SymbolTableEntry));
    ste->next = st->tbl[h];
    ste->table = st;
    ste->s = strdup(s);
    ste->type = t;

    switch (t->primitive_Type)
    {
    case METHOD_Type: // only ever perform in-situ icode address generation for method declarations
    case MAIN_Type:
        ste->address = newAddr(R_CLASS, 1);
        ste->address->u.name = s;
        break; // WARNING: this may actually break something if there are multiple classes
    case CLASS_Type:
        resetOffset(R_CLASS);
        break;
    default:
        break;
    }

    st->tbl[h] = ste;
    st->nEntries++;
    return 1;
}

/*
 * lookup_st - search the symbol table for a given symbol, return its entry.
 */
SymbolTableEntry lookup_st(SymbolTable st, char *s)
{
    SymbolTableEntry se;

    int h = hash(st, s);
    // PRINTDEBUG("Term '%s' hashes to %d\n",s,h);

    for (se = st->tbl[h]; se != NULL; se = se->next)
        if (!strcmp(s, se->s))
            return se; // Return a pointer to the symbol table entry

    return NULL;
}

/**
 * @brief resursively searches for the passed symbol s in all scopes visible from the passed scope of st
 *
 * @param st symbol table for the scope to search from
 * @param s text name of the symbol to search for
 * @return SymbolTableEntry - the entry in the symtab, if extant and visible from the passed scope
 */
SymbolTableEntry search_st(SymbolTable st, char *s)
{
    SymbolTableEntry target;
    if ((target = lookup_st(st, s)))
    { // return target if found in current scope
        return target;
    }
    else if (st->parent)
    { // resursively call on the parent if one exists and traverse upwards
        return search_st(st->parent, s);
    }
    else
    { // return NULL if the requested symbol has not been found and there are no more superscopes to traverse
        // printf("Error: symbol '%s' is not declared or is outside of scope.\n", s);
        return NULL;
    }
}

char *alloc(int n)
{
    void *a = calloc(n, sizeof(char));
    if (a == NULL)
    {
        fprintf(stderr, "alloc(%d): out of memory\n", (int)n);
        exit(ERROR_MEMORY);
    }
    return a;
}

void printsymbols(SymbolTable st)
{
    printf("--- symbol table for: %s ---\n", st->name);
    SymbolTableEntry ste;
    if (st == NULL)
        return;

    for (int i = 0; i < st->nBuckets; i++)
    {
        // print all the symbols in the symbol table
        for (ste = st->tbl[i]; ste; ste = ste->next)
        {
            print_symbol_entry(ste);
        }
    }
    if (st->nSubscopes > 0 && debug) // print the number of subscopes when debugging
        PRINTVERBO("subscopes: %d\n", st->nSubscopes);

    printf("---\n"); // end of scope indicator

    // recursively visit all nested scope symbol tables for traversal
    for (int i = 0; i < st->nBuckets; i++)
    {
        // visit all the symbols in the symbol table and traverse subscopes
        for (ste = st->tbl[i]; ste; ste = ste->next)
        {
            switch (ste->type->primitive_Type)
            {
            case PACKAGE_Type:
                printsymbols(ste->type->u.PackageType.scope);
                break;
            case CLASS_Type:
                printsymbols(ste->type->u.ClassType.scope);
                break;
            case MAIN_Type:
            case METHOD_Type:
                printsymbols(ste->type->u.MethodType.scope);
                break;
            default:
                break;
            }
        }
    }
}

/**
 * @brief print a single SymbolTableEntry
 *
 * @param ste the entry to print
 */
int print_symbol_entry(SymbolTableEntry ste)
{
    printf("    %s\033[16D\033[17C", ste->s);
    // verbose output
    if (args.verbose)
    {
        printf("type: %s", get_type_string(ste->type->primitive_Type));
        switch (ste->type->primitive_Type)
        {
        case METHOD_Type:
        case MAIN_Type:
            printf("\treturns: %s", get_type_string(ste->type->u.MethodType.returnType->primitive_Type));
            break;
        case ARRAY_Type:
            printf("[%s]", get_type_string(ste->type->u.ArrayType.elemType->primitive_Type));
            break;
        default:
            break;
        }
    }
    printf("\n");
    return ste->type->primitive_Type;
}

/**
 * @brief frees the memory of the passed symbol table and all tables for nested subscopes
 *
 * @param st the symbol table to delete (recursively deletes all subscope symbol tables as well)
 */
void delete_st(SymbolTable st)
{
    SymbolTableEntry se, se1;
    int h;

    // if there are subscopes, free their symbol tables first
    if (st->nSubscopes > 0)
    {
        for (int i = 0; i < st->nSubscopes; i++)
        {
            delete_st(st->children[i]);
        }
    }
    free(st->children);

    // free the table itself
    for (h = 0; h < st->nBuckets; ++h)
    {
        for (se = st->tbl[h]; se != NULL; se = se1)
        {
            se1 = se->next;
            // printf("Freeing: %s\n", se->s);
            free(se->s); /* strings in the table were all strdup'ed. */
            // free_typepointer(se->type);
            free(se->type);
            free(se);
        }
    }
    free(st->tbl);
    free(st);
}
