/**
 * @file symt.c
 * @author Saugat Sharma & Joseph Coston
 * @brief defines the functions for traversing and populating the symbol table
 * @version HW6
 * @date 2022-04-06
 */
#include "symt.h"
#include "type.h"
#include "termargs.h"

extern int errors;
extern char *yyfilename;

// defined sizes of literal values, corresponding tho the types enum indexes from symt.h
ssize_t typeSizes[] = {
    sizeof(int),     // boolean type size
    sizeof(int),     // int type size
    sizeof(char),    // char type size
    sizeof(long),    // long type size
    sizeof(float),   // float type size
    sizeof(double),  // double type size
    sizeof(char *),  // string pointer type size
    sizeof(void *)}; // generic pointer type size (for debug)
ssize_t getTypeSize(int i) { return typeSizes[i]; }

typeptr *new_type(enum types primitive)
{
    // allocate the new typeptr
    typeptr *new = NULL;
    if ((new = malloc(sizeof(typeptr))))
    { // set the primitive_Type of the new typeptr and return
        new->primitive_Type = primitive;
        return new;
    }
    else
    { // fatal error if failed to allocate memory
        PRINTERR("Error: failed to allocate typepointer.\n");
        exit(ERROR_MEMORY);
    }
}

void populate_symboltables(Tree *parTree)
{
    int i;
    if (parTree == NULL)
        return;
    /* pre-order activity */

    switch (parTree->prodrule)
    {
    case ClassDeclaration:
        handleClassDeclaration(parTree);
        break;
    // case MethodCall:
    //     PRINTDEBUG("Checking method call...\n");
    //     handleMethodCall(parTree);
    //     break;
    case Identifier:
        handleIdentifier(parTree);
        break;
    case BOOLLIT:
        handleLiteral(BOOLLIT, parTree);
        break;
    case CHARLIT:
        handleLiteral(CHARLIT, parTree);
        break;
    case INTLIT:
        handleLiteral(INTLIT, parTree);
        break;
    case LONGLIT:
        handleLiteral(LONGLIT, parTree);
        break;
    case FLOATLIT:
        handleLiteral(FLOATLIT, parTree);
        break;
    case DOUBLELIT:
        handleLiteral(DOUBLELIT, parTree);
        break;
    case STRINGLIT:
        handleLiteral(STRINGLIT, parTree);
        break;
    }

    // visit children
    for (i = 0; i < parTree->nkids; i++)
    { // NOTE: excluding children visits on Method Calls here may not work, long-term...
        if (parTree->kids[i] != NULL)
            if ((parTree->kids[i]->prodrule != ClassBody) && (parTree->kids[i]->prodrule != ReturnStmt))
            {
                // printf("hit\n");
                populate_symboltables(parTree->kids[i]);
            }
    }
}

void handleDeclaredTokens(Tree *parTree)
{
    typeptr *newType = malloc(sizeof(typeptr));
    if (parTree->prodrule == ID)
    {
        // inserting identifier value in symbol table
        newType->primitive_Type = ID_Type;
        insert_sym(currentST, parTree->leaf->text, newType);
    }
    else
    {
        // not sure if to save types in ST
        newType->primitive_Type = find_primitive_type(parTree->leaf->category);
        insert_sym(currentST, parTree->leaf->text, newType);
    }
    if (debug)
        printf("Inserted: %s\n", parTree->leaf->text);
}

void handleArray(Tree *parTree)
{
    // set the type of the entry to array and determine/set the element type
    typeptr *newType;
    newType = malloc(sizeof(typeptr));
    newType->primitive_Type = ARRAY_Type;
    newType->u.ArrayType.elemType = malloc(sizeof(typeptr));
    newType->u.ArrayType.elemType->primitive_Type = find_primitive_type(parTree->kids[0]->leaf->category);
    // PRINTDEBUG("array element type: %s\n",get_type_string(newType->u.ArrayType.elemType->primitive_Type));

    if (parTree->kids[3]->prodrule > 1000)
    {
        if (parTree->kids[3]->prodrule == ArrVarDeclarator)
        {
            handleArrayVariableDecl(parTree->kids[3], newType);
        }
        else
        {
            handleDeclaredTokens(parTree->kids[3]);
        }
    }
    else
    {
        insert_sym(currentST, parTree->kids[3]->leaf->text, newType);
    }
}

void handleArrayVariableDecl(Tree *parTree, typeptr *setType)
{
    // if (debug) print_tree(parTree, 10);
    // loop over children to enter declaration into the symtab
    SymbolTableEntry arrayDestination = NULL;

    for (int i = 0; i < parTree->nkids; i++)
    {
        if (parTree->kids[i]->prodrule < 1000)
        {
            if (parTree->kids[i]->prodrule == ID)
            {
                arrayDestination = search_st(currentST, parTree->kids[i]->leaf->text);
                if (arrayDestination == NULL)
                {
                    insert_sym(currentST, parTree->kids[i]->leaf->text, setType);
                    PRINTDEBUG("Inserted: %s\n", parTree->kids[i]->leaf->text);
                }
            }
        }
        else if (parTree->kids[i]->prodrule > 1000)
        {
            if (parTree->kids[i]->prodrule == CreateArray)
            {
                setType->u.ArrayType.size = atoi(parTree->kids[i]->kids[3]->leaf->text);
            }
            else
            {
                handleInitArrLit(parTree->kids[i], setType);
            }
        }
    }
}

