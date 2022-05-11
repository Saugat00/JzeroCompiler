/**
 * @file tree.c
 * @author Saugat Sharma & Joseph Coston
 * @brief making an AST to store parse tokens for second stage of Compiler Development
 * @date 2022-04-06
 * @version HW6
 */
#ifndef TREE_H
#define TREE_H
#include "token.h"
#include "tac.h"

typedef struct tree
{
  int id;
  char *symbol;
  int prodrule;
  int nkids;
  Token *leaf;
  struct tree *kids[9];
  
  Addr *address;
  Addr *onTrue;     // OnTrue and OnFalse will hold labels to use as targets when Boolean exp is found true/false.
  Addr *onFalse;
  
  Instr *icode;
  // Instr *code; // might need this later...  Instr *code;

} Tree;

Tree *createTree(int prodrule, int nkids, ...);
Tree *createInternal(int prodrule, int nkids, ...);
Tree *createLeaf(int prodrule, int nkids, ...);
void free_tree(Tree *n);
int print_tree(Tree *t, int depth);
int print_vibrant_tree(Tree *treePtr, int depth);
int isArrOpt(int prod);
char *humanreadable(int prod);

#endif
