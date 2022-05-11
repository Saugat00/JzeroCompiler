/**
 * @file builtins.c
 * @author Joseph Coston & Saugat Sharma
 * @brief contains logic for implementing and inserting symbols for j0 built-in functions
 * @version HW6
 * @date 2022-04-08
 */
#include "builtins.h"
#include <stdio.h>

extern char *yyfilename;

/**
 * @brief Converts all of the package imports in the globalST into function entries,
 *      throwing errors if the package import is not a built-in.
 *
 */
void convertPackageImports(void)
{
    PRINTDEBUG("Converting package imports to functions:\n");
    SymbolTableEntry ste;
    if (globalST == NULL)
    {
        PRINTERR("SYMTAB ERROR: unable to convert package imports in NULL global Symbol Table.\n");
        exit(ERROR_MEMORY);
    }
    // loop over all the symbol entries in the golbalST
    for (int i = 0; i < globalST->nBuckets; i++)
    {
        for (ste = globalST->tbl[i]; ste; ste = ste->next)
        {
            if (ste->type->primitive_Type == IMPORT_Type)
            {
                DEBUG(print_symbol_entry(ste));
                if (strcmp(ste->s, "System.out.print") == 0)
                {
                    load__System_out_print();
                }
                else if (strcmp(ste->s, "System.out.println") == 0)
                {
                    load__System_out_println();
                }
                else if (strcmp(ste->s, "System.in.read") == 0)
                {
                    load__System_in_read();
                }
                else if (strcmp(ste->s, "Character.toString") == 0)
                {
                    load__Character_toString();
                }
                else if (strcmp(ste->s, "Integer.toString") == 0)
                {
                    load__Integer_toString();
                }
                else if (strcmp(ste->s, "string.length") == 0)
                { // NOTE: We actually check for "string.length" due to "String" being a reserved word...
                    load__String_length();
                }
                else if (strcmp(ste->s, "string.charAt") == 0)
                {
                    load__String_charAt();
                }
                else if (strcmp(ste->s, "string.equals") == 0)
                {
                    load__String_equals();
                }
                else if (strcmp(ste->s, "string.compareTo") == 0)
                {
                    load__String_compareTo();
                }
                else
                { // the last case is if the package import is not implemented by j0 as a built-in function
                    PRINTERR("IMPORT ERROR in %s: Package '%s' does not contain a method currently implemented by j0.\n", yyfilename, ste->s);
                    exit(ERROR_LEXICAL);
                }
            }
        }
    }
}

/**
 * @brief generates a symbol table entry for j0's System.out.println builtin function
 *
 */
void load__System_out_println(void)
{
    int numParams = 1;
    char *funcName = "System.out.println";
    // allocate memory for the typeptrs
    typeptr *methodType, *returnType, *param0Type;
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL || (param0Type = malloc(sizeof(struct typeptr))) == NULL)
    {
        PRINTERR("Error alocating memory for typeptr in load__System_out_println().\n");
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL)
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    // set method types
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = STRINGLIT_Type;
    returnType->primitive_Type = VOID_Type;

    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type;
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}

/**
 * @brief generates a symbol table entry for j0's System.out.print builtin function
 *
 */
void load__System_out_print(void)
{
    int numParams = 1;
    char *funcName = "System.out.print";
    // allocate memory for the typeptrs
    typeptr *methodType, *returnType, *param0Type;
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL || (param0Type = malloc(sizeof(struct typeptr))) == NULL)
    {
        PRINTERR("Error alocating memory for typeptr in load__System_out_print().\n");
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL)
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    // set method types
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = STRINGLIT_Type;
    returnType->primitive_Type = VOID_Type;

    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type;
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}

/**
 * @brief generates a symbol table entry for j0's System.in.read builtin function
 *
 */
void load__System_in_read(void)
{
    int numParams = 0;
    char *funcName = "System.in.read";
    // allocate memory for the typeptrs
    typeptr *methodType, *returnType, *param0Type;
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL || (param0Type = malloc(sizeof(struct typeptr))) == NULL)
    {
        PRINTERR("Error alocating memory for typeptr in load__System_in_read().\n");
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL)
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    // set method types
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = VOID_Type;
    returnType->primitive_Type = CHARLIT_Type;

    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type;
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}

/**
 * @brief generates a symbol table entry for j0's Character.toString builtin function
 *
 */
void load__Character_toString(void)
{
    int numParams = 1;
    char *funcName = "Character.toString";
    typeptr *methodType, *returnType, *param0Type;
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL || (param0Type = malloc(sizeof(struct typeptr))) == NULL)
    {
        PRINTERR("Error alocating memory for typeptr in load__Character_toString().\n");
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL)
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = CHARLIT_Type;
    returnType->primitive_Type = STRINGLIT_Type;
    // set method types
    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type;
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}

/**
 * @brief generates a symbol table entry for j0's Integer.toString builtin function
 *
 */
