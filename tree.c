/**
 * @file tree.c
 * @author Saugat Sharma & Joseph Coston
 * @brief making an AST to store parse tokens for second stage of Compiler Development
 * @date 2022-04-06
 * @version HW6
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "tree.h"
#include "j0.tab.h"
#include "termargs.h"

extern Tree *root;
extern int debug;

/**
 * @brief creates AST
 * @param prodr as product rules as interger
 * @param nkids as number of kids in a tree as interger
 * @param ellipse (...) as variable argument functions
 * @returns tree pointer
 */
Tree *createTree(int prodr, int nkids, ...)
{
    // va_list found in <stdarg.h> and used to iterate on ellipsis
    va_list list;

    // defining tree pointer as NULL
    Tree *node = NULL;
    Tree *treePtr = NULL;
    int sizeLen = sizeof(Tree);

    // creating a treePtr struct pointer in the memory
    treePtr = malloc(sizeLen + (nkids - 1) * sizeof(Tree *));
    // treePtr = malloc(sizeof(Tree *)*(nkids-1));

    // checking if malloc is successful
    if (treePtr == NULL)
    {
        fprintf(stderr, "Infficient Tree memory\n");
        exit(1);
    }

    // checking if there are any nkids in tree
    if (nkids == 1)
    {
        // Initialize position of va_list
        va_start(list, nkids);
        // assigning argument of va_arg to tree node
        node = va_arg(list, Tree *);
        // this will end the use of va_list
        va_end(list);
        return node;
    }

    // if there is no kids then return nulls
    if (nkids < 1)
    {
        return NULL;
    }

    // assigning value to the struct
    treePtr->prodrule = prodr;
    treePtr->nkids = nkids;

    va_start(list, nkids);
    // Iterate through every argument in va_list
    for (int i = 0; i < nkids; i++)
    {
        treePtr->kids[i] = va_arg(list, Tree *);
    }
    // this will end the use of va_list
    va_end(list);
    root = treePtr;
    return treePtr;
}

/**
 * @brief print the passed token
 * @param Token *t the token to print
 * @return void
 */
void print_token(Token *t)
{
    printf("\ttoken (%d) on line %d of %s: '%s'\t", t->category, t->lineno, t->filename, t->text);
    switch (t->category)
    {
    case BOOLLIT:
        printf("%s\n", t->literal.ival ? "true" : "false");
        break;
    case CHARLIT:
        printf("%c\n", t->literal.cval);
        break;
    case INTLIT:
        printf("%d\n", t->literal.ival);
        break;
    case LONGLIT:
        printf("%ld\n", t->literal.lval);
        break;
    case FLOATLIT:
        printf("%f\n", t->literal.fval);
        break;
    case DOUBLELIT:
        printf("%f\n", t->literal.dval);
        break;
    case STRINGLIT: // must dynamically allocate memory for strings!
        printf("%s\n", t->literal.sval);
        break;
    default:
        printf("\n");
    }
}

/**
 * @brief prints all the nodes in AST
 * @param tree struct parameter that points to tree node
 * @param depth as interger that gives the depth of the AST
 * @returns 0
 */
int print_tree(Tree *treePtr, int depth)
{
    if (treePtr == NULL)
    {
        if (args.verbose)
            printf("leaf: NULL\n");
        return 0;
    }

    if (treePtr->nkids != 0)
    {
        printf(" %s: %d", humanreadable(treePtr->prodrule), treePtr->nkids);
        printf("\n");
    }
    else
    {
        printf(" leaf: %s", treePtr->leaf->text);
        printf("    type: %d", treePtr->leaf->category);
        printf("\n");
    }

    // prints all the tree nodes
    for (int i = 0; i < treePtr->nkids; i++)
    {
        if (debug)
            printf("%*s %d", depth * 2, " ", i);
        else
            printf("%*s", depth * 2, " ");
        print_tree(treePtr->kids[i], depth + 1);
    }
    return 0;
}

/**
 * @brief prints all the nodes in AST, but more vibrantly
 * @param tree struct parameter that points to tree node
 * @param depth as interger that gives the depth of the AST
 * @returns 0
 */
