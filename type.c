/**
 * @file type.c
 * @author Saugat Sharma & Joseph Coston
 * @date 2022-04-06
 * @version HW6
 * @brief Part 2 of Stage 3 (Semantic Analysis) of compiler design: Type check for J0 program
 *
 * @todo 2. Array Type checks (I think its done but please make sure checking test1 commented lines)
 *       3. Type check for packages
 */

#include "type.h"
#include "j0.tab.h"
#include "errcodes.h"
#include "termargs.h"
#include <string.h>

extern int errors;
extern char *yyfilename;

/**
 * @brief this function parses through tree and type checks
 * @param tree struct parameter that points to syntax tree node
 * @returns void
 */
void typeCheck(Tree *parTree)
{
    int i;
    // DEBUGGER("Checking\n");
    if (parTree == NULL)
        return;
    /* pre-order activity */
    switch (parTree->prodrule)
    {
    case ClassDeclaration:
        changeClassScope(parTree);
        break;
    case ClassBody:
        checkClassBodyDecl(parTree);
        break;
    case MethodCall:
        checkMethodCall(parTree);
        break;
    case MethodDeclarator:
        changeMethodScope(parTree);
        // break;
    case FormalParm:
        break;
    }
    for (i = 0; i < parTree->nkids; i++)
    {
        /* visit children */
        typeCheck(parTree->kids[i]);
    }
}

/**
 * @brief perform semantic analysis on the code in the parse tree rooted at the passed tree node pointer
 * 
 * @param root pointer to the root node of the code to check
 */
void TypeCheck_InitGlobal(Tree *root)
{
    TypeCurrent = globalST;
    if (root->prodrule == compilationUnit)
    {
        for (int i = 0; i < root->nkids; i++)
        {
            switch (root->kids[i]->prodrule)
            {
            case packageDeclaration:
                changePackageScope(root->kids[i]);
                break;
            case ClassDeclaration:
                changeClassScope(root->kids[i]);
                typeCheck(root->kids[i]);
                break;
            // TODO: cover these cases -- look in builtins.c
            case importDeclaration:
            case singleTypeImportDeclaration:
            case multiImportDeclaration:
            case typeImportOnDemandDeclaration:
            // DEBUGGER("package import typecheck reached!\n"); moving this functionality to builtins.c
            default:
                // typeCheck(parTree->kids[i]);
                break;
            }
        }
    }
    else
    {
        TypeCurrent = globalST;
    }
    typeCheck(root);
}

/**
 * @brief changes current scope (TypeCurrent) to Package Symbol Table scope
 * @param tree struct parameter that points to syntax tree node
 * @returns void
 */
void changePackageScope(Tree *parTree)
{
    SymbolTableEntry FindPackageScope;
    // Using Package Idenfier looking up for package ST in global symbolTable
    FindPackageScope = lookup_st(globalST, parTree->kids[1]->leaf->text);
    // getting package scope symbolTable
    TypeCurrent = FindPackageScope->type->u.PackageType.scope;
}

/**
 * @brief CHANGES to classDeclereation scope
 * @param tree struct parameter that points to syntax tree node
 * @returns void
 */
void changeClassScope(Tree *parTree)
{
    // TypeCurrent = globalST->u.ClassType.scope;

    SymbolTableEntry classScope;

    // Using class Identifier to look up in symbol table in current scope
    classScope = search_st(TypeCurrent, parTree->kids[2]->leaf->text);
    if (classScope)
    { // if class scope found, enter it
        TypeCurrent = classScope->type->u.ClassType.scope;
    }
    else
    {
        PRINTERR("MEMORY ERROR in %s when trying to enter the %s scope.\n", yyfilename, parTree->kids[2]->leaf->text);
        exit(ERROR_MEMORY);
    }
    // TypeCurrent = FindClassScope->table->children[0];
}

/**
 * @brief CHANGES current scope to the method scope
 * @param tree struct parameter that points to syntax tree node
 * @returns void
 */
void changeMethodScope(Tree *parTree)
{
    SymbolTableEntry methodSTE;
    // Using method name to serach symbol table in current and parent scope
    methodSTE = search_st(TypeCurrent, parTree->kids[0]->leaf->text);
    if (methodSTE == NULL)
    {
        PRINTERR("SCOPE ERROR in %s on line number: %d Scope not found: Method %s need to define in current scope \n", yyfilename, parTree->kids[0]->leaf->lineno, parTree->kids[0]->leaf->text);
        exit(ERROR_SEMANTIC);
    }
    PRINTDEBUG("Entering scope %s\n", methodSTE->s);

    // TypeCurrent = FindMethodScope->table->children[0];
    TypeCurrent = methodSTE->type->u.MethodType.scope;
}

/**
 * @brief recursively loop through Block statments until LocalVarDeclStmt or Assignment is found
 * @param tree struct parameter that points to syntax tree node
 * @returns void
 */
void checkBlockTypes(Tree *parTree)
{
    int prodrule = 0;
    for (int i = 0; i < parTree->nkids; i++)
    {
        prodrule = parTree->kids[i]->prodrule;

        switch (prodrule)
        {
        case BlockStmt:
            // recursively calling this funciton to loop through block statements
            checkBlockTypes(parTree->kids[i]);
            break;
        case LocalVarDeclStmt:

            for (int j = 0; j < parTree->kids[i]->nkids; j++)
            {
                if (parTree->kids[i]->kids[j]->prodrule == InitializeArrayElement)
                {

                    checkInitlArrElemType(parTree->kids[i]->kids[j]);
                }
                else if (parTree->kids[i]->kids[j]->prodrule == LocalVarDecl)
                {
                    // Type checking local variable declariations
                    checkLocVarDecType(parTree->kids[i]->kids[0]);
                }
            }

            break;
        case Stmt:
            if (parTree->kids[i]->kids[0]->prodrule == Assignment)
            {
                checkAssignmentType(parTree->kids[i]->kids[0]);
            }
            break;

        case IfThenStmt:
        case IfThenElseStmt:
            // case IfThenElseStmtNoShortIf:
            checkCondStatTypes(parTree->kids[i]);
            break;

        case WhileStmt:
        case WhileStmtNoShortIf:
            checkCondStatTypes(parTree->kids[i]);
            break;
        case ForStmt:
            // case ForStmtNoShortIf:
            checkForStatementTypes(parTree->kids[i]);
            break;
        case SwitchStatement:
            checkSwitchStatementTypes(parTree->kids[i]);
            break;
        case ReturnStmt:
            checkReturnStatement(parTree->kids[i]);
            break;
        }
    }
}

