/**
 * @file icode.c
 * @author Joseph Coston & Saugat Sharma
 * @brief contains logic for the generation of intermediate code from the parse tree and symbol table
 * @version HW6
 * @date 2022-04-22
 */
#include "icode.h"

FILE *op; // the global output file pointer

/**
 * @brief generates Icode in the syntax tree for
 *
 * @param t
 */
void generateArithmeticExpression(Tree *t)
{
    // int size = t->kids[0]->address->size;
    t->address = newAddr(R_LOCAL, WORD_S);
    // print_tree(t, 10);
    int opCode;
    switch (t->kids[1]->leaf->category)
    {
    case '+':
        opCode = O_ADD;
        break;
    case '-':
        opCode = O_SUB;
        break;
    case '*':
        opCode = O_MUL;
        break;
    case '/':
        opCode = O_DIV;
        break;
    case LEQ:
        opCode = O_BLE;
        break;
    case GEQ:
        opCode = O_BGE;
        break;
    case LT:
        opCode = O_BLT;
        break;
    case GT:
        opCode = O_BGT;
        break;
    case EQ:
        opCode = O_BEQ;
        break;
    case NEQ:
        opCode = O_BNE;
        break;

    // case '%': // todo: find mod operator
    //     opCode = O_MOD;
    //     break;
    default:
        GENERR("encountered invaild operator (%d) in arithmetic expression.\n", t->kids[1]->leaf->category);
        break;
    }
    // join left and right child icode expressions
    t->icode = concat(t->kids[0]->icode, t->kids[2]->icode);
    t->icode = concat(t->icode, gen(opCode, t->address, t->kids[0]->address, t->kids[2]->address));
}

void generateMethodCall(Tree *t)
{
    SymbolTableEntry query;
    MEMCHECK(query = search_st(currentST, t->kids[0]->leaf->text), "unable to find method '%s' from scope of %s\n", t->kids[0]->leaf->text, currentST->name);
    t->address = query->address; // done in pre-order
    Addr *returnAddress = newAddr(R_LOCAL, WORD_S);
    print_tree(t,10);
    if (t->kids[2])                                                                                                                        // check first to see if params were passed
        t->icode = concat(t->icode, t->kids[2]->icode);                                                                                    // concatenate the parameter pushes, first
    t->icode = concat(t->icode, gen(O_CALL, t->address, newAddr(R_CONST, query->type->u.MethodType.numParams), returnAddress)); // generate the method call, itself
    t->address = returnAddress;                                                                                                           // set the method call address to its return value
}

/**
 * @brief genereate icode for assignment statements
 *
 * @param t pointer to the parse tree node at which the assignment statement is rooted
 */
void generateAssignment(Tree *t)
{
    t->address = t->kids[0]->address;
    switch (t->kids[1]->prodrule)
    {
    case INC: // expand increment out from x++ to x = x + 1
        t->icode = concat(t->icode, gen(O_ADD, t->address, t->kids[0]->address, newAddr(R_CONST, 1)));
        break;
    case DEC: // expand decrement out from x-- to x = x - 1
        t->icode = concat(t->icode, gen(O_SUB, t->address, t->kids[0]->address, newAddr(R_CONST, 1)));
        break;
    case ADDASS: // expand increment/assign from x += y to x = x + y
        t->icode = concat(t->icode, gen(O_ADD, t->address, t->kids[0]->address, t->kids[2]->address));
        break;
    case SUBASS: // expand increment/assign from x -= y to x = x - y
        t->icode = concat(t->icode, gen(O_SUB, t->address, t->kids[0]->address, t->kids[2]->address));
        break;
    case '=':
        t->icode = concat(t->kids[2]->icode, gen(O_ASN, t->kids[0]->address, t->kids[2]->address, NULL));
        break;
    default:
        UNKNOWNTREE(" assignemnt expression:\n");
        break;
    }
    // printInstr(t->icode);
}