int print_vibrant_tree(Tree *treePtr, int depth)
{
    if (treePtr == NULL)
    {
        if (args.verbose)
            printf("%*s leaf: \033[36mNULL\033[0m\n", depth * 2, " ");
        return 0;
    }

    if (treePtr->nkids != 0)
    {
        printf("%*s %s: %d", depth * 2, " ", humanreadable(treePtr->prodrule), treePtr->nkids);
        printf("\n");
    }
    else
    {
        printf("%*s leaf: \033[36m %s\033[0m", depth * 2, " ", treePtr->leaf->text);
        printf("    type: %d", treePtr->leaf->category);
        if (treePtr->leaf->category == STRINGLIT)
            PRINTDEBUG("    value: %s", treePtr->leaf->literal.sval);
        printf("\n");
    }

    // prints all the tree nodes
    for (int i = 0; i < treePtr->nkids; i++)
    {
        print_vibrant_tree(treePtr->kids[i], depth + 1);
    }
    return 0;
}

/**
 * @brief helper function for printing symbols
 * @param char *s takes the token to print
 * @return void
 */
void printsymbol(char *s)
{
    printf("%s\n", s);
    fflush(stdout);
}

/**
 * @brief helper function to determine whether a production rule is an array optional or not
 *
 * @param prod the production rule code
 * @return int 1 if prod is an array optional, 0 otherwise
 */
int isArrOpt(int prod)
{
    switch (prod)
    {
    case intArrOpt:
    case boolArrOpt:
    case charArrOpt:
    case longArrOpt:
    case floatArrOpt:
    case doubleArrOpt:
    case stringArrOpt:
        return 1;
    default:
        return 0;
    }
}

/**
 * @brief function for tree traversal for lab6
 * @param Tree *t takes the token to print
 * @return void
 */
void printsyms(Tree *t)
{
    if (t)
    {
        if (t->nkids > 0)
        {
            for (int i = 0; i < t->nkids; i++)
            {
                printsyms(t->kids[i]);
            }
        }
        else
        {
            if (t->leaf->category == ID)
                printsymbol(t->leaf->text);
        }
    }
    else
    {
        printf("Error: passed tree pointer is NULL");
    }
}

/**
 * @brief recursively traverse and free all nodes in the parse tree
 *  //TODO needs work
 * @param n the root node of the parse tree
 */
void free_tree(Tree *n)
{
    if (n->nkids > 1)
    { // if there are children, free them first
        Tree *child = n->kids[0];
        for (int i = 1; i < n->nkids; child = n->kids[i++])
            free_tree(child);
        free(child->symbol);
        free(child->leaf);
        free(child);
    }
    else
    { // node is a leaf, so free it
        free(n->leaf->text);
        free(n->leaf);
        return;
    }
}