void checkReturnStatement(Tree *parTree)
{
    typeptr *findType = NULL;
    SymbolTableEntry findReturnEntry = NULL, findReturnType = NULL;
    char *methodName = NULL;
    char *s = NULL;
    char *token;

    if ((findType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for setType in checkSwitchStatementTypes\n");
        exit(ERROR_MEMORY);
    }

    // findType->primitive_Type = TypeCurrent->u.MethodType.returnType;
    methodName = TypeCurrent->name;
    int size = sizeof(TypeCurrent->name);
    s = malloc(size);

    s = strtok(methodName, " ");
    while (s)
    {
        if (strcmp(s, "method") != 0)
        {
            token = s;
        }
        s = strtok(NULL, " ");
    }

    findReturnEntry = search_st(TypeCurrent, token);

    if (parTree->kids[1]->prodrule == ID)
    {
        findReturnType = search_st(TypeCurrent, parTree->kids[1]->leaf->text);

        if (findReturnType == NULL)
        {
            PRINTERR("TYPE ERROR in %s on line number: %d Variable Decleration error: Variable %s need to be declared first \n", yyfilename, parTree->kids[1]->leaf->lineno, parTree->kids[1]->leaf->text);
            exit(ERROR_SEMANTIC);
        }
        else
        {
            if (findReturnEntry->type->u.MethodType.returnType->primitive_Type != findReturnType->type->primitive_Type)
            {
                PRINTERR("TYPE ERROR in %s on line number: %d Return Type Error: Return Type  %s mismatch \n", yyfilename, parTree->kids[1]->leaf->lineno, parTree->kids[1]->leaf->text);
                exit(ERROR_SEMANTIC);
            }
        }
    }
    else
    {
        findType->primitive_Type = find_primitive_type(parTree->kids[1]->prodrule);

        if (findReturnEntry->type->u.MethodType.returnType->primitive_Type != findType->primitive_Type)
        {
            PRINTERR("TYPE ERROR in %s on line number: %d Return Type Error: Return Type  %s mismatch \n", yyfilename, parTree->kids[1]->leaf->lineno, parTree->kids[1]->leaf->text);
            exit(ERROR_SEMANTIC);
        }
        free(findType);
    }
    // if (findReturnEntry->type->u.MethodType.returnType->primitive_Type != )

    /*strcpy(s,findReturnEntry->table->name);*/
}

/**
 * @brief Type check swithch statement expression >>switch(expression)>>
 *        and type check case values with switch expression.
 * @param tree struct parameter that points to AST tree node
 * @returns void
 */
void checkSwitchStatementTypes(Tree *parTree)
{
    typeptr *findType = NULL;
    SymbolTableEntry findSwitchExpEntry = NULL;

    // allocate memory for the setType pointer
    if ((findType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for setType in checkSwitchStatementTypes\n");
        exit(ERROR_MEMORY);
    }

    // checking if switch expression is ID
    if (parTree->kids[2]->prodrule == ID)
    {
        findSwitchExpEntry = lookup_st(TypeCurrent, parTree->kids[2]->leaf->text);
        if (findSwitchExpEntry == NULL)
        {
            PRINTERR("TYPE ERROR in %s on line number: %d Variable Decleration error: Variable %s need to be declared first \n", yyfilename, parTree->kids[2]->leaf->lineno, parTree->kids[2]->leaf->text);
            exit(ERROR_SEMANTIC);
        }
        // updates FindTypes
        findType = findSwitchExpEntry->type;

        if ((findSwitchExpEntry->type->primitive_Type != INTLIT_Type))
        {
            PRINTERR("TYPE ERROR in %s on line number: %d. J0 doesn't support this feature.Variable %s need to be Integer Type \n", yyfilename, parTree->kids[2]->leaf->lineno, parTree->kids[2]->leaf->text);
            exit(ERROR_SEMANTIC);
        }
    }
    else
    {
        // checking switch type
        findType->primitive_Type = find_primitive_type(parTree->kids[2]->prodrule);

        if (findType->primitive_Type != INTLIT_Type)
        {
            PRINTERR("TYPE ERROR in %s on line number: %d. J0 doesn't support this feature.Variable %s need to be Integer Type \n", yyfilename, parTree->kids[2]->leaf->lineno, parTree->kids[2]->leaf->text);
            exit(ERROR_SEMANTIC);
        }
    }

    // if SwitchCase expression is detected call checkSwitchCase funciton
    if (parTree->kids[5]->prodrule == SwitchBody)
    {
        checkSwitchBody(parTree->kids[5], findType);
        free(findType);
    }
}

/**
 * @brief recursively calls the function until SwitchCaseBlock is found
 * @param tree struct parameter that points to AST tree node
 * @pararm base type of switch expression for typechecking
 * @returns void
 */
void checkSwitchBody(Tree *parTree, typeptr *switchExpType)
{
    for (int i = 0; i < parTree->nkids; i++)
    {
        if (parTree->kids[i]->prodrule == SwitchCases)
        {
            checkSwitchBody(parTree->kids[i], switchExpType);

            // checking if there is a case value declared
        }
        else if (parTree->kids[i]->prodrule == SwitchCaseBlock)
        {
            checkSwitchCaseBlock(parTree->kids[i], switchExpType);
        }
    }
}

/**
 * @brief Type checks switch case value with swicth expression
 * @param tree struct parameter that points to AST tree node
 * @pararm base type of switch expression for typechecking
 * @returns void
 */
void checkSwitchCaseBlock(Tree *parTree, typeptr *switchExpType)
{

    SymbolTableEntry findCaseType = NULL;

    typeptr *caseType = NULL;

    // allocate memory for the setType pointer
    if ((caseType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for FindLeftType in checkSwitchCaseBlock\n");
        exit(ERROR_MEMORY);
    }

    for (int j = 0; j < parTree->nkids; j++)
    {
        if (parTree->kids[j]->prodrule == SwitchCase)
        {
            if (parTree->kids[j]->kids[0]->prodrule == CASE)
            {
                if (parTree->kids[j]->kids[1] != NULL)
                {
                    if (parTree->kids[j]->kids[1]->prodrule == ID)
                    {
                        // if case Value is ID then
                        findCaseType = lookup_st(TypeCurrent, parTree->kids[j]->kids[1]->leaf->text);

                        if (findCaseType == NULL)
                        {
                            // right operand not found
                            PRINTERR("TYPE ERROR in %s on line number: %d Variable Decleration error: Variable %s need to be declared first \n", yyfilename, parTree->kids[j]->kids[1]->leaf->lineno, parTree->kids[j]->kids[1]->leaf->text);
                            exit(ERROR_SEMANTIC);
                        }
                        else
                        {
                            checkType(switchExpType, findCaseType->type, parTree->kids[j]->kids[1]);
                        }
                    }
                    else
                    {
                        // if case value is token then
                        caseType->primitive_Type = find_primitive_type(parTree->kids[j]->kids[1]->prodrule);
                        checkType(switchExpType, caseType, parTree->kids[j]->kids[1]);
                        free(caseType);
                    }
                }
            }
        }
        else if (parTree->kids[j]->prodrule == BlockStmt)
        {
            checkBlockTypes(parTree->kids[j]);
        }
    }
}

/**
 * @brief Type checks For loop statement initialization, condition, and increment
 * @param tree struct parameter that points to AST tree node
 * @returns void
 */
void checkForStatementTypes(Tree *parTree)
{
    for (int i = 0; i < parTree->nkids; i++)
    {
        // Type checking initialization
        if (parTree->kids[i]->prodrule == LocalVarDecl)
        {
            checkLocVarDecType(parTree->kids[i]);
        }
        else if (parTree->kids[i]->prodrule == Assignment)
        {
            checkAssignmentType(parTree->kids[i]);
        }
        else if (parTree->kids[i]->prodrule == RelExpr)
        {
            checkCondExpTypes(parTree->kids[i]);
        }
        else if (parTree->kids[i]->prodrule == Block)
        {
            checkBlockTypes(parTree->kids[i]);
        }
    }
}

/**
 * @brief checks conditional statement Types under if/while statements
 * @param tree struct parameter that points to AST tree node
 * @returns void
 */

void checkCondStatTypes(Tree *parTree)
{
    SymbolTableEntry getRightOperand = NULL;
    typeptr *rightOpType = NULL;
    typeptr *leftOpType = NULL;

    // allocate memory for the setType pointer
    if ((rightOpType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for FindLeftType in checkCondStatTypes\n");
        exit(ERROR_MEMORY);
    }

    // allocate memory for the setType pointer
    if ((leftOpType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for FindLeftType in checkCondStatTypes\n");
        exit(ERROR_MEMORY);
    }

    for (int i = 0; i < parTree->nkids; i++)
    {
        // checking expressions like relation expressions, equation expresssion
        if (parTree->kids[i]->prodrule > 1000)
        {
            // checking inside for and while loop

            if (parTree->kids[i]->prodrule == Block)
            {
                checkBlockTypes(parTree->kids[i]);
            }
            else if ((parTree->kids[i]->prodrule == IfThenElseStmt) || (parTree->kids[i]->prodrule == IfThenStmt))
            {
                checkCondStatTypes(parTree->kids[i]);
            }
            else
            {
                checkCondExpTypes(parTree->kids[i]);
            }
        }
        else
        {
            // if no relation expressions are found then checking if the value is BoolType
            if (parTree->kids[i]->prodrule == ID)
            {
                // checking right operand in symbol table
                getRightOperand = lookup_st(TypeCurrent, parTree->kids[i]->leaf->text);

                if (getRightOperand == NULL)
                {
                    // right operand not found
                    PRINTERR("TYPE ERROR in %s on line number: %d Variable Decleration error: Variable %s need to be declared first \n", yyfilename, parTree->kids[i]->leaf->lineno, parTree->kids[i]->leaf->text);
                    exit(ERROR_SEMANTIC);
                }

                if (getRightOperand->type->primitive_Type != BOOLLIT_Type)
                {
                    // right operand not found
                    PRINTERR("TYPE ERROR in %s on line number: %d Variable %s comparision not possible. Need to be Boolean Type\n", yyfilename, parTree->kids[i]->leaf->lineno, parTree->kids[i]->leaf->text);
                    exit(ERROR_SEMANTIC);
                }
            }
            else
            {
                // ignores non tokens
                if ((parTree->kids[i]->prodrule > 250) && (parTree->kids[i]->prodrule != IF) && (parTree->kids[i]->prodrule != WHILE) && (parTree->kids[i]->prodrule != ELSE))
                {
                    // if rightOpType is token type find compare its base type to BOOLLIT Type.
                    rightOpType->primitive_Type = find_primitive_type(parTree->kids[i]->prodrule);
                    if (rightOpType->primitive_Type != BOOLLIT_Type)
                    {
                        // right operand not found
                        PRINTERR("TYPE ERROR in %s on line number: %d Variable %s comparision not possible. Need to be Boolean Type\n", yyfilename, parTree->kids[i]->leaf->lineno, parTree->kids[i]->leaf->text);
                        exit(ERROR_SEMANTIC);
                    }
                }
            }
        }
    }
    free(leftOpType);
    free(rightOpType);
}

/**
 * @brief checks conditional expression  Types under if/while statements
 * @param tree struct parameter that points to AST tree node
 * @returns void
 */
void checkCondExpTypes(Tree *parTree)
{
    SymbolTableEntry getRightOperand = NULL, getLeftOperand = NULL;
    typeptr *rightOpType = NULL;
    typeptr *leftOpType = NULL;

    // allocate memory for the setType pointer
    if ((rightOpType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for FindLeftType in checkCondExpTypes\n");
        exit(ERROR_MEMORY);
    }

    // allocate memory for the setType pointer
    if ((leftOpType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for FindLeftType in checkCondExpTypes\n");
        exit(ERROR_MEMORY);
    }

    if (parTree->kids[0]->prodrule == Primary)
    {
        if (parTree->kids[0]->kids[1]->prodrule > 1000)
        {
            // recursively calling if expressions like relation expressions, equation expresssion is found
            checkCondExpTypes(parTree->kids[0]->kids[1]);
        }
    }
    else
    {
        // checking right operand in conditional statements
        if (parTree->kids[0]->prodrule == ID)
        {
            // checking right operand in symbol table
            getRightOperand = lookup_st(TypeCurrent, parTree->kids[0]->leaf->text);

            if (getRightOperand == NULL)
            {
                // right operand not found
                PRINTERR("TYPE ERROR in %s on line number: %d Variable Decleration error: Variable %s need to be declared first \n", yyfilename, parTree->kids[2]->leaf->lineno, parTree->kids[2]->leaf->text);
                exit(ERROR_SEMANTIC);
            }
            // checking the case when right operand is literals
        }
        else if (parTree->kids[0]->prodrule != ID)
        {
            if ((parTree->kids[0]->prodrule > 250) && (parTree->kids[0]->prodrule != IF) && (parTree->kids[0]->prodrule != WHILE) && (parTree->kids[0]->prodrule != ELSE))
            {
                rightOpType->primitive_Type = find_primitive_type(parTree->kids[0]->prodrule);
            }
        }

        // checking if left operand is present
        if (parTree->kids[2] != NULL)
        {
            if (parTree->kids[2]->prodrule == ID)
            {
                // checking left operand in conditional statements
                getLeftOperand = lookup_st(TypeCurrent, parTree->kids[2]->leaf->text);

                if (getLeftOperand == NULL)
                {
                    // left operand not found in ST
                    PRINTERR("TYPE ERROR in %s on line number: %d Variable Decleration error: Variable %s need to be declared first \n", yyfilename, parTree->kids[2]->leaf->lineno, parTree->kids[2]->leaf->text);
                    exit(ERROR_SEMANTIC);
                }
                // checking the case when left operand is literals
            }
            else if (parTree->kids[2]->prodrule != ID)
            {
                if ((parTree->kids[2]->prodrule > 250) && (parTree->kids[2]->prodrule != IF) && (parTree->kids[2]->prodrule != WHILE) && (parTree->kids[2]->prodrule != ELSE))
                {
                    leftOpType->primitive_Type = find_primitive_type(parTree->kids[2]->prodrule);
                }
            }
        }
    }

    // checking if right and left operand are either variables or literals and comparing its types
    if ((getRightOperand != NULL) && (getLeftOperand != NULL))
    {
        checkType(getRightOperand->type, getLeftOperand->type, parTree->kids[0]);
    }
    else if ((rightOpType != NULL) && (getLeftOperand != NULL))
    {
        checkType(rightOpType, getLeftOperand->type, parTree->kids[0]);
        // DEBUG(free(rightOpType)); better to move this...
    }
    else if ((getRightOperand != NULL) && (leftOpType != NULL))
    {
        checkType(getRightOperand->type, leftOpType, parTree->kids[0]);
        // DEBUG(free(leftOpType)); ...and this...
    }
    else if ((rightOpType != NULL) && (leftOpType != NULL))
    {
        checkType(rightOpType, leftOpType, parTree->kids[0]);
    }
    free(leftOpType); // ...to here
    free(rightOpType);
}

/**
 * @brief helper function to check if prodrule is methodHeader or Block
 * @param tree struct parameter that points to syntax tree node
 * @returns void
 */
void checkClassBodyDecl(Tree *parTree)
{
    typeptr *FindLeftType = NULL;
    SymbolTableEntry FindRight;
    struct SymbolTable *previousScope = NULL;
    previousScope = TypeCurrent;

    // allocate memory for the setType pointer
    if ((FindLeftType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for FindLeftType in checkClassBodyDecl\n");
        exit(ERROR_MEMORY);
    }

    for (int j = 0; j < parTree->nkids; j++)
    {
        if (parTree->kids[j]->prodrule == MethodDecl)
        {
            // looping through kids of MethodReturnVal
            for (int k = 0; k < parTree->kids[j]->nkids; k++)
            {
                if (parTree->kids[j]->kids[k]->prodrule == MethodHeader)
                {
                    changeMethodScope(parTree->kids[j]->kids[k]->kids[3]);
                }
                else if (parTree->kids[j]->kids[k]->prodrule == Block)
                {
                    checkBlockTypes(parTree->kids[j]->kids[k]);
                }
            }
            // changing current scope to previous scope
            TypeCurrent = previousScope;
        }
        else if (parTree->kids[j]->prodrule == ClassBodyDeclarations)
        {
            // calling recursively if ClassBodyDeclarations is found
            checkClassBodyDecl(parTree->kids[j]);
        }
        else if (parTree->kids[j]->prodrule == ClassBodyDeclaration)
        {
            if (parTree->kids[j]->kids[0]->prodrule > 1000)
            {
                // setting base type (not stored in ST)

                if (parTree->kids[j]->kids[0]->prodrule == VarDeclarator)
                {
                    // getting varDecleration type
                    FindRight = lookup_st(TypeCurrent, parTree->kids[j]->kids[0]->kids[0]->leaf->text);

                    if (FindRight == NULL)
                    {
                        PRINTERR("TYPE ERROR in %s on line %d: Variable Decleration error: %s needs to be declared first \n", yyfilename, parTree->kids[j]->kids[0]->kids[0]->leaf->lineno, parTree->kids[j]->kids[0]->kids[0]->leaf->text);
                        exit(ERROR_SEMANTIC);
                    }
                    else
                    {
                        FindLeftType->primitive_Type = find_primitive_type(parTree->kids[j]->kids[0]->kids[2]->prodrule);
                        checkType(FindLeftType, FindRight->type, parTree->kids[j]->kids[0]->kids[2]);
                    }
                }
            }
            else
            {
                if (parTree->kids[j]->kids[1]->prodrule == VarDeclarator)
                {
                    // getting varDecleration type
                    FindRight = lookup_st(TypeCurrent, parTree->kids[j]->kids[1]->kids[0]->leaf->text);

                    if (FindRight == NULL)
                    {
                        PRINTERR("TYPE ERROR in %s on line %d: Variable Decleration error: %s needs to be declared first \n", yyfilename, parTree->kids[j]->kids[1]->kids[0]->leaf->lineno, parTree->kids[j]->kids[1]->kids[0]->leaf->text);
                        exit(ERROR_SEMANTIC);
                    }
                    else
                    {
                        // setting base type (not stored in ST)
                        FindLeftType->primitive_Type = find_primitive_type(parTree->kids[j]->kids[1]->kids[2]->prodrule);
                        checkType(FindLeftType, FindRight->type, parTree->kids[j]->kids[1]->kids[2]);
                    }
                }
            }
        }
    }
    free(FindLeftType); // moved this here
}

/**
 * @brief changes current scope (TypeCurrent) to Package Symbol Table scope
 * @param tree struct parameter that points to syntax tree node
 * @returns void
 */
void checkLocVarDecType(Tree *parTree)
{
    typeptr *getType = NULL;
    SymbolTableEntry FindRight;

    // allocate memory for the setType pointer
    // getType = new_type(NULL);
    if ((getType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for setType in checkLocVarDecType\n");
        exit(ERROR_MEMORY);
    }

    if (parTree->kids[1]->prodrule == VarDeclarator)
    {
        // getting varDecleration type
        FindRight = lookup_st(TypeCurrent, parTree->kids[1]->kids[0]->leaf->text);

        if (FindRight == NULL)
        {
            PRINTERR("TYPE ERROR in %s on line %d: Variable Decleration error: %s needs to be declared first \n", yyfilename, parTree->kids[1]->kids[0]->leaf->lineno, parTree->kids[1]->kids[0]->leaf->text);
            exit(ERROR_SEMANTIC);
        }
        else
        {
            // setting base type (not stored in ST)
            getType->primitive_Type = find_primitive_type(parTree->kids[1]->kids[2]->prodrule);
            checkType(getType, FindRight->type, parTree->kids[1]->kids[0]);
        }
    }
    else if (parTree->kids[1]->prodrule == '[')
    {
        if (parTree->kids[3]->prodrule == ArrVarDeclarator)
        {
            checkArrayDeclTypes(parTree->kids[3]);
        }
    }
    free(getType); // moved this
}
/**
 * @brief check types when initializing array type
 *
 * @param parTree the parse tree node at which the method call is rooted
 */
void checkInitlArrElemType(Tree *parTree)
{
    int arrSize = 0, setSize = 0;
    SymbolTableEntry ArrType;

    typeptr *getArrayLitType = NULL, *getArrayType = NULL;

    // allocate memory for the setType pointer
    if ((getArrayLitType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for setType in checkInitlArrElemType\n");
        exit(ERROR_MEMORY);
    }

    // allocate memory for the setType pointer
    if ((getArrayType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for setType in checkInitlArrElemType\n");
        exit(ERROR_MEMORY);
    }

    // check Symbol table entry to find Id base type
    ArrType = lookup_st(TypeCurrent, parTree->kids[0]->leaf->text);

    // getting Array element type
    getArrayType->primitive_Type = ArrType->type->u.ArrayType.elemType->primitive_Type;

    // getting actual array size
    arrSize = ArrType->type->u.ArrayType.size;

    // setting insert array index size
    setSize = atoi(parTree->kids[2]->leaf->text);

    getArrayLitType->primitive_Type = find_primitive_type(parTree->kids[5]->prodrule);

    checkType(getArrayType, getArrayLitType, parTree->kids[5]);
    // checking insert arrayElement size with the size set for array
    if (arrSize < setSize)
    {
        PRINTERR("Array SIZE ERROR in %s on line %d: Array '%s' has %d  but more were provided.\n", yyfilename, parTree->kids[2]->leaf->lineno, parTree->kids[0]->leaf->text, arrSize);
        exit(ERROR_SYNTAX);
    }

    free(getArrayLitType);
    free(getArrayType);
}
/**
 * @brief check Array decleration types
 *
 * @param parTree the parse tree node at which the method call is rooted
 */
void checkArrayDeclTypes(Tree *parTree)
{
    SymbolTableEntry ArrType;
    typeptr *getType = NULL, *getArrayType = NULL;

    // allocate memory for the setType pointer
    if ((getType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for setType in checkArrayDeclTypes\n");
        exit(ERROR_MEMORY);
    }

    // allocate memory for the setType pointer
    if ((getArrayType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for setType in checkArrayDeclTypes\n");
        exit(ERROR_MEMORY);
    }

    if (parTree->kids[0]->prodrule == ID)
    {
        // check Symbol table entry to find Id base type
        ArrType = lookup_st(TypeCurrent, parTree->kids[0]->leaf->text);

        // getting Array element type
        getArrayType->primitive_Type = ArrType->type->u.ArrayType.elemType->primitive_Type;
    }

    if (parTree->kids[2]->prodrule == CreateArray)
    {
        getType->primitive_Type = find_primitive_type(parTree->kids[2]->kids[1]->prodrule);

        // Type checks array initializer Type and array type
        checkType(getArrayType, getType, parTree->kids[2]->kids[1]);
        free(getArrayType);
        free(getType);
    }
}

/**
 * @brief check that the method call is in the symbol table and that the arguments match the correct types
 *
 * @param parTree the parse tree node at which the method call is rooted
 */
void checkMethodCall(Tree *parTree)
{
    SymbolTableEntry target;
    char *funcName = getName(parTree->kids[0]);
    if ((target = search_st(TypeCurrent, funcName)) == NULL)
    { // check for target function call existence in the symtab
        PRINTERR("REFERENCE ERROR in %s on line %d: function '%s' is not defined in the current scope.\n", yyfilename, parTree->kids[1]->leaf->lineno, funcName);
        exit(ERROR_SEMANTIC);
    }
    // debug printers
    PRINTDEBUG("method call: ");
    if (debug)
        print_symbol_entry(target);
    PRINTDEBUG("\tFunction %s takes %d arguments.\n", funcName, target->type->u.MethodType.numParams);
    if (debug)
        print_tree(parTree, 10);

    // detect special cases with String OOP function call
    Tree *n; // find the node containing first name in qname
    for (n = parTree->kids[0]; n->prodrule == Name; n = n->kids[0]);
    if (n->prodrule == ID)
    { // attempt to find the first name as a STE visible from the current scope
        SymbolTableEntry ste = lookup_st(TypeCurrent, n->leaf->text);
        if (ste)
        {
            if (ste->type->primitive_Type == STRINGLIT_Type)
            {                                                 // if the found STE is a String type, attempt to pass it as the first argument to its own method call
                if (target->type->u.MethodType.numParams > 1) // if the method call also takes parameters, join them with the String as ArgList
                    checkMethodParams(createTree(ArgList, 3, n, ',', parTree->kids[2]), target, parTree->kids[1]->leaf->lineno, 0);
                else // if the method takes no other parameters, just pass the first name as a parameter
                    checkMethodParams(n, target, parTree->kids[1]->leaf->lineno, 0);
            }
        }
    }
    else
    { // if the first name is not a valid String ID, attempt to check it as a static built-in
        checkMethodParams(parTree->kids[2], target, parTree->kids[1]->leaf->lineno, 0);
    }

    // checkMethodParams(parTree->kids[2], target, 0);

    // for (int i = 0; i < target->type->u.MethodType.numParams; i++)
    // {
    //     DEBUGGER("\tArgument %d takes type: %s\n", i, get_type_string(target->type->u.MethodType.parameterList[i]->primitive_Type));
    // }
    // TODO finish checking argument type match
}

/**
 * @brief recursively walks through the passed parameters of a function call and type checks them
 *      (also compares number of params passed to the number of params expected)
 *
 * @param parTree the parse tre node at which the first method parameter is rooted
 * @param func the symbol table entry for the function being called
 * @param lineno the line number of the call
 * @param paramIndex the index of the parameter currently being checked (default to 0, used for recursive traversal)
 */
void checkMethodParams(Tree *parTree, SymbolTableEntry func, int lineno, int paramIndex)
{
    DEBUG(print_tree(parTree, 10));
    if (func->type->u.MethodType.numParams > 0)
    { // if the function is supposed to take parameters
        if (parTree == NULL)
        { // and if there are no parameters passed, throw an error
            PRINTERR("PARAMETER ERROR in %s on line %d: method '%s' takes %d arguments but none were provided.\n", yyfilename, lineno, func->s, func->type->u.MethodType.numParams);
            exit(ERROR_SEMANTIC);
        }
        else if (parTree->prodrule == ArgList)
        { // for multiple passed args, recursively visit all of them and typecheck along the way
            int passedParamType = getComplexParameterType(parTree->kids[0]);
            PRINTDEBUG("param %d type: %s\t expected: %s\n", paramIndex + 1, get_type_string(passedParamType), get_type_string(func->type->u.MethodType.parameterList[paramIndex]->primitive_Type));
            if (passedParamType != func->type->u.MethodType.parameterList[paramIndex]->primitive_Type)
            { // perform typecheck by comparing the type of the passed parameter to the expected type from the method STE
                PRINTERR("TYPE ERROR in %s on line %d: method '%s' expected parameter %d to have type %s but was passed type %s instead.\n", yyfilename, lineno, func->s, paramIndex + 1, get_type_string(func->type->u.MethodType.parameterList[paramIndex]->primitive_Type), get_type_string(passedParamType));
                exit(ERROR_SEMANTIC);
            }
            checkMethodParams(parTree->kids[2], func, lineno, paramIndex + 1); // recursive call on self when multiple arguments passed in function call
        }
        else
        { // for single arguments or the end of a parameter list
            int passedParamType = getComplexParameterType(parTree);
            PRINTDEBUG("param %d type: %s\t expected: %s\n", paramIndex + 1, get_type_string(passedParamType), get_type_string(func->type->u.MethodType.parameterList[paramIndex]->primitive_Type));
            if (passedParamType != func->type->u.MethodType.parameterList[paramIndex]->primitive_Type)
            { // perform typecheck
                PRINTERR("TYPE ERROR in %s on line %d: method '%s' expected parameter %d to have type %s but was passed type %s instead.\n", yyfilename, lineno, func->s, paramIndex + 1, get_type_string(func->type->u.MethodType.parameterList[paramIndex]->primitive_Type), get_type_string(passedParamType));
                exit(ERROR_SEMANTIC);
            }
            if (paramIndex + 1 < func->type->u.MethodType.numParams)
            {
                PRINTERR("PARAMETER ERROR in %s on line %d: method '%s' takes %d parameters but was passed only %d.\n", yyfilename, lineno, func->s, func->type->u.MethodType.numParams, paramIndex + 1);
                exit(ERROR_SEMANTIC);
            }
        }
    }
    else
    { // if the function takes no parameters
        if (parTree != NULL)
        { // and if a parameter was provided, throw an error and exit
            PRINTERR("PARAMETER ERROR in %s on line %d: method '%s' takes %d arguments but more were provided.\n", yyfilename, lineno, func->s, func->type->u.MethodType.numParams);
            exit(ERROR_SYNTAX);
        }
    }
}

int getComplexParameterType(Tree *parTree)
{
    // DEBUGGER("CurrentST: %s\n",TypeCurrent->name);
    int passedParamType = find_primitive_type(parTree->prodrule);
    if (passedParamType == ID_Type)
    {
        // DEBUGGER("Searching for parameter '%s'\n", parTree->leaf->text);
        SymbolTableEntry passedComplexParam = search_st(TypeCurrent, parTree->leaf->text);
        if (passedComplexParam)
        { // if the passed complex paramter exists in a visible scope
            passedParamType = passedComplexParam->type->primitive_Type;
        }
        else
        {
            PRINTERR("PARAMETER ERROR in %s on line %d: passed variable parameter '%s' is  not declared in the current scope.\n", yyfilename, parTree->leaf->lineno, parTree->leaf->text);
            exit(ERROR_SEMANTIC);
        }
    }
    else if (passedParamType == METHOD_Type)
    {
        char *paramName;
        if (parTree->kids[0]->prodrule == Name)
        {
            paramName = getName(parTree->kids[0]);
        }
        else
        {
            paramName = parTree->kids[0]->leaf->text;
        }
        PRINTDEBUG("Searching for parameter '%s'\n", paramName);
        DEBUG(print_tree(parTree, 10));
        SymbolTableEntry passedComplexParam = search_st(TypeCurrent, paramName);
        if (passedComplexParam)
        {
            DEBUG(print_symbol_entry(passedComplexParam));
            passedParamType = passedComplexParam->type->u.MethodType.returnType->primitive_Type;
            PRINTDEBUG("parameter %s found with return type %d\n", passedComplexParam->s, (passedParamType))
        }
        else
        {
            PRINTERR("PARAMETER ERROR in %s on line %d: passed method call parameter '%s' is not declared in the current scope.\n", yyfilename, parTree->kids[0]->leaf->lineno, paramName);
            exit(ERROR_SEMANTIC);
        }
    }
    return passedParamType;
}

/**
 * @brief get the qualified name by recursively handling the qualified name hierarchy structure
 *
 * @param parTree the parse tree node at which the qualified name name is rooted
 * @return char* the full qualified name of the function as a single string
 */
char *getName(Tree *parTree)
{
    // if package import has no . in it
    if (parTree->prodrule == ID)
        return parTree->leaf->text;

    char *qName; // the combined qualified name
    char *first; // the first name (before the .)
    char *last;  // the last name
    if (parTree->kids[0]->prodrule == Name)
        first = getName(parTree->kids[0]); // recursively build multi-level qualified names
    else
        first = parTree->kids[0]->leaf->text;
    last = parTree->kids[2]->leaf->text;

    // check the current scope for a symbol matching the first half of the qname
    SymbolTableEntry ste = search_st(TypeCurrent, first);
    if (ste)
    {
        PRINTDEBUG("Found symbol %s in qname on line %d\n", first, parTree->kids[0]->leaf->lineno);
        if (ste->type->primitive_Type == STRINGLIT_Type)
        {
            // generate the actuall method name for symtab lookup
            int len = strlen(last);
            char *StringMethodName = malloc(len + 8);
            strncpy(StringMethodName, "string.", 8);
            strncat(StringMethodName, last, len);
            SymbolTableEntry steMethod = search_st(globalST, StringMethodName);
            PRINTDEBUG("Searching for method '%s'\n", StringMethodName);
            if (steMethod)
            {
                PRINTDEBUG("Method '%s' found.\n", StringMethodName);
                qName = StringMethodName;
            }
            else
            {
                PRINTERR("SYNTAX ERROR in %s on line %d: undefined function call '%s.%s' encountered.\n", yyfilename, parTree->kids[2]->leaf->lineno, first, last);
                exit(ERROR_SEMANTIC);
            }
        }
        else
        {
            PRINTERR("SEMANTIC ERROR in %s on line %d: OOP-like function calls may only be applied to variables of type String in j0.\n", yyfilename, parTree->kids[2]->leaf->lineno);
            exit(ERROR_SEMANTIC);
        }
    }
    else
    {
        qName = malloc(sizeof(char) * (strlen(first) + 1 + strlen(last)));
        strncpy(qName, first, strlen(first));
        strncat(qName, ".", strlen(first) + 1);
        strncat(qName, last, strlen(first) + 1 + strlen(last));
    }
    return qName;
}

/**
 * @brief Finds the leftType in Assignment and calls checkAssignmentExprType if Assignment Epression is found
 * @param tree struct parameter that points to syntax tree node
 * @returns void
 */
void checkAssignmentType(Tree *parTree)
{
    SymbolTableEntry FindLeft = NULL;

    DEBUG(print_tree(parTree, 10));
    // uses left operands to loop up in symbol Table in Current scope
    FindLeft = search_st(TypeCurrent, parTree->kids[0]->leaf->text);

    if (FindLeft == NULL)
    {
        PRINTERR("%s:%d: Variable Decleration error: Variable %s need to be declared first \n", yyfilename, parTree->kids[0]->leaf->lineno, parTree->kids[0]->leaf->text);
        exit(ERROR_SEMANTIC);
    }

    if (parTree->kids[2] != NULL)
    {
        if (parTree->kids[2]->prodrule > 1000)
        {
            if (parTree->kids[2]->prodrule == CreateArray)
            {
                checkArrayDeclTypes(parTree);
            }
            else
            {
                checkAssignmentExprType(parTree->kids[2], FindLeft);
            }
        }
        else
        {
            typeptr *sourceType = malloc(sizeof(typeptr));
            sourceType->primitive_Type = find_primitive_type(parTree->kids[2]->prodrule);
            checkType(FindLeft->type, sourceType, parTree->kids[2]);
            free(sourceType); // don't need this after the typecheck
        }
    }
}
/**
 * @brief handles Assignment Epression found in Assignment expression
 * @param tree struct parameter that points to syntax tree node
 * @returns void
 */
void checkAssignmentExprType(Tree *parTree, SymbolTableEntry dest)
{
    SymbolTableEntry source = NULL;
    typeptr *setType = NULL;

    // allocate memory for the setType pointer
    if ((setType = malloc(sizeof(typeptr))) == NULL)
    {
        PRINTERR("Error allocating memory for setType in checkAssignmentExprType\n");
        exit(ERROR_MEMORY);
    }
    // DEBUG(print_tree(parTree, 10));
    // if the source is a method call, get the return type of the method
    if (parTree->prodrule == MethodCall)
    {
        // DEBUGGER("Method call '%s' on right of assignment.\n", parTree->kids[0]->leaf->text);
        char *methodName;
        if (parTree->kids[0]->prodrule == Name)
        {
            methodName = getName(parTree->kids[0]);
        }
        else
        {
            methodName = parTree->kids[0]->leaf->text;
        }
        source = search_st(TypeCurrent, methodName); // breaks here !!!!!!!!!!!
        if (source)
        {
            checkType(dest->type, source->type->u.MethodType.returnType, parTree->kids[0]);
        }
        else
        {
            PRINTERR("NAME ERROR in %s on line %d: function call to undefined method '%s' in assignment.\n", yyfilename, parTree->kids[1]->leaf->lineno, methodName);
            exit(ERROR_SEMANTIC);
        }
    }
    else if (parTree->prodrule == intArrLit)
    { // I know this solution is hacky and repetetive, but it works
        typeptr *elemType = NULL;
        if ((elemType = malloc(sizeof(typeptr))) == NULL)
        {
            PRINTERR("Error allocating memory for setType in checkAssignmentExprType\n");
            exit(ERROR_MEMORY);
        }
        setType->primitive_Type = ARRAY_Type;
        setType->u.ArrayType.elemType = elemType;
        elemType->primitive_Type = INTLIT_Type;
        checkType(dest->type, setType, parTree->kids[0]);
        free(elemType);
        free(setType);
    }
    else if (parTree->prodrule == longArrLit)
    {
        typeptr *elemType = NULL;
        if ((elemType = malloc(sizeof(typeptr))) == NULL)
        {
            PRINTERR("Error allocating memory for setType in checkAssignmentExprType\n");
            exit(ERROR_MEMORY);
        }
        setType->primitive_Type = ARRAY_Type;
        setType->u.ArrayType.elemType = elemType;
        elemType->primitive_Type = LONGLIT_Type;
        checkType(dest->type, setType, parTree->kids[0]);
        free(elemType);
        free(setType);
    }
    else if (parTree->prodrule == floatArrLit)
    {
        typeptr *elemType = NULL;
        if ((elemType = malloc(sizeof(typeptr))) == NULL)
        {
            PRINTERR("Error allocating memory for setType in checkAssignmentExprType\n");
            exit(ERROR_MEMORY);
        }
        setType->primitive_Type = ARRAY_Type;
        setType->u.ArrayType.elemType = elemType;
        elemType->primitive_Type = FLOATLIT_Type;
        checkType(dest->type, setType, parTree->kids[0]);
        free(elemType);
        free(setType);
    }
    else if (parTree->prodrule == doubleArrLit)
    {
        typeptr *elemType = NULL;
        if ((elemType = malloc(sizeof(typeptr))) == NULL)
        {
            PRINTERR("Error allocating memory for setType in checkAssignmentExprType\n");
            exit(ERROR_MEMORY);
        }
        setType->primitive_Type = ARRAY_Type;
        setType->u.ArrayType.elemType = elemType;
        elemType->primitive_Type = DOUBLELIT_Type;
        checkType(dest->type, setType, parTree->kids[0]);
        free(elemType);
        free(setType);
    }
    else if (parTree->prodrule == charArrLit)
    {
        typeptr *elemType = NULL;
        if ((elemType = malloc(sizeof(typeptr))) == NULL)
        {
            PRINTERR("Error allocating memory for setType in checkAssignmentExprType\n");
            exit(ERROR_MEMORY);
        }
        setType->primitive_Type = ARRAY_Type;
        setType->u.ArrayType.elemType = elemType;
        elemType->primitive_Type = CHARLIT_Type;
        checkType(dest->type, setType, parTree->kids[0]);
        free(elemType);
        free(setType);
    }
    else if (parTree->prodrule == stringArrLit)
    {
        typeptr *elemType = NULL;
        if ((elemType = malloc(sizeof(typeptr))) == NULL)
        {
            PRINTERR("Error allocating memory for setType in checkAssignmentExprType\n");
            exit(ERROR_MEMORY);
        }
        setType->primitive_Type = ARRAY_Type;
        setType->u.ArrayType.elemType = elemType;
        elemType->primitive_Type = STRINGLIT_Type;
        checkType(dest->type, setType, parTree->kids[0]);
        free(elemType);
        free(setType);
    }
    else if (parTree->prodrule == boolArrLit)
    {
        typeptr *elemType = NULL;
        if ((elemType = malloc(sizeof(typeptr))) == NULL)
        {
            PRINTERR("Error allocating memory for setType in checkAssignmentExprType\n");
            exit(ERROR_MEMORY);
        }
        setType->primitive_Type = ARRAY_Type;
        setType->u.ArrayType.elemType = elemType;
        elemType->primitive_Type = BOOLLIT_Type;
        checkType(dest->type, setType, parTree->kids[0]);
        free(elemType);
        free(setType);
    }
    else
    {
        for (int i = 0; i < parTree->nkids; i++)
        {
            // checking if the leaves contain non token values
            if (parTree->kids[i]->prodrule > 1000)
            {
                checkAssignmentExprType(parTree->kids[i], dest);
            }
            else
            {
                // Avoiding operands and just reading tokens
                if (parTree->kids[i]->prodrule > 250)
                {
                    // checking if assignment is ID
                    if (parTree->kids[i]->prodrule == ID)
                    {
                        source = search_st(TypeCurrent, parTree->kids[i]->leaf->text);
                        if (source == NULL)
                        {
                            PRINTERR("%s:%d: Variable Decleration error: Variable %s need to be declared first \n", yyfilename, parTree->kids[i]->leaf->lineno, parTree->kids[i]->leaf->text);
                            exit(ERROR_SEMANTIC);
                        }
                        checkType(dest->type, source->type, parTree->kids[i]);
                    }
                    else // if (parTree->kids[i]->prodrule)
                    {
                        setType->primitive_Type = find_primitive_type(parTree->kids[i]->prodrule);
                        checkType(dest->type, setType, parTree->kids[i]);
                    }
                }
            }
        }
        free(setType);
    }
}

/**
 * @brief validates left and right Types
 * @param tree struct parameter that points to syntax tree node
 * @param leftType struct parameter to point to the base type
 * @param rightType struct parameter to point to the base type
 * @returns void
 */
void checkType(typeptr *leftType, typeptr *rightType, Tree *parTree)
{
    // PRINTDEBUG("%lu\n",rightType);
    // Comparing if right base type and left right is equal
    if (leftType->primitive_Type != rightType->primitive_Type)
    {
        PRINTERR("TYPE ERROR in %s on line %d: Type mismatch between %s and %s.\n", yyfilename, parTree->leaf->lineno, get_type_string(leftType->primitive_Type), get_type_string(rightType->primitive_Type));
        exit(ERROR_SEMANTIC);
    }
    if (leftType->primitive_Type == ARRAY_Type)
    { // special case for array types, since element tyes must match
        if (leftType->u.ArrayType.elemType->primitive_Type != rightType->u.ArrayType.elemType->primitive_Type)
        {
            PRINTERR("TYPE ERROR in %s on line %d: Type mismatch between array[%s] and array[%s].\n", yyfilename, parTree->leaf->lineno, get_type_string(leftType->u.ArrayType.elemType->primitive_Type), get_type_string(rightType->u.ArrayType.elemType->primitive_Type));
            exit(ERROR_SEMANTIC);
        }
    }
}