void handleInitArrLit(Tree *parTree, typeptr *setType)
{
    // print_tree(parTree, 20);
    int arrCount = 0;

    // type check to ensure assignment variable type matches the array literal type
    switch (parTree->prodrule)
    {
    case intArrLit:
        if (setType->u.ArrayType.elemType->primitive_Type != INTLIT_Type)
        {
            PRINTERR("Error: Array literal is type int while declaration is type %d\n", setType->u.ArrayType.elemType->primitive_Type);
            exit(ERROR_SEMANTIC);
        }
        break;
    case charArrLit:
        if (setType->u.ArrayType.elemType->primitive_Type != CHARLIT_Type)
        {
            PRINTERR("Error: Array literal is type char while declaration is type %d\n", setType->u.ArrayType.elemType->primitive_Type);
            exit(ERROR_SEMANTIC);
        }
        break;
    case boolArrLit:
        if (setType->u.ArrayType.elemType->primitive_Type != BOOLLIT_Type)
        {
            PRINTERR("Error: Array literal is type bool while declaration is type %d\n", setType->u.ArrayType.elemType->primitive_Type);
            exit(ERROR_SEMANTIC);
        }
        break;
    case doubleArrLit:
        if (setType->u.ArrayType.elemType->primitive_Type != DOUBLELIT_Type)
        {
            PRINTERR("Error: Array literal is type double while declaration is type %d\n", setType->u.ArrayType.elemType->primitive_Type);
            exit(ERROR_SEMANTIC);
        }
        break;
    case longArrLit:
        if (setType->u.ArrayType.elemType->primitive_Type != LONGLIT_Type)
        {
            PRINTERR("Error: Array literal is type long while declaration is type %d\n", setType->u.ArrayType.elemType->primitive_Type);
            exit(ERROR_SEMANTIC);
        }
        break;
    case floatArrLit:
        if (setType->u.ArrayType.elemType->primitive_Type != FLOATLIT_Type)
        {
            PRINTERR("Error: Array literal is type float while declaration is type %d\n", setType->u.ArrayType.elemType->primitive_Type);
            exit(ERROR_SEMANTIC);
        }
        break;
    case stringArrLit:
        if (setType->u.ArrayType.elemType->primitive_Type != STRINGLIT_Type)
        {
            PRINTERR("Error: Array literal is type String while declaration is type %d\n", setType->u.ArrayType.elemType->primitive_Type);
            exit(ERROR_SEMANTIC);
        }
        break;
    default:
        PRINTERR("Error: right half of assignment is not an array literal type.\n");
        exit(ERROR_SEMANTIC);
        break;
    }

    arrCount = countArrayMembers(parTree->kids[1]);
    setType->u.ArrayType.size = arrCount;
}

/**
 * @brief recursively counts all the members of an array literal in the parse tree
 *
 * @param parTree a node in the parse tree at which an array literal is rooted
 * @return int - the number of elements in the array literal
 */
