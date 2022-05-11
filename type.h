/**
 * @file type.h
 * @author Joseph Coston & Saugat Sharma
 * @brief outlines the functions used in the type-checking phase of semantic analysis.
 * @version HW6
 */
#ifndef TYPE_H
#define TYPE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "tree.h"
#include "token.h"
#include "symt.h"

struct SymbolTable *TypeCurrent;
void TypeCheck_InitGlobal(Tree *root);
void typeCheck(Tree *parTree);
void checkClassBodyDecl(Tree *parTree);

void changeMethodScope(Tree *parTree);
void changeClassScope(Tree *parTree);
void changePackageScope(Tree *parTree);
void checkClassVarDecl(Tree *parTree);

void checkReturnStatement(Tree *parTree);
void checkBlockTypes(Tree *parTree);
void checkAssignmentExprType(Tree *parTree, SymbolTableEntry FindLeftType);
void checkType(typeptr *leftType, typeptr *rightType, Tree *parTree);
void checkAssignmentType(Tree *parTree);
void checkLocVarDecType(Tree *parTree);

void checkMethodCall(Tree *parTree);
void checkMethodParams(Tree *parTree, SymbolTableEntry func, int lineno, int paramIndex);
char *getName(Tree *parTree);
int getComplexParameterType(Tree *parTree);

void checkInitlArrElemType(Tree *parTree);
void checkArrayDeclTypes(Tree *parTree);
void checkCondStatTypes(Tree *parTree);
void checkCondExpTypes(Tree *parTree);
void checkForStatementTypes(Tree *parTree);
void checkSwitchStatementTypes(Tree *parTree);
void checkSwitchBody(Tree *parTree, typeptr *switchExpType);
void checkSwitchCaseBlock(Tree *parTree, typeptr *switchExpType);

#endif