char *humanreadable(int prod)
{
    // char *ret = malloc(32);
    switch (prod)
    {
    case Identifier:
        return "Identifier";
    case ClassDeclaration:
        return "Class Declaration";
    case ClassBody:
        return "Class Body";
    case ClassBodyDeclarations:
        return "Class Body Declarations";
    case ClassBodyDeclaration:
        return "Class Body Declaration";
    case FieldDeclaration:
        return "Field Declaration";
    case Type:
        return "Type";
    case Types:
        return "Types";
    case ArraySubscript:
        return "Array Subscript";
    case Name:
        return "Name";
    case VarDecls:
        return "Variable Declarations";
    case VarDeclarator:
        return "Variable Declarator";
    case MethodReturnVal:
        return "Method Return Value";
    case MethodDecl:
        return "Method Declaration";
    case MethodHeader:
        return "Method Header";
    case MethodDeclarator:
        return "Method Declarator";
    case FormalParm:
        return "Formal Parameter";
    case FormalParmListOpt:
        return "Optional Formal Parameter List ";
    case FormalParmList:
        return "Formal Parameter List";
    case ConstructorDecl:
        return "Constructor Declaration";
    case Block:
        return "Block";
    case BlockStmtsOpt:
        return "Optional Block Statements ";
    case BlockStmts:
        return "Block Statements";
    case BlockStmt:
        return "Block Statement";
    case LocalVarDeclStmt:
        return "Local Variable Declaration Statement";
    case LocalVarDecl:
        return "Local Variable Declaration";
    case Stmt:
        return "Statement";
    case ExprStmt:
        return "Expression Statement";
    case StmtExpr:
        return "Statement Expression";
    case IfThenStmt:
        return "If-Then Statement";
    case IfThenElseStmt:
        return "If-Then-Else Statement";
    case IfThenElseStmtNoShortIf:
        return "If-Then-Else Statement No Short If";
    case StmtNoShortIf:
        return "Statement No Short If";
    case ForStmtNoShortIf:
        return "For Statement No Short If";
    case WhileStmtNoShortIf:
        return "While Statement No Short If";
    case WhileStmt:
        return "While Statement";
    case ForStmt:
        return "For Statement";
    case ForInit:
        return "For Initialization Clause";
    case ExprOpt:
        return "Optional Expression";
    case ForUpdate:
        return "For Update Clause";
    case StmtExprList:
        return "Statement Expression List";
    case BreakStmt:
        return "Break Statement";
    case ReturnStmt:
        return "Return Statement";
    case Primary:
        return "Primary";
    case Literal:
        return "Literal";
    case ArgList:
        return "Argument List";
    case FieldAccess:
        return "Field Access";
    case ArgListOpt:
        return "Optional Argument List";
    case MethodCall:
        return "Method Call";
    case PostFixExpr:
        return "Post Fix Expression";
    case UnaryExpr:
        return "Unary Expression";
    case MulExpr:
        return "Multiplication Expression";
    case AddExpr:
        return "Add Expression";
    case RelOp:
        return "Relation Operation";
    case RelExpr:
        return "Relation Expression";
    case EqExpr:
        return "Eqation Expression";
    case CondAndExpr:
        return "Conditional And Expression";
    case CondOrExpr:
        return "Conditional Or Expression";
    case Expr:
        return "Expression";
    case Assignment:
        return "Assignment";
    case AssignOp:
        return "Assignment Operation";
    case SwitchStatement:
        return "Switch Statement";
    case SwitchBody:
        return "Switch Body";
    case SwitchCase:
        return "Switch Case";
    case SwitchDefault:
        return "Switch Default";
    case LeftHandSide:
        return "Left Hand Side";
    case QualifiedName:
        return "Qualified Name";
    case InstantiationExpr:
        return "Instantiation Expression";
    case ArrVarDeclarator:
        return "Array Variable Declarator";
    case intArrLit:
        return "Int Array Literal";
    case intArrOpt:
        return "Int Array Optional";
    case ArrLiteral:
        return "Array Literal";
    case NullArr:
        return "Null Array";
    case intLit:
        return "Initialize Literal";
    case longLit:
        return "Long Literal";
    case longArrLit:
        return "Long Array Literal";
    case longArrOpt:
        return "Long Array Optional";
    case charLit:
        return "Char Literal";
    case charArrLit:
        return "Char Array Literal";
    case charArrOpt:
        return "Char Array Optional";
    case floatLit:
        return "Float Literal";
    case floatArrLit:
        return "Float Array Literal";
    case floatArrOpt:
        return "Float Array Optional";
    case doubleLit:
        return "Double Literal";
    case doubleArrLit:
        return "Double Array Literal";
    case doubleArrOpt:
        return "Double Array Optional";
    case stringLit:
        return "String Literal";
    case stringArrLit:
        return "String Array Literal";
    case stringArrOpt:
        return "String Array Optional";
    case booleanLit:
        return "Boolean Literal";
    case boolArrOpt:
        return "Boolean Array Optional";
    case packageDeclaration:
        return "Package Declaration";
    case packageName:
        return "Package Name";
    case importDeclarations:
        return "Import Declarations";
    case importDeclaration:
        return "Import Declaration";
    case singleTypeImportDeclaration:
        return "Single Type Import Declaration";
    case typeImportOnDemandDeclaration:
        return "Type Import On Demand Declaration";
    case typeDeclaration:
        return "Type Declaration";
    case compilationUnit:
        return "Compilation Unit";
    case typeName:
        return "Type Name";
    case SwitchCases:
        return "Switch Cases";
    case SwitchCaseBlock:
        return "Switch Case Block";
    case CreateArray:
        return "Create Array";

    case InitializeArrayElement:
        return "Initialize Array Element";
    case AssignArrayElement:
        return "Assign Array Element";

    default:
        // snprintf(ret, 32, "nondescript token (%d)", prod);
        return "nondescript token";
    }
}