void load__Integer_toString(void)
{
    int numParams = 1;
    char *funcName = "Integer.toString";
    typeptr *methodType, *returnType, *param0Type;
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL || (param0Type = malloc(sizeof(struct typeptr))) == NULL)
    {
        PRINTERR("Error alocating memory for typeptr in load__Integer_toString().\n");
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL)
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = INTLIT_Type;
    returnType->primitive_Type = STRINGLIT_Type;
    // set method types
    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type;
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}

/**
 * @brief generates a symbol table entry for j0's String.length builtin function
 *
 */
void load__String_length(void)
{
    int numParams = 1;
    char *funcName = "string.length";
    typeptr *methodType, *returnType, *param0Type;
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL || (param0Type = malloc(sizeof(struct typeptr))) == NULL)
    {
        PRINTERR("Error alocating memory for typeptr in load__String_length().\n");
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL)
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = STRINGLIT_Type;
    returnType->primitive_Type = INTLIT_Type;
    // set method types
    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type;
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}

/**
 * @brief generates a symbol table entry for j0's String.charAt builtin function
 *
 */
void load__String_charAt(void)
{
    int numParams = 2;
    char *funcName = "string.charAt";
    typeptr *methodType, *returnType, *param0Type, *param1Type;
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL ||
        (param0Type = malloc(sizeof(struct typeptr))) == NULL ||
        (param1Type = malloc(sizeof(struct typeptr))) == NULL)
    {
        PRINTERR("Error alocating memory for typeptr in load__String_charAt().\n");
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL)
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = STRINGLIT_Type;
    param1Type->primitive_Type = INTLIT_Type;
    returnType->primitive_Type = CHARLIT_Type;
    // set method types
    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type;
    methodType->u.MethodType.parameterList[1] = param1Type;
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}

/**
 * @brief generates a symbol table entry for j0's String.equals builtin function
 *
 */
void load__String_equals(void)
{
    int numParams = 2;
    char *funcName = "string.equals";
    typeptr *methodType, *returnType, *param0Type, *param1Type;
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL ||
        (param0Type = malloc(sizeof(struct typeptr))) == NULL ||
        (param1Type = malloc(sizeof(struct typeptr))) == NULL)
    {
        PRINTERR("Error alocating memory for typeptr in load__String_equals().\n");
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL)
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = STRINGLIT_Type;
    param1Type->primitive_Type = STRINGLIT_Type;
    returnType->primitive_Type = BOOLLIT_Type;
    // set method types
    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type;
    methodType->u.MethodType.parameterList[1] = param1Type;
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}

/**
 * @brief generates a symbol table entry for j0's String.compareTo builtin function
 *
 */
void load__String_compareTo(void)
{
    int numParams = 2;
    char *funcName = "string.compareTo";
    typeptr *methodType, *returnType, *param0Type, *param1Type;
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL ||
        (param0Type = malloc(sizeof(struct typeptr))) == NULL ||
        (param1Type = malloc(sizeof(struct typeptr))) == NULL)
    {
        PRINTERR("Error alocating memory for typeptr in load__String_compareTo().\n");
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL)
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = STRINGLIT_Type;
    param1Type->primitive_Type = STRINGLIT_Type;
    returnType->primitive_Type = INTLIT_Type;
    // set method types
    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type;
    methodType->u.MethodType.parameterList[1] = param1Type;
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}

/*
String.toString(i) vs. String.valueOf()  ??
*/

/* prototype for making static builtins:

void load__Builtin_name(void) // CHANGE THIS
{
    int numParams = 1;                                                                                                  // CHANGE THIS
    char *funcName = "builtin.name";                                                                                    // CHANGE THIS
    typeptr *methodType, *returnType, *param0Type;                                                                      // CHANGE THESE
    if ((returnType = malloc(sizeof(struct typeptr))) == NULL || (param0Type = malloc(sizeof(struct typeptr))) == NULL) // CHANGE THESE
    {
        PRINTERR("Error alocating memory for typeptr in load__System_out_println().\n"); // CHANGE THE FUNCTION NAME TO MATCH
        exit(ERROR_MEMORY);
    }
    SymbolTableEntry targetSTE = lookup_st(globalST, funcName);
    if (targetSTE == NULL )
    {
        PRINTERR("Error finding global symbol table entry for '%s' package import.\n", funcName);
        exit(ERROR_MEMORY);
    }
    methodType = targetSTE->type;
    // modify the package import symbol entry
    methodType->primitive_Type = METHOD_Type;
    param0Type->primitive_Type = STRINGLIT_Type; // CHANGE THIS
    returnType->primitive_Type = VOID_Type;      // CHANGE THIS
    // set method types
    methodType->u.MethodType.numParams = numParams;
    methodType->u.MethodType.parameterList = malloc(sizeof(typeptr) * numParams);
    methodType->u.MethodType.parameterList[0] = param0Type; // CHANGE THIS OR ADD MORE AS NEEDED FOR ADDITIONAL PARAMETERS
    methodType->u.MethodType.returnType = returnType;
    methodType->u.MethodType.scope = new_st(1, globalST);
    methodType->u.MethodType.scope->name = funcName;
}
*/