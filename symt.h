/**
 * @file symt.h
 * @author Saugat Sharma & Joseph Coston
 * @brief defines the names of functions for traversing and populating the symbol table 
 *          and defines types and structures used in the type system
 * @version HW6
 * @date 2022-04-06
 */
#ifndef SYMT_H
#define SYMT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "tree.h"
#include "token.h"
#include "j0.tab.h"
#include "termargs.h"
#include "errcodes.h"

struct SymbolTable* globalST;
struct SymbolTable* currentST;


// default set size 
static const int setClassSize   = 128;
static const int setMethodSize  = 128;
static const int setGlobalSize  = 128;
static const int setPackageSize = 128;

enum types
{
    BOOLLIT_Type,   // 0
    INTLIT_Type,    // 1
    CHARLIT_Type,   // 2
    LONGLIT_Type,   // 3
    FLOATLIT_Type,  // 4
    DOUBLELIT_Type, // 5
    STRINGLIT_Type, // 6
    ARRAY_Type,     // 7
    PACKAGE_Type,   // 8
    IMPORT_Type,    // 9
    CLASS_Type,     // 10
    METHOD_Type,    // 11
    MAIN_Type,      // 12
    VOID_Type,      // 13
    VAR_Type,       // 14
    ID_Type         // 15
};

typedef struct typeptr
{
    enum types primitive_Type; /*stores J0 primitive type name*/

    union
    {
        struct varType_t
        {
            struct typeptr *paramType;
        } VarType;

        struct methodType_t
        {
            struct SymbolTable *scope;
            struct typeptr *returnType;
            struct typeptr **parameterList;
            int numParams;
            // struct
        } MethodType;

        struct arrayType_t
        {
            int size;
            struct typeptr *elemType;
        } ArrayType;

        struct classType_t
        {
            struct SymbolTable *scope;
        } ClassType;

        struct packageType_t
        {
            struct SymbolTable *scope;
        } PackageType;
    } u;
} typeptr;


typeptr *new_type(enum types primitive);
ssize_t getTypeSize(int i);

#define pushscope(stp) do { stp->parent = current; current = stp; } while (0)
#define popscope() do { current = current->parent; } while(0)
void initGlobalST(Tree *parTree);
void contGlobalST(Tree *parTree);
void populate_symboltables(Tree *parTree);
void handleIdentifier(Tree *parTree);
void handleLiteral(int LIT, Tree *parseT);
void handlePackageSpecification(Tree *parTree);
void handlePackageImports(Tree *parTree);
void handlePackageImport(Tree *parTree);
void handlePackageImportInsertion(Tree *parTree);
char *getPackageName(Tree *parTree);
void handleClassBodyDeclaration(Tree *parTree, typeptr *setType);
void handleClassDeclaration(Tree *parTree);
void handleClassBodyDeclarations(Tree *parTree, typeptr *setType);
void handleClassBody(Tree *parTree, typeptr *setType);
void handleArray(Tree *parTree);
void handleArrayVariableDecl(Tree *parTree, typeptr *setType);
int countArrayMembers(Tree *parTree);
void handleInitArrLit(Tree *parTree, typeptr *setType);
void handleExpressionStatement(Tree *parTree);

void handleDeclaredTokens(Tree *parTree);
void handleMethodHeader(Tree *parTree, typeptr *setType);
void handleMethodDeclaration(Tree *parTree, typeptr *setType);
void handleMethodDeclarator(Tree *parTree, typeptr *retType);
void handleMethodCall(Tree *parTree);
void handleMethodParams(Tree *parTree);

void handleArithmeticExpr(Tree *parTree);
void handleLocalVariableDecl(Tree *parTree);
void handleBlock(Tree *parTree, int i);
void handleBlockStatement(Tree *parTree);
void handleLocalVariableDeclStatement(Tree *parTree);


typeptr * handleFormalParam(Tree *parTree, typeptr *setType);

int find_primitive_type(int token);
void semantic_error(char *s, Tree *n);

char *get_type_string(enum types t);

void free_typepointer(typeptr *type);

void dovariabledeclarator(Tree *n);

#endif  
                /* SYMTAB_H */