void codegen(Tree *t)
{
    // int opCode = 0;
    // Instr *g = NULL;
    // return on null pointers
    if (t == NULL)
        return;

    SymbolTableEntry query = NULL;

    // pre-order activity:

    if (t->prodrule >= 1000)
    { // if the node is a nonterminal
        switch (t->prodrule)
        {
        case ClassDeclaration:
            // change scope on class declaration
            MEMCHECK(query = search_st(currentST, t->kids[2]->leaf->text), "symbol table search for '%s' failed in %s.\n", t->kids[2]->leaf->text, currentST->name);
            currentST = query->type->u.ClassType.scope;
            PRINTDEBUG("Entering scope of %s...\n", currentST->name);
            t->address = newAddr(R_GLOBAL, 1);
            resetOffset(R_CLASS);
            break;
        case MethodDecl:
            // change scope on method declaration
            MEMCHECK(query = search_st(currentST, t->kids[0]->kids[3]->kids[0]->leaf->text), "symbol table search for '%s' failed in %s.\n", t->kids[0]->kids[3]->kids[0]->leaf->text, currentST->name);
            currentST = query->type->u.MethodType.scope;
            PRINTDEBUG("Entering scope of %s...\n", currentST->name);
            // reset the local offset counter

            /* t->address = newAddr(R_CLASS, 1); // address gen for methods moved to hash.c at the point of STE insertion */
            t->address = query->address;
            MEMCHECK(t->address, "Method Declaration address gen failure.\n");

            /* t->address->u.name = query->s; // address gen for methods moved to hash.c at the point of STE insertion
                query->address = t->address; //*/

            // printAddr(t->address);
            resetOffset(R_LOCAL); // not sure this is right, yet... but resetting the local offset at the start of each method seems right.
            resetOffset(R_NAME);
            break;
        case LocalVarDecl: // assign new address to local variable
            if (t->kids[1]->prodrule == Name)
            {
                UNKNOWNTREE("Variable Declaration with Name prodrule\n");
            }
            else if (t->kids[1]->prodrule != '[')
            {
                // search the symtab for the declared variable
                MEMCHECK(query = search_st(currentST, getName(t->kids[1])), "symbol '%s' undefined in %s.\n", getName(t->kids[1]), currentST->name);
                // generate the new address and store it in the symtab for reference
                query->address = newAddr(R_NAME, 1); // getTypeSize(query->type->primitive_Type)); // apparently using constant 8-byte words for everything
                query->address->u.name = getName(t->kids[1]);
                t->address = query->address;                                             // also store a copy of the reference in the tree at the variable node in the tree
                t->icode = gen(D_LOCAL, query->address, newAddr(R_LOCAL, WORD_S), NULL); // generate a new local variable declaration instruction
            }
            else
            { // handle array declaration
                UNKNOWNTREE("unimplemented Variable Declaration type\n");
            }
            break;

        case FormalParm:
            t->address = newAddr(R_NAME, 1);                                     // declare the name of the parameter
            t->icode = gen(D_LOCAL, t->address, newAddr(R_LOCAL, WORD_S), NULL); // allocate a new local address for the parameter
            if (t->kids[1]->prodrule == '[')                                     // in the case that the parameter is an array
                t->icode->dest->u.name = t->kids[3]->leaf->text;
            else
                t->icode->dest->u.name = t->kids[1]->leaf->text;
            // find the corresponding STE
            MEMCHECK(query = search_st(currentST, t->icode->dest->u.name), "symbol '%s' undefined in %s.\n", t->icode->dest->u.name, currentST->name);
            query->address = t->address; // set the STE address to that of the param
            break;

        case ArgList:
            // t->address = t->kids[0]->address;
            break;

        default:
            break;
        }
    }

    if (t->nkids == 0)
    { // if the node is a leaf node (without children)
        switch (t->prodrule)
        {
        case ID:
            PRINTDEBUG("Visited leaf: ID '%s'\n", getName(t));
            MEMCHECK(query = search_st(currentST, getName(t)), "symbol '%s' on line %d undefined in %s (%s).\n", getName(t), t->leaf->lineno, currentST->name, yyfilename);
            if (query == NULL)
                return;

            // checking if it's methodType
            if (query->type->primitive_Type == METHOD_Type)
                return;

            t->address = query->address; // store referenced variable address in tree node address

            // if (query->type->primitive_Type >= INTLIT_Type && query->type->primitive_Type < ARRAY_Type)
            //     query->a = newtmp(lit_size[query->type->primitive_Type]);
            break;
        /* literal value cases */
        case BOOLLIT:
            PRINTDEBUG("Visited leaf: BOOL '%s'\n", t->leaf->text);
            t->address = newAddr(R_CONST, t->leaf->literal.ival);
            break;
        case INTLIT:
            PRINTDEBUG("Visited leaf: INT '%s'\n", t->leaf->text);
            t->address = newAddr(R_CONST, t->leaf->literal.ival);
            break;
        case CHARLIT:
            PRINTDEBUG("Visited leaf: CHAR '%s'\n", t->leaf->text);
            t->address = newAddr(R_CONST, t->leaf->literal.cval);
            break;
        case LONGLIT:
            PRINTDEBUG("Visited leaf: LONG '%s'\n", t->leaf->text);
            t->address = newAddr(R_CONST, t->leaf->literal.lval);
            break;
        case FLOATLIT:
            PRINTDEBUG("Visited leaf: FLOAT '%s'\n", t->leaf->text);
            t->address = newAddr(R_CONST, t->leaf->literal.fval);
            break;
        case DOUBLELIT:
            PRINTDEBUG("Visited leaf: FLOAT '%s'\n", t->leaf->text);
            t->address = newAddr(R_CONST, t->leaf->literal.dval);
            break;
        case STRINGLIT:
            PRINTDEBUG("Encountered string: %s\n", t->leaf->literal.sval);
            t->address = newStrAddr(t->leaf->literal.sval);
            break;
        default:
            break;
        }
    }

    // visit children:
    for (int i = 0; i < t->nkids; i++)
        codegen(t->kids[i]);

    // post-order activity:

    // assign icode at each tree node
    switch (t->prodrule)
    {
    case ClassDeclaration: // change scope after class declaration
        currentST = currentST->parent;
        break;
    case MethodDecl: // change scope after method declaration
        MEMCHECK(query = search_st(currentST, t->address->u.name), "method '%s' not in scope of %s", t->address->u.name, currentST->name);
        // print_tree(t, 10);
        t->icode = concat(concat(t->icode, t->kids[0]->icode), t->kids[1]->icode);
        t->icode = concat(gen(D_PROC, t->address, newAddr(R_CONST, query->type->u.MethodType.numParams), newAddr(R_CONST, getOffset(R_LOCAL))), t->icode);
        PRINTDEBUG("Exiting scope of %s...\n", currentST->name);
        PRINTOFILE("\n"); // separate methods (procedures) with newline
        printInstr(t->icode);
        currentST = currentST->parent;
        break;

    case LocalVarDecl:
        if (t->nkids == 4)
        {
            if (t->kids[2]->prodrule == '=')
            {
                t->icode = concat(t->icode, t->kids[3]->icode); // concatenate any icode on the RHS of a varaiable declaration
                t->icode = concat(t->icode, gen(O_ASN, t->address, t->kids[3]->address, NULL));
                // t->icode->next->dest = t->address; // this might work to reduce instructions, but might also fail in some cases.
            }
        }
        break;

    case AddExpr: // addition and subtraction
    case MulExpr: // multiplication and division
    case RelExpr:
    case EqExpr:
    case CondAndExpr:
    case CondOrExpr:
        generateArithmeticExpression(t);
        break;
    case UnaryExpr:
        // DEBUG(print_tree(t, 10));
        switch (t->kids[0]->prodrule)
        {
        case '-':
        case NOT:
            t->address = newAddr(R_LOCAL, WORD_S); // create a new local temp address
            t->icode = concat(t->kids[1]->icode, gen(O_NEG, t->address, t->kids[1]->address, NULL));
            break;
        default:
            UNKNOWNTREE("unary expression error.\n");
            break;
        }
        break;
    case Assignment:
        // DEBUG(print_tree(t, 10));
        generateAssignment(t);
        break;
    case MethodCall:
        // print_tree(t, 10);
        generateMethodCall(t);
        // printInstr(t->icode);
        break;
    case ArgList:
        if (t->kids[0]->prodrule == MethodCall)
        {
            t->icode = concat(t->icode, t->kids[0]->icode);
            t->icode = concat(t->icode, gen(O_PARM, t->kids[0]->address, NULL, NULL));
        }
        else
        {
            t->icode = concat(t->icode, gen(O_PARM, t->kids[0]->address, NULL, NULL));
        }

        if (t->kids[2]->prodrule == ArgList)
        { // if the arglist continues, do this
            t->icode = concat(t->icode, t->kids[2]->icode);
        }
        else if (t->kids[2]->prodrule == MethodCall)
        {
            t->icode = concat(t->icode, t->kids[2]->icode);
            t->icode = concat(t->icode, gen(O_PARM, t->kids[2]->address, NULL, NULL));
        }
        else
        { // if the arglist stops here, do this
            t->icode = concat(t->icode, gen(O_PARM, t->kids[2]->address, NULL, NULL));
        }
        break;

    /** @todo add missing cases **/
    default:
        /* default is: concatenate our children's code */
        // t->icode = NULL;
        for (int i = 0; i < t->nkids; i++)
            if (t->kids[i])
            {
                t->icode = concat(t->icode, t->kids[i]->icode);
            }
            else
            {
                continue;
            }
    }
}