int countArrayMembers(Tree *parTree)
{
    if (isArrOpt(parTree->prodrule))
    {
        return countArrayMembers(parTree->kids[0]) + 1;
    }
    else if (parTree->prodrule < 1000)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

// this function handles field constructors and method decl
void handleClassBody(Tree *parTree, typeptr *setType)
{
    for (int i = 0; i < parTree->nkids; i++)
    {
        if (parTree->kids[i]->prodrule == MethodDecl)
        {
            handleMethodDeclaration(parTree->kids[i], setType);
        }
        else if (parTree->kids[i]->prodrule == ClassBodyDeclarations)
        {
            handleClassBodyDeclarations(parTree->kids[i], setType);
        }
        else if (parTree->kids[i]->prodrule == FieldDeclaration)
        {
            handleClassBodyDeclaration(parTree->kids[i], setType);
        }
    }
}

void handleClassBodyDeclarations(Tree *parTree, typeptr *setType)
{
    for (int i = 0; i < parTree->nkids; i++)
    {
        if (parTree->kids[i]->prodrule == MethodDecl)
        {
            handleMethodDeclaration(parTree->kids[i], setType);
            // if (debug) printsymbols(currentST);
        }
        else if (parTree->kids[i]->prodrule == ClassBodyDeclarations)
        {
            handleClassBody(parTree->kids[0], setType);
            // if (debug) printsymbols(currentST);
        }
        else if (parTree->kids[i]->prodrule == FieldDeclaration)
        {
            handleClassBodyDeclaration(parTree->kids[i], setType);
            // if (debug) printsymbols(currentST);
        }
    }
}

void handleClassBodyDeclaration(Tree *parTree, typeptr *setType)
{
    // if tokens then
    typeptr *newType;
    if ((newType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error: failed to allocate typepointer.\n");
        exit(1);
    }

    if (parTree->kids[0]->prodrule > 1000)
    {
        if (parTree->kids[0]->prodrule == VarDeclarator)
        {
            SymbolTableEntry variableDestination = search_st(currentST, parTree->kids[0]->kids[0]->leaf->text);

            // TYPE CHECKING variable uniqueness
            if (variableDestination == NULL)
            {
                PRINTERR("SYMBOL ERROR on line %d: Variable '%s' is not declared \n", parTree->kids[0]->kids[0]->leaf->lineno, parTree->kids[0]->kids[0]->leaf->text);
                exit(ERROR_SEMANTIC);
            }
            else
            {
                // inserting the variable leaf to the symbol table
                insert_sym(currentST, parTree->kids[0]->kids[0]->leaf->text, variableDestination->type);
            }
        }
    }
    else
    {
        if (parTree->kids[1]->prodrule == VarDeclarator)
        {
            SymbolTableEntry variableDestination = search_st(currentST, parTree->kids[1]->kids[0]->leaf->text);

            // TYPE CHECKING variable uniqueness
            if (variableDestination != NULL)
            {
                PRINTERR("SYMBOL ERROR on line %d: Variable '%s' is already declared before\n", parTree->kids[1]->kids[0]->leaf->lineno, parTree->kids[1]->kids[0]->leaf->text);
                exit(ERROR_SEMANTIC);
            }
            else
            {
                // inserting the variable leaf to the symbol table
                newType->primitive_Type = find_primitive_type(parTree->kids[0]->leaf->category);
                insert_sym(currentST, parTree->kids[1]->kids[0]->leaf->text, newType);
            }
        }
        else
        {
            // but don't we also need to store their base types for type checking literals ???
            newType->primitive_Type = find_primitive_type(parTree->kids[0]->leaf->category);
            insert_sym(currentST, parTree->kids[1]->leaf->text, newType);
        }
    }
}

void handleMethodDeclaration(Tree *parTree, typeptr *setType)
{
    for (int i = 0; i < parTree->nkids; i++)
    {
        if (parTree->kids[i]->prodrule == MethodHeader)
        {
            handleMethodHeader(parTree->kids[i], setType);
        }
        else if (parTree->kids[i]->prodrule == Block)
        {
            handleBlock(parTree, i);
        }

        // populating method scope into symbol table
        for (int j = 0; j < parTree->kids[i]->nkids; j++)
        {
            populate_symboltables(parTree->kids[i]->kids[j]);
        }
    }
}

void handleMethodHeader(Tree *parTree, typeptr *setType)
{
    // setType->u.MethodType.returnType = malloc(sizeof(typeptr));
    typeptr *retType = malloc(sizeof(typeptr));
    if (retType == NULL)
    {
        PRINTERR("Error allocating memory for Method ReturnType.\n");
    }

    // get the return type for the method
    retType->primitive_Type = find_primitive_type(parTree->kids[2]->leaf->category);
    for (int j = 0; j < parTree->nkids; j++)
    {
        if (parTree->kids[j]->prodrule > 1000)
        {
            if (parTree->kids[j]->prodrule == MethodDeclarator)
            {
                handleMethodDeclarator(parTree->kids[j], retType);
                // print_tree(parTree, 10);
                // set name of method scope
                char *methodName = parTree->kids[3]->kids[0]->leaf->text;
                char *scopeName = malloc(8 + strlen(methodName));
                strncpy(scopeName, "method ", 8);
                strncat(scopeName, methodName, strlen(methodName));
                currentST->name = scopeName;
            }
            else if (parTree->kids[j]->prodrule == Block)
            {
                handleBlock(parTree, j);
            }
        }
    }
}

/**
 * @brief handles insertion of method symbols into the current symtab
 *
 * @param parTree the parse tree node at which the method declarator is rooted
 * @param retType the return typs of the method
 */
void handleMethodDeclarator(Tree *parTree, typeptr *retType)
{
    typeptr *newType;
    if ((newType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error: failed to allocate typepointer.\n");
        exit(1);
    }

    // set the function return type
    newType->u.MethodType.returnType = retType;

    if (parTree->kids[0]->prodrule == ID)
    {
        char *methodName = parTree->kids[0]->leaf->text;
        if (strcmp(methodName, "main") == 0)
        {
            // printf("check MAIN\n");
            newType->primitive_Type = MAIN_Type;

            SymbolTableEntry otherMain = search_st(currentST, "main");
            // checking if main has been declared before
            if (otherMain)
            {
                PRINTERR("SYMBOL ERROR in %s on line %d: main method already declared.\n", yyfilename, parTree->kids[0]->leaf->lineno);
                exit(ERROR_SEMANTIC);
            }
            else
            {
                insert_sym(currentST, methodName, newType);
            }
        }
        else
        {
            /*PROBLEM: variables are getting this type */
            newType->primitive_Type = METHOD_Type;
            if ((search_st(currentST, methodName)))
            {
                PRINTERR("SYMBOL ERROR on line %d: method %s declared more then once.\n", parTree->kids[0]->leaf->lineno, methodName);
                exit(ERROR_SEMANTIC);
            }
            else
            {
                insert_sym(currentST, methodName, newType);
            }
        }
    }
    // allocate new symtab for method subscope
    newType->u.MethodType.scope = new_st(setMethodSize, currentST);
    // change scopes
    currentST = newType->u.MethodType.scope;

    // printf("Method Declarator ptree:\n");
    // print_tree(parTree, 10);

    // enter the method parameter types into the method scope symtab and link them in the method typeptr paramList
    newType->u.MethodType.numParams = 0;
    if (parTree->kids[2]->prodrule == FormalParm)
    { // if there is only one parameter, enter it in the method subscope symtab
        typeptr *paramType = handleFormalParam(parTree->kids[2], newType);
        // link the parameters to the method typeptr
        newType->u.MethodType.numParams = 1;
        newType->u.MethodType.parameterList = malloc(sizeof(typeptr *));
        // newType->u.MethodType.parameterList[0] = malloc(sizeof(typeptr));
        newType->u.MethodType.parameterList[0] = paramType;
    }
    else if (parTree->kids[2]->prodrule == FormalParmList)
    { // if there is a list of parameters
        for (int i = 0; i < parTree->kids[2]->nkids; i++)
        { // loop through the parameters
            if (parTree->kids[2]->kids[i]->prodrule == FormalParm)
            { // and add all the parameters to the  method subscope symtab
                typeptr *paramType = handleFormalParam(parTree->kids[2]->kids[i], newType);
                newType->u.MethodType.numParams++; // increment the paramter field counter
                if (paramType)
                {
                    newType->u.MethodType.parameterList = realloc(newType->u.MethodType.parameterList, sizeof(typeptr) * (newType->u.MethodType.numParams + 1));
                    // newType->u.MethodType.parameterList[newType->u.MethodType.numParams] = paramType;
                    newType->u.MethodType.parameterList[newType->u.MethodType.numParams - 1] = paramType;
                }
                // verify successful insertion of parameter in method scope
            }
        }
    }
}

void handleMethodCall(Tree *parTree)
{
    // print_tree(parTree,10);
    if (parTree->kids[2])
    { // if there are parameters, check that they are in the symtab visible from the current scope
        switch (parTree->kids[2]->prodrule)
        {
        case MethodCall: // handle nested method calls recursively
            handleMethodCall(parTree->kids[2]);
            break;
        case ArgList: // handle multiple arguments with another function
            handleMethodParams(parTree->kids[2]);
            break;
        case ID: // check whether identifiers are already in a symtab visible from the current scope
            if (search_st(currentST, parTree->kids[2]->leaf->text) == NULL)
            { // if symbol is not found, it is undefined so throw error
                PRINTERR("SYMBOL ERROR in %s on line %d: symbol '%s' is undefined.\n", yyfilename, parTree->kids[2]->leaf->lineno, parTree->kids[2]->leaf->text);
                exit(ERROR_SEMANTIC);
            }
            break;
        // case AddExpr:
        //     handleArithmeticExpr(Tree * parTree);
        //     break;
        case INTLIT:
        case LONGLIT:
        case FLOATLIT:
        case DOUBLELIT:
        case STRINGLIT:
        case CHARLIT:
            break; // do nothing for literals
        default:
            if (parTree->kids[2]->prodrule > 1000)
            {
                handleArithmeticExpr(parTree->kids[2]);
            }
            else
            {
                PRINTERR("unknown parameter node %d\n", parTree->kids[2]->prodrule);
                exit(ERROR_TODO);
            }
        }
    } // if no params, return
    return;
}

void handleArithmeticExpr(Tree *parTree)
{
    for (int i = 0; i < parTree->nkids; i++)
    {
        if (parTree->kids[i]->prodrule > 1000)
        { // checking if the leaves contain non token values
            handleArithmeticExpr(parTree->kids[i]);
        }
        else
        {
            if (parTree->kids[i]->prodrule > 250)
            { // Avoiding operands and just reading tokens
                if (parTree->kids[i]->prodrule == ID)
                { // if the node is an identifier
                    if (search_st(currentST, parTree->kids[i]->leaf->text) == NULL)
                    {
                        PRINTERR("SYMBOL ERROR in %s on line %d: symbol '%s' is undefined.\n", yyfilename, parTree->kids[i]->leaf->lineno, parTree->kids[i]->leaf->text);
                        exit(ERROR_SEMANTIC);
                    }
                }
                else if (parTree->kids[i]->prodrule == MethodCall)
                {
                    handleMethodCall(parTree->kids[i]);
                }
            }
        }
    }
}

/**
 * @brief checks that symbols in passed method parameters are declared
 *
 * @param parTree
 */
void handleMethodParams(Tree *parTree)
{
    if (parTree)
    { // check that passed node is not null
        if (parTree->kids[0] && parTree->kids[2])
        { // check that children are not null
            if (parTree->kids[2]->prodrule == ArgList)
            {                                         // if there are additional parameters
                handleMethodParams(parTree->kids[2]); // visit them recursively
            }
            else
            { // if no more parameters, check the last one
                switch (parTree->kids[2]->prodrule)
                {
                case ID:
                    PRINTDEBUG("Current scope: %s\n", currentST->name);
                    if (search_st(currentST, parTree->kids[2]->leaf->text) == NULL)
                    { // look for the last symbol in the current scope; throw error and exit if not found
                        PRINTERR("SYMBOL ERROR in %s on line %d: symbol '%s' is undefined.\n", yyfilename, parTree->kids[2]->leaf->lineno, parTree->kids[2]->leaf->text);
                        exit(ERROR_SEMANTIC);
                    }
                    break;
                case MethodCall: // recursive handling of method calls
                    handleMethodCall(parTree->kids[2]);
                    break;
                case INTLIT:
                case LONGLIT:
                case FLOATLIT:
                case DOUBLELIT:
                case STRINGLIT:
                case CHARLIT:
                    break; // do nothing for literals
                default:
                    PRINTERR("ERROR: Unknown Prodrule %d in handleMethodParams().\n", parTree->kids[2]->prodrule);
                }
            }
            switch (parTree->kids[0]->prodrule)
            { // check the first parameter
            case ID:
                PRINTDEBUG("Current scope: %s\n", currentST->name);
                if (search_st(currentST, parTree->kids[0]->leaf->text) == NULL)
                { // look for the first symbol in the current scope; throw error and exit if not found
                    PRINTERR("SYMBOL ERROR in %s on line %d: symbol '%s' is undefined.\n", yyfilename, parTree->kids[0]->leaf->lineno, parTree->kids[0]->leaf->text);
                    exit(ERROR_SEMANTIC);
                }
                break;
            case MethodCall: // recursive handling of method calls
                handleMethodCall(parTree->kids[0]);
                break;
            case INTLIT:
            case LONGLIT:
            case FLOATLIT:
            case DOUBLELIT:
            case STRINGLIT:
            case CHARLIT:
                break; // do nothing for literals
            default:
                PRINTERR("ERROR: Unknown Prodrule %d in handleMethodParams().\n", parTree->kids[0]->prodrule);
            }
        }
    }
}
// {
//     DEBUG(print_tree(parTree, 10));
//     if (func->type->u.MethodType.numParams > 0)
//     { // if the function is supposed to take parameters
//         if (parTree == NULL)
//         { // and if there are no parameters passed, throw an error
//             PRINTERR("PARAMETER ERROR in %s on line %d: method '%s' takes %d arguments but none were provided.\n", yyfilename, lineno, func->s, func->type->u.MethodType.numParams);
//             exit(ERROR_SEMANTIC);
//         }
//         else if (parTree->prodrule == ArgList)
//         { // for multiple passed args, recursively visit all of them and typecheck along the way
//             int passedParamType = getComplexParameterType(parTree->kids[0]);
//             PRINTDEBUG("param %d type: %s\t expected: %s\n", paramIndex + 1, get_type_string(passedParamType), get_type_string(func->type->u.MethodType.parameterList[paramIndex]->primitive_Type));
//             if (passedParamType != func->type->u.MethodType.parameterList[paramIndex]->primitive_Type)
//             { // perform typecheck by comparing the type of the passed parameter to the expected type from the method STE
//                 PRINTERR("TYPE ERROR in %s on line %d: method '%s' expected parameter %d to have type %s but was passed type %s instead.\n", yyfilename, lineno, func->s, paramIndex + 1, get_type_string(func->type->u.MethodType.parameterList[paramIndex]->primitive_Type), get_type_string(passedParamType));
//                 exit(ERROR_SEMANTIC);
//             }
//             checkMethodParams(parTree->kids[2], func, lineno, paramIndex + 1); // recursive call on self when multiple arguments passed in function call
//         }
//         else
//         { // for single arguments or the end of a parameter list
//             int passedParamType = getComplexParameterType(parTree);
//             PRINTDEBUG("param %d type: %s\t expected: %s\n", paramIndex + 1, get_type_string(passedParamType), get_type_string(func->type->u.MethodType.parameterList[paramIndex]->primitive_Type));
//             if (passedParamType != func->type->u.MethodType.parameterList[paramIndex]->primitive_Type)
//             { // perform typecheck
//                 PRINTERR("TYPE ERROR in %s on line %d: method '%s' expected parameter %d to have type %s but was passed type %s instead.\n", yyfilename, lineno, func->s, paramIndex + 1, get_type_string(func->type->u.MethodType.parameterList[paramIndex]->primitive_Type), get_type_string(passedParamType));
//                 exit(ERROR_SEMANTIC);
//             }
//             if (paramIndex + 1 < func->type->u.MethodType.numParams)
//             {
//                 PRINTERR("PARAMETER ERROR in %s on line %d: method '%s' takes %d parameters but was passed only %d.\n", yyfilename, lineno, func->s, func->type->u.MethodType.numParams, paramIndex + 1);
//                 exit(ERROR_SEMANTIC);
//             }
//         }
//     }
//     else
//     { // if the function takes no parameters
//         if (parTree != NULL)
//         { // and if a parameter was provided, throw an error and exit
//             PRINTERR("PARAMETER ERROR in %s on line %d: method '%s' takes %d arguments but more were provided.\n", yyfilename, lineno, func->s, func->type->u.MethodType.numParams);
//             exit(ERROR_SYNTAX);
//         }
//     }
// }

/**
 * @brief handler function for inserting formal parameters into the symtab of the corresponding method
 *
 * @param parTree node in the parse tree about which the formal parameter is rooted
 */
typeptr *handleFormalParam(Tree *parTree, typeptr *setType)
{
    // allocate a new type pointer for the parameter
    typeptr *newType;
    if ((newType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for newType\n");
        exit(ERROR_MEMORY);
    }

    // get the parameter type
    newType->primitive_Type = find_primitive_type(parTree->kids[0]->prodrule);
    // printf("param prod rule: %d\n",newType->primitive_Type);

    // enter the new parameter into the method symtab
    int paramIDIndex = parTree->nkids - 1;
    char *paramName = parTree->kids[paramIDIndex]->leaf->text;
    insert_sym(setType->u.MethodType.scope, paramName, newType);
    // verify the symbol entry
    SymbolTableEntry param = lookup_st(setType->u.MethodType.scope, paramName);
    if (param)
    { // if not null, return the parameter type
        return newType;
    }
    else
    { // if symtab lookup returns NULL, memory error
        PRINTERR("Error inserting method parameter %s.\n", paramName);
        exit(ERROR_MEMORY);
    }
    return NULL;
}

void handleBlock(Tree *parTree, int i)
{
    for (int k = 0; k < parTree->kids[i]->nkids; k++)
    {
        if (parTree->kids[i]->kids[k]->prodrule == BlockStmt)
        {
            handleBlockStatement(parTree->kids[i]->kids[k]);
        }
        else if (parTree->kids[i]->kids[k]->prodrule == LocalVarDeclStmt)
        {
            handleLocalVariableDeclStatement(parTree->kids[i]->kids[k]);
        }
        else if (parTree->kids[i]->kids[k]->prodrule == ExprStmt)
        {
            handleExpressionStatement(parTree->kids[i]->kids[k]);
        }
        else if (parTree->kids[i]->kids[k]->prodrule == ForStmt)
        {
            if (parTree->kids[i]->kids[k]->kids[2]->prodrule == LocalVarDecl)
            {
                handleLocalVariableDecl(parTree->kids[i]->kids[k]->kids[2]);
            }
        }
    }
    // change scope after the end of method decleration
    currentST = currentST->parent;
}

void handleExpressionStatement(Tree *parTree)
{
    typeptr *newType;
    SymbolTableEntry assignmentDestination = NULL;
    if ((newType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error: failed to allocate typepointer.\n");
        exit(ERROR_SEMANTIC);
    }
    // DEBUG(print_tree(parTree, 10));

    if (parTree->kids[0]->prodrule == Assignment)
    {
        char *symbol = parTree->kids[0]->kids[0]->leaf->text;
        assignmentDestination = search_st(currentST, symbol);
        // print_tree(parTree,10);
        // printf("Search passed: %s\n", symbol);
        if (assignmentDestination == NULL)
        {
            PRINTERR("TYPE ERROR on line %d: Variable '%s' must be declared.\n", parTree->kids[0]->kids[0]->leaf->lineno, symbol);
            exit(ERROR_SEMANTIC);
        }
        // implement checking right half of assignment
        if (assignmentDestination->type->primitive_Type == ARRAY_Type)
            handleArrayVariableDecl(parTree->kids[0], assignmentDestination->type);
    }
    else if (parTree->kids[0]->prodrule == MethodCall)
    {
        PRINTDEBUG("handling method call\n");
        handleMethodCall(parTree->kids[0]);
    }
}

void handleBlockStatement(Tree *parTree)
{
    for (int i = 0; i < parTree->nkids; i++)
    {
        switch (parTree->kids[i]->prodrule)
        {
        case BlockStmt:
            handleBlockStatement(parTree->kids[0]);
            break;
        case LocalVarDeclStmt:
            handleLocalVariableDeclStatement(parTree->kids[i]);
            break;
        case ExprStmt:
            handleExpressionStatement(parTree->kids[i]);
            break;
        case ForStmt:
            if (parTree->kids[i]->kids[2]->prodrule == LocalVarDecl)
            {
                handleLocalVariableDecl(parTree->kids[i]->kids[2]);
            }
            break;
        default:
            // PRINTDEBUG("Statement type %d passed over\n",parTree->kids[i]->prodrule); // for debug
            break;
        }
    }
}

void handleLocalVariableDeclStatement(Tree *parTree)
{
    for (int i = 0; i < parTree->nkids; i++)
    {
        if (parTree->kids[i]->prodrule > 1000)
        {
            if (parTree->kids[i]->prodrule == LocalVarDecl)
            {
                handleLocalVariableDecl(parTree->kids[i]);
            } // end of localVarDeclLoop
        }
    }
}

void handleLocalVariableDecl(Tree *parTree)
{
    typeptr *newType;
    SymbolTableEntry newVariable = NULL;
    if ((newType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error: failed to allocate typepointer.\n");
        exit(ERROR_MEMORY);
    }

    if (parTree->kids[1]->prodrule == '[')
    { // if it is a arrayDecleration
        handleArray(parTree);
    }
    else if (parTree->kids[1]->prodrule == VarDeclarator)
    {
        newVariable = search_st(currentST, parTree->kids[1]->kids[0]->leaf->text);
        // TYPE CHECKING variable uniqueness
        if (newVariable != NULL)
        {
            PRINTERR("TYPE ERROR on line %d: Variable '%s' is already declared before\n", parTree->kids[1]->kids[0]->leaf->lineno, parTree->kids[1]->kids[0]->leaf->text);
            exit(ERROR_SEMANTIC);
        }
        else
        {
            newType->primitive_Type = find_primitive_type(parTree->kids[0]->leaf->category);
            insert_sym(currentST, parTree->kids[1]->kids[0]->leaf->text, newType);
        }
    }
    else
    {
        newVariable = search_st(currentST, parTree->kids[1]->leaf->text);
        if (newVariable != NULL)
        {
            PRINTERR("TYPE ERROR on line %d: Variable '%s' is already declared before\n", parTree->kids[1]->leaf->lineno, parTree->kids[1]->leaf->text);
            exit(ERROR_SEMANTIC);
        }
        else
        {
            newType->primitive_Type = find_primitive_type(parTree->kids[0]->leaf->category);
            insert_sym(currentST, parTree->kids[1]->leaf->text, newType);
        }
    }
}

void handleIdentifier(Tree *parTree)
{
    // printf("identifer: %s\n",parTree->leaf->text);
    if (lookup_st(currentST, parTree->leaf->text) == NULL)
    {
        semantic_error(parTree->leaf->text, parTree);
        exit(ERROR_SEMANTIC);
    }
}

void handleLiteral(int LIT, Tree *parTree)
{
    typeptr *setType = NULL;
    char *litName;

    setType = malloc(sizeof(struct typeptr));
    if (setType == NULL)
    {
        PRINTERR("typeptr out of memory\n");
        exit(-1);
    }
    litName = parTree->leaf->text;

    switch (LIT)
    {
    case INTLIT:
        setType->primitive_Type = INTLIT_Type;
        break;
    case FLOATLIT:
        setType->primitive_Type = FLOATLIT_Type;
        break;
    case DOUBLELIT:
        setType->primitive_Type = DOUBLELIT_Type;
        break;
    case LONGLIT:
        setType->primitive_Type = LONGLIT_Type;
        break;
    case CHARLIT:
        setType->primitive_Type = CHARLIT_Type;
        break;
    case STRINGLIT:
        setType->primitive_Type = STRINGLIT_Type;
        break;
    case BOOLLIT:
        setType->primitive_Type = BOOLLIT_Type;
        break;
    default:
        PRINTERR("error occured at handleLiteral function\n");
        exit(ERROR_SYNTAX);
    }

    // attempt to insert the literal into the symbol table
    if (currentST != NULL)
    {
        // if (lookup_st(currentST, getName) == NULL)
        // {
        //     insert_sym(currentST, getName, setType);
        // }
    }
    else // if current scope is null, attempt to insert into global scope
    {
        insert_sym(globalST, litName, setType);
    }
}

/**
 * @brief handles optional package specification by creating a new package scope and adding its associated symbol to the global symbol table (also sets the current scope to the package scope)
 *
 * @param parTree the parse tree node at which the package specification is rooted
 */
void handlePackageSpecification(Tree *parTree)
{
    // create new type pointer
    // typeptr *newType = malloc(sizeof(struct typeptr));
    // MEMCHECK(newType);
    NEWMEM(newType, typeptr);
    newType->primitive_Type = PACKAGE_Type;

    // create symtab for new package scope
    SymbolTable packageScope = new_st(setPackageSize, globalST);

    // associate the new scope with the typeptr union struct
    newType->u.PackageType.scope = packageScope;

    // get the scope name from the package specification in the parse tree
    char *packName = parTree->kids[1]->leaf->text;
    // naming the package scope symbol table
    int scopeNameSize = 9 + strlen(packName);
    char *scopeName = malloc(scopeNameSize);
    strncpy(scopeName, "package ", 9);
    strncat(scopeName, packName, scopeNameSize - 9);
    packageScope->name = scopeName; // set the scope name

    PRINTDEBUG("  Using: %s\n", scopeName);

    // insert the package scope into the globalST
    insert_sym(globalST, packName, newType);

    currentST = packageScope;
    return;
}

/**
 * @brief handles multiple optional package imports by iterating them with the handler function
 *
 * @param parTree the parse tree node at which a package import is rooted
 */
void handlePackageImports(Tree *parTree)
{
    for (int i = 0; i < parTree->nkids; i++)
    {
        // recursively handle multiple package imports
        if (parTree->kids[i]->prodrule == importDeclaration)
            handlePackageImports(parTree->kids[i]); // recursive call!
        else
            handlePackageImport(parTree->kids[i]);
    }
}

/**
 * @brief handles optional package imports by inserting their symbols and subscopes in the global symbol table
 *
 * @param parTree
 */
void handlePackageImport(Tree *parTree)
{
    // print_tree(parTree,10); // for debug*************************************
    char *packName;
    switch (parTree->prodrule)
    {
    case singleTypeImportDeclaration:
        packName = getPackageName(parTree->kids[1]);
        PRINTDEBUG("  package: %s (single)\n", packName);
        handlePackageImportInsertion(parTree->kids[1]);
        break;
    case typeImportOnDemandDeclaration: // likely need to fix this in future
        if (parTree->kids[1]->prodrule == packageName)
        {
            packName = getPackageName(parTree->kids[1]);
            handlePackageImportInsertion(parTree->kids[1]);
        }
        else
        {
            packName = getPackageName(parTree);
            handlePackageImportInsertion(parTree);
        }
        PRINTDEBUG("  package: %s.* (on demand)\n", packName);
        // TODO actually insert using handlePackageImportInsertion()
        break;
    default:
        break;
    }
}

/**
 * @brief Inserts a stymbol table entry for the package imports into the globalST
 *
 * @param parTree the parse tree node at which the package import is rooted
 */
void handlePackageImportInsertion(Tree *parTree)
{
    // create new type pointer
    // typeptr *newType = malloc(sizeof(struct typeptr));
    // MEMCHECK(newType);
    NEWMEM(newType, typeptr);
    newType->primitive_Type = IMPORT_Type;

    // get the scope name from the package specification in the parse tree
    char *packName = getPackageName(parTree);

    // insert the package scope into the globalST
    insert_sym(globalST, packName, newType);
    return;
}

/**
 * @brief get the package name by recursively handling the package hierarchy structure
 *
 * @param parTree the parse tree node at which the package name is rooted
 * @return char* the full name of the package
 */
char *getPackageName(Tree *parTree)
{
    // if package import has no . in it
    if (parTree->kids[1]->prodrule == ID)
        return parTree->kids[1]->leaf->text;
    char *packName; // the combined package name
    char *first;    // the package first name (before the .)
    char *last;     // the package last name
    if (parTree->kids[0]->prodrule == packageName)
        first = getPackageName(parTree->kids[0]); // recursively build multi-level package names
    else
        first = parTree->kids[0]->leaf->text;
    last = parTree->kids[2]->leaf->text;
    packName = calloc(strlen(first) + 2 + strlen(last), sizeof(char));
    strncpy(packName, first, strlen(first));
    strncat(packName, ".", strlen(first) + 1);
    strncat(packName, last, strlen(first) + 1 + strlen(last));
    return packName;
}

// void insertBuiltins(){
//     // list of built-ins as an array of strings:
//     char *builtins_java[] = {};
//     // iterate over list and insert all built-ins
//     for (int i = 0; i < sizeof(builtins_java) / sizeof(char *); i++)
//     {
//         typeptr *newType;
//         if ((newType = malloc(sizeof(typeptr))) == NULL)
//         {
//             PRINTERR("Error allocating memory for newType in insertBuiltins\n");
//             exit(ERROR_MEMORY);
//         }
//     }
// }

/**
 * @brief creates a new class scope and handles insertion
 * of symbols into the new corresponding symbol table
 *
 * @param parTree pointer to the parse tree node at which the class declaration is rooted
 */
void handleClassDeclaration(Tree *parTree)
{
    SymbolTable classScope = NULL;
    // typeptr *setType = malloc(sizeof(typeptr));
    // MEMCHECK(setType);
    NEWMEM(setType, typeptr);
    // allocate memory for the setType pointer

    // creating a symbol table for classScope
    classScope = new_st(setClassSize, currentST);
    // get the name of the class from the parse tree
    char *className = parTree->kids[2]->leaf->text;
    // naming the class scope symbol table
    int scopeNameSize = 7 + strlen(className);
    char *scopeName = malloc(scopeNameSize);
    strncpy(scopeName, "class ", 7);
    strncat(scopeName, className, scopeNameSize - 7);
    classScope->name = scopeName; // set the scope name

    // stores class types in class struct
    setType->u.ClassType.scope = classScope;
    setType->primitive_Type = CLASS_Type;

    if (search_st(currentST, className))
    { // throw error and exit if class name already used
        PRINTERR("DECLARATION ERROR in %s on line %d: class %s already defined.\n", yyfilename, parTree->kids[2]->leaf->lineno, className);
        exit(ERROR_SEMANTIC);
    }

    // inserting new class scope in current scope symbol table and confirm its entry.
    insert_sym(currentST, className, setType);
    SymbolTableEntry entryConfirmation = search_st(currentST, className);
    entryConfirmation->type->u.MethodType.scope = classScope;

    // updating current scope
    currentST = classScope;

    handleClassBody(parTree->kids[3], setType);
}

void initGlobalST(Tree *parTree)
{
    SymbolTable globalScope = NULL;

    // creating a symbol table for classScope
    globalScope = new_st(setGlobalSize, NULL);

    // naming the global scope symbol table as just global
    globalScope->name = "GLOBAL scope";

    // setting global scope
    globalST = globalScope;

    // udating the current scope
    currentST = globalScope;

    // if the root node is a compilation unit, handle preprocessing directives first
    if (parTree->prodrule == compilationUnit)
    {
        PRINTDEBUG("Num Compilation Units: %d\n", parTree->nkids);
        // Tree *child = parTree->kids[0]; // loop over the children of compilation unit nodes in root
        for (int i = 0; i < parTree->nkids; i++)
        {
            switch (parTree->kids[i]->prodrule)
            {
            case packageDeclaration:
                PRINTDEBUG(" Package Specification Handling: \n");
                handlePackageSpecification(parTree->kids[i]);
                break;
            case importDeclaration:
                PRINTDEBUG(" Multiple Package Import Handling: \n");
                handlePackageImports(parTree->kids[i]);
                break;
            case singleTypeImportDeclaration:
            case typeImportOnDemandDeclaration:
                PRINTDEBUG(" Single Package Import Handling: \n");
                handlePackageImport(parTree->kids[i]);
                break;
            case multiImportDeclaration:
                PRINTERR("ERROR: unimplemented edgecase reached: multiImportDeclaration handling in initGlobalST()\n");
                exit(-1);
            default:
                // print_tree(parTree->kids[i], 10); // for debug *********************
                // populate the symbol table
                populate_symboltables(parTree->kids[i]);
                break;
            }
        }
    }
    else
    {
        // print_tree(parTree, 10); // for debug *********************
        // populate the symbol table
        populate_symboltables(parTree);
    }
}

/**
 * @brief continues to add to the global symtab
 *
 * @param parTree the root node of the parse tree
 */
void contGlobalST(Tree *parTree)
{
    // updating the current scope
    currentST = globalST;

    // if the root node is a compilation unit, handle preprocessing directives first
    if (parTree->prodrule == compilationUnit)
    {
        PRINTDEBUG("Num Compilation Units: %d\n", parTree->nkids);
        // Tree *child = parTree->kids[0]; // loop over the children of compilation unit nodes in root
        for (int i = 0; i < parTree->nkids; i++)
        {
            switch (parTree->kids[i]->prodrule)
            {
            case packageDeclaration:
                PRINTDEBUG(" Package Specification Handling: \n");
                handlePackageSpecification(parTree->kids[i]);
                break;
            case importDeclaration:
                PRINTDEBUG(" Package Import Handling: \n");
                handlePackageImports(parTree->kids[i]);
                break;
            case singleTypeImportDeclaration:
            case multiImportDeclaration:
            default:
                break;
            }
        }
    }
    // populate the symbol table
    populate_symboltables(parTree);
}

void semantic_error(char *s, Tree *n)
{
    while (n)
        n = n->kids[0];
    if (n)
    {
        PRINTERR("%s:%d: ", n->leaf->filename, n->leaf->lineno);
    }
    PRINTERR("%s", s);
    if (n && n->id == Identifier)
        PRINTERR(" %s", n->leaf->text);
    PRINTERR("\n");
    // errors++;
}

int find_primitive_type(int token)
{
    switch (token)
    {
    case (VOID):
        return VOID_Type;
        break;
    case (INT):
    case (INTLIT):
        return INTLIT_Type;
        break;
    case (BOOL):
    case (BOOLLIT):
        return BOOLLIT_Type;
        break;
    case (DOUBLE):
    case (DOUBLELIT):
        return DOUBLELIT_Type;
        break;
    case (LONG):
    case (LONGLIT):
        return LONGLIT_Type;
        break;
    case (FLOAT):
    case (FLOATLIT):
        return FLOATLIT_Type;
        break;
    case (CHAR):
    case (CHARLIT):
        return CHARLIT_Type;
        break;
    case (STRING):
    case (STRINGLIT):
        return STRINGLIT_Type;
        break;
    case (CLASS):
        return CLASS_Type;
        break;
    case (ID):
        return ID_Type;
    case (MethodCall):
        return METHOD_Type;
        break;
    // case (ArrLiteral):
    //     return ARRAY_Type;
    //     break;
    // PACKAGE_Type,   // 8
    // CLASS_Type,     // 9
    // METHOD_Type,    // 10
    // MAIN_Type,      // 11
    // VAR_Type,       // 13
    default:
        PRINTERR("TYPE ERROR in %s when attepting to find primitive type for (%d).\n", yyfilename, token);
        exit(ERROR_SYNTAX);
    }
}

/**
 * @brief Get the human readable name of each type enum as a string.
 *
 * @param t the type enum
 * @return char* the name of the type
 */
char *get_type_string(enum types t)
{
    switch (t)
    {
    case INTLIT_Type:
        return "int";
        break;
    case BOOLLIT_Type:
        return "bool";
        break;
    case CHARLIT_Type:
        return "char";
        break;
    case LONGLIT_Type:
        return "long";
        break;
    case FLOATLIT_Type:
        return "float";
        break;
    case DOUBLELIT_Type:
        return "double";
        break;
    case STRINGLIT_Type:
        return "String";
        break;
    case ARRAY_Type:
        return "array";
        break;
    case PACKAGE_Type:
        return "package";
        break;
    case IMPORT_Type:
        return "package import";
        break;
    case CLASS_Type:
        return "class";
        break;
    case METHOD_Type:
        return "method";
        break;
    case MAIN_Type:
        return "MAIN method";
        break;
    case VOID_Type:
        return "void";
        break;
    case VAR_Type:
        return "variable";
        break;
    case ID_Type:
        return "identifier";
        break;
    default:
        return "type unknown";
        break;
    }
}

/**
 * @brief function to delete a typepointer (unstable)
 *
 * @param type the typeptr to free
 */
void free_typepointer(typeptr *type)
{
    switch (type->primitive_Type)
    {
    case PACKAGE_Type:
        delete_st(type->u.PackageType.scope);
        free(type->u.PackageType.scope);
        break;
    case CLASS_Type:
        delete_st(type->u.ClassType.scope);
        free(type->u.ClassType.scope);
        break;
    case METHOD_Type:
    case MAIN_Type:
        delete_st(type->u.MethodType.scope);
        free(type->u.MethodType.scope);
        break;
    default:
        break;
    }
}