void generate_intermediate_code(void)
{
    // generate the output's filename
    char *oldFileExtension = ".java";
    char *newFileExtension = ".ic";
    char *outFilename = calloc(strlen(yyfilename) - strlen(oldFileExtension) + strlen(newFileExtension) + 1, sizeof(char));
    strncpy(outFilename, yyfilename, strlen(yyfilename) - strlen(oldFileExtension));
    strncat(outFilename, newFileExtension, strlen(newFileExtension) + 1);
    PRINTVERBO("Generating intermediate code: %s\n", outFilename);

    // set the current scope to global
    currentST = globalST;
    // traverse the parse tree and generate icode at each node

    if ((op = fopen(outFilename, "w")) != NULL)
    {
        PRINTOFILE(".file\t\"%s\"\n", yyfilename); // print the filename

        string_constant_region.contents = calloc(1, sizeof(char)); // initialize the string constant region contents
        string_constant_region.size = 0;                           // initialize the size of constant region contents to 1 (null byte)

        codegen(root);
        printStringRegion();
        /** @todo generate icode here!! **/
    }
    else
    {
        PRINTERR("SYSTEM ERROR: Unable to open %s: No such file or directory.\n", outFilename);
        exit(-1);
    }

    // handle memory and streams
    fclose(op);
    free(outFilename);
}
