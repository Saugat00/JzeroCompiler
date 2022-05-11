/** Joseph Coston & Saugat Sharma
 *  CSE423 HW6
 *
 *  Grammar based on that provided in class, with added inspiration from https://cs.au.dk/~amoeller/RegAut/JavaBNF.html
 */
%{
        #include <stdio.h>
        #include <string.h>
        #include <stdlib.h>

        #include "tree.h"
        #include "token.h"
        #include "errcodes.h"
        
        Tree *root;

        extern int lineno;
        extern char *yytext;
        extern char *yyfilename;
        extern void yyerror(char* s);
        extern int yylex(void);
        #define YYDEBUG 1

%}

%union {
        Tree *treeptr;
}
//Jzero Package

%token  <treeptr> PACKAGE
%token  <treeptr> IMPORT

//Jzero reserved words
%token <treeptr>STRING
%token <treeptr>BOOL
%token <treeptr>CHAR    
%token <treeptr>DOUBLE  
%token <treeptr>FLOAT
%token <treeptr>INT
%token <treeptr>LONG      

%token <treeptr>BREAK   
%token <treeptr>CASE
%token <treeptr>CLASS   
%token <treeptr>CONTINUE
%token <treeptr>DEFAULT 
%token <treeptr>IF      
%token <treeptr>ELSE    
%token <treeptr>FOR     
%token <treeptr>NEW     
%token <treeptr>RETURN  
%token <treeptr>SWITCH  
%token <treeptr>PUBLIC  
%token <treeptr>STATIC  
%token <treeptr>VOID    
%token <treeptr>WHILE   

//error code thrown when Java reserved word not in Jzero found
%token <treeptr>NOTINJ0  280

//algebra
%token <treeptr>ADDASS 
%token <treeptr>SUBASS 
%token <treeptr>INC
%token <treeptr>DEC
%token <treeptr>'='
%token <treeptr>'+'
%token <treeptr>'-'
%token <treeptr>'*'
%token <treeptr>'/'
%token <treeptr>'%'

//comparators
%token <treeptr>EQ      
%token <treeptr>NEQ     
%token <treeptr>LT      
%token <treeptr>GT      
%token <treeptr>LEQ     
%token <treeptr>GEQ 

//boolean logic
%token <treeptr>AND     
%token <treeptr>OR      
%token <treeptr>NOT //is this used?     

//brackets/braces/parens
%token <treeptr>'['    
%token <treeptr>']'    
%token <treeptr>'('    
%token <treeptr>')'    
%token <treeptr>'{'    
%token <treeptr>'}'    

/*punctuation*/
%token <treeptr>':'   
%token <treeptr>';'   
%token <treeptr>','   
%token <treeptr>'.'     

/* type literals */  // DO NOT CHANGE ORDER! 
%token <treeptr>NULLVAL
%token <treeptr>BOOLLIT    
%token <treeptr>INTLIT     
%token <treeptr>CHARLIT    
%token <treeptr>LONGLIT    
%token <treeptr>FLOATLIT   
%token <treeptr>DOUBLELIT  
%token <treeptr>STRINGLIT
%token <treeptr>ID

/*=== non tokens ===*/
%type <treeptr> ClassDeclaration
%type <treeptr> Identifier
%type <treeptr> ClassBody
%type <treeptr> ClassBodyDeclarations
%type <treeptr> ClassBodyDeclaration
%type <treeptr> FieldDeclaration
%type <treeptr> Type
%type <treeptr> Name
%type <treeptr> QualifiedName
%type <treeptr> VarDeclarator
%type <treeptr> ArrVarDeclarator
%type <treeptr> MethodReturnVal
%type <treeptr> MethodDecl
%type <treeptr> MethodHeader
%type <treeptr> MethodDeclarator
%type <treeptr> FormalParmListOpt
%type <treeptr> FormalParmList
%type <treeptr> FormalParm
%type <treeptr> ConstructorDecl
%type <treeptr> ConstructorDeclarator
%type <treeptr> ArgListOpt
%type <treeptr> Block
%type <treeptr> BlockStmtsOpt
%type <treeptr> BlockStmts
%type <treeptr> BlockStmt
%type <treeptr> LocalVarDeclStmt
%type <treeptr> LocalVarDecl
%type <treeptr> Stmt
%type <treeptr> StmtNoShortIf
%type <treeptr> StmtWOTrailingStmt
%type <treeptr> ExprStmt
%type <treeptr> StmtExpr
%type <treeptr> IfThenStmt
%type <treeptr> IfThenElseStmt
%type <treeptr> IfThenElseStmtNoShortIf
%type <treeptr> WhileStmt
%type <treeptr> WhileStmtNoShortIf
%type <treeptr> ForStmt
%type <treeptr> ForStmtNoShortIf
%type <treeptr> ForInit
%type <treeptr> ExprOpt
%type <treeptr> ForUpdate
%type <treeptr> StmtExprList
%type <treeptr> BreakStmt
%type <treeptr> Primary
%type <treeptr> InstantiationExpr
%type <treeptr> ArgList
%type <treeptr> FieldAccess
%type <treeptr> MethodCall
%type <treeptr> PostFixExpr
%type <treeptr> UnaryExpr
%type <treeptr> MulExpr
%type <treeptr> AddExpr
%type <treeptr> RelOp
%type <treeptr> RelExpr
%type <treeptr> EqExpr
%type <treeptr> CondAndExpr
%type <treeptr> CondOrExpr
%type <treeptr> Expr
%type <treeptr> Assignment
%type <treeptr> LeftHandSide
%type <treeptr> AssignOp
%type <treeptr> Literal
%type <treeptr> ArrLiteral
%type <treeptr> NullArr
%type <treeptr> intLit
%type <treeptr> intArrLit
%type <treeptr> intArrOpt
%type <treeptr> longLit
%type <treeptr> longArrLit
%type <treeptr> longArrOpt
%type <treeptr> charLit
%type <treeptr> charArrLit
%type <treeptr> charArrOpt
%type <treeptr> floatLit
%type <treeptr> floatArrLit
%type <treeptr> floatArrOpt
%type <treeptr> doubleLit
%type <treeptr> doubleArrLit
%type <treeptr> doubleArrOpt
%type <treeptr> stringLit
%type <treeptr> stringArrLit
%type <treeptr> stringArrOpt
%type <treeptr> booleanLit
%type <treeptr> boolArrLit
%type <treeptr> boolArrOpt
%type <treeptr> SwitchStatement
%type <treeptr> SwitchBody
%type <treeptr> SwitchCases
%type <treeptr> SwitchCase
%type <treeptr> SwitchCaseBlock
%type <treeptr> VarDecls
%type <treeptr> ReturnStmt
%type <treeptr> ContinueStmt
%type <treeptr> packageDeclaration
%type <treeptr> importDeclaration
%type <treeptr> importDeclarations
%type <treeptr> singleTypeImportDeclaration
%type <treeptr> typeImportOnDemandDeclaration
%type <treeptr> packageName
%type <treeptr> typeName
%type <treeptr> typeDeclaration
%type <treeptr> compilationUnit
%type <treeptr> CreateArray
%type <treeptr> InitializeArrayElement
%type <treeptr> AssignArrayElement
%start      compilationUnit

%%


compilationUnit: packageDeclaration importDeclarations typeDeclaration      { $$ = createTree(compilationUnit, 3 , $1, $2, $3); root = $$;  }
    | packageDeclaration typeDeclaration                                    { $$ = createTree(compilationUnit, 2 , $1, $2); root = $$; }
    | importDeclarations typeDeclaration                                    { $$ = createTree(compilationUnit, 2 , $1, $2); root = $$; } 
    | typeDeclaration                                                       { $$ = createTree(compilationUnit, 1 , $1); root = $$; }
    ;

packageDeclaration: PACKAGE packageName ';'                             { $$ = createTree(packageDeclaration, 3 , $1, $2, $3);}
    ;

importDeclarations: importDeclaration                                   { $$ = createTree(importDeclaration, 1 , $1); }
    |importDeclarations importDeclaration                                   { $$ = createTree(importDeclaration, 2 , $1, $2); }
    ;

importDeclaration: singleTypeImportDeclaration                          { $$ = createTree(importDeclaration, 1 , $1); }
    | typeImportOnDemandDeclaration                                        { $$ = createTree(importDeclaration, 1 , $1); }
    ;

singleTypeImportDeclaration: IMPORT typeName ';'                        { $$ = createTree(singleTypeImportDeclaration, 3 , $1, $2, $3); }
    ;

typeImportOnDemandDeclaration: IMPORT packageName '.' '*' ';'           { $$ = createTree(typeImportOnDemandDeclaration, 5 , $1, $2, $3, $4, $5); }
    ;

typeDeclaration: ClassDeclaration                                       { $$ = createTree(typeDeclaration, 1 , $1); } 
    |                                                                       { $$ = NULL; } 
    ;


packageName: Identifier                                                 { $$ = createTree(packageName, 1 , $1); } 
    | packageName '.' Identifier                                            { $$ = createTree(packageName, 3 , $1, $2, $3); }
    ;

typeName: Identifier                                                    { $$ = createTree(typeName, 1 , $1); } 
    | packageName '.' Identifier                                            { $$ = createTree(typeName, 3 , $1, $2, $3); }
    ;


ClassDeclaration: PUBLIC CLASS Identifier ClassBody       { $$ = createTree(ClassDeclaration, 4 , $1, $2, $3, $4);}
    ;

Identifier: ID                                            { $$ = createTree(Identifier, 1 , $1); }
    ;


ClassBody: '{' ClassBodyDeclarations '}'                  { $$ = createTree(ClassBody, 3 , $1, $2, $3); }
    | '{' '}'                                               { $$ = createTree(ClassBody, 2 , $1, $2); }
    ;


ClassBodyDeclarations: ClassBodyDeclaration               { $$ = createTree(ClassBodyDeclarations, 1 , $1); }
    | ClassBodyDeclarations ClassBodyDeclaration            { $$ = createTree(ClassBodyDeclarations, 2 , $1, $2); }
    ;


ClassBodyDeclaration: FieldDeclaration                      { $$ = createTree(ClassBodyDeclaration, 1 , $1); }
    | MethodDecl                                            { $$ = createTree(ClassBodyDeclaration, 1 , $1); }
    | ConstructorDecl                                       { $$ = createTree(ClassBodyDeclaration, 1 , $1); }
    ;

// // old
// FieldDeclaration: Type VarDecls ';'   { $$ = createTree(FieldDeclaration, 3 , $1, $2, $3); }
//     | VarDecls ';'                    { $$ = createTree(FieldDeclaration, 2 , $1, $2); }
//     ;
// //

FieldDeclaration: PUBLIC STATIC MethodReturnVal VarDecls ';'   { $$ = createTree(FieldDeclaration, 3 , $3, $4, $5); }
    |VarDecls ';'                    { $$ = createTree(FieldDeclaration, 2 , $1, $2); }
    ;

Type: INT                                         { $$ = createTree(Type, 1 , $1); }  
    | LONG                                          { $$ = createTree(Type, 1 , $1); }
    | FLOAT                                         { $$ = createTree(Type, 1 , $1); }
    | DOUBLE                                        { $$ = createTree(Type, 1 , $1); }
    | BOOL                                          { $$ = createTree(Type, 1 , $1); } 
    | CHAR                                          { $$ = createTree(Type, 1 , $1); } 
    | STRING                                        { $$ = createTree(Type, 1 , $1); }
    | Name                                          { $$ = createTree(Type, 1 , $1); }
    ;
    

Name: Identifier                                  { $$ = createTree(Name, 1 , $1); }
    | QualifiedName  '.' Identifier                               { $$ = createTree(Name, 3 , $1, $2, $3); }
    ;

QualifiedName: Identifier               {$$ = createTree(QualifiedName, 1 , $1);}
    | QualifiedName '.' Identifier        { $$ = createTree(QualifiedName, 3 , $1, $2, $3); }
    ;


VarDecls: VarDeclarator                      { $$ = createTree(VarDecls, 1 , $1); }
    | VarDecls ',' VarDeclarator             { $$ = createTree(VarDecls, 3 , $1, $2, $3); }
    ;

ArrVarDeclarator: Identifier        { $$ = createTree(ArrVarDeclarator, 1 , $1); }
    | Identifier '=' ArrLiteral       { $$ = createTree(ArrVarDeclarator, 3 , $1, $2, $3); }
    | Identifier '=' CreateArray                { $$ = createTree(ArrVarDeclarator, 3 , $1, $2, $3); }
    ;

// this should probably be intlit, not literal
CreateArray: NEW Type '[' Literal ']'               { $$ = createTree(CreateArray, 5 , $1, $2, $3, $4, $5); }   
    ;


VarDeclarator: Identifier                         { $$ = createTree(VarDeclarator, 1 , $1); } 
    | Identifier '=' Literal                      { $$ = createTree(VarDeclarator, 3 , $1, $2, $3); }
    | Identifier '=' MethodCall                     { $$ = createTree(VarDeclarator, 3 , $1, $2, $3); }
    ;

MethodReturnVal : Type                            { $$ = createTree(MethodReturnVal, 1 , $1); }
    | Type '[' ']'                                  { $$ = createTree(MethodReturnVal, 3 , $1, $2, $3); }
    | VOID                                          { $$ = createTree(MethodReturnVal, 1 , $1); }
    ;

MethodDecl: MethodHeader Block                    { $$ = createTree(MethodDecl, 2 , $1, $2); }
    ;


MethodHeader: PUBLIC STATIC MethodReturnVal MethodDeclarator
    { $$ = createTree(MethodHeader, 4 , $1, $2, $3, $4); }
    ;


MethodDeclarator: Identifier '(' FormalParmListOpt ')'    { $$ = createTree(MethodDeclarator, 4 , $1, $2, $3, $4); }
    | Identifier '(' ')'                                                                        { $$ = createTree(MethodDeclarator, 3 , $1, $2, $3); }
    ;

FormalParmListOpt: FormalParmList                   { $$ = createTree(FormalParmListOpt, 1 , $1); }
    // | /*epsilon*/                                     { $$ = NULL; } // don't think this needs to be here
    ;

    // TODO probably broken, fix later...
FormalParmList: FormalParm                          { $$ = createTree(FormalParmList, 1 , $1); }
    | FormalParm ',' FormalParm           { $$ = createTree(FormalParmList, 3 , $1, $2, $3); }
    | FormalParm ',' FormalParm ',' FormalParm          { $$ = createTree(FormalParmList, 5 , $1, $2, $3, $4, $5); }
    | FormalParm ',' FormalParm ',' FormalParm ',' FormalParm           { $$ = createTree(FormalParmList, 7 , $1, $2, $3, $4, $5, $6, $7); }
    ;

FormalParm: Type VarDeclarator                      { $$ = createTree(FormalParm, 2 , $1, $2); }
    | Type '[' ']' ArrVarDeclarator                      { $$ = createTree(FormalParm, 4 , $1, $2, $3, $4); }
    ;

ConstructorDecl: ConstructorDeclarator Block        { $$ = createTree(ConstructorDecl, 2 , $1, $2); }       
    ;


ConstructorDeclarator: Identifier '(' FormalParmListOpt ')'
    { $$ = createTree(ConstructorDeclarator, 4 , $1, $2, $3, $4); }
    ;

ArgListOpt:  ArgList                                  { $$ = createTree(ArgListOpt, 1 , $1); }
    |/*epsilon*/                                        { $$ = NULL; }
    ;

Block: '{' BlockStmtsOpt '}'                          { $$ = createTree(Block, 3, $1, $2, $3); }
    |   '{' '}'                                     { $$ = createTree(Block, 2, $1, $2); }
    ;

BlockStmtsOpt: BlockStmts                             { $$ = createTree(BlockStmtsOpt, 1, $1); }
    ;

BlockStmts:  BlockStmt                                { $$ = createTree(BlockStmts, 1, $1); }
    | BlockStmts BlockStmt                              { $$ = createTree(BlockStmt, 2, $1, $2); }
    ;


BlockStmt: LocalVarDeclStmt                           { $$ = createTree(BlockStmt, 1, $1); }
    | Stmt                                              { $$ = createTree(BlockStmt, 1, $1); }
    ;



LocalVarDeclStmt: LocalVarDecl ';'                    { $$ = createTree(LocalVarDeclStmt, 2, $1, $2); }
    ;


LocalVarDecl: Type VarDecls                           { $$ = createTree(LocalVarDecl, 2, $1, $2); }
    | Type ID '=' Expr                      { $$ = createTree(LocalVarDecl, 4, $1, $2, $3, $4); }
    | Type '[' ']' ArrVarDeclarator                   { $$ = createTree(LocalVarDecl, 4, $1, $2, $3, $4); }
    | InitializeArrayElement                        { $$ = createTree(LocalVarDecl, 1, $1); }
    | InstantiationExpr                             { $$ = createTree(LocalVarDecl, 1, $1); }                       
    ;


InitializeArrayElement: Identifier '[' Literal ']' '=' Literal      { $$ = createTree(InitializeArrayElement, 6 , $1, $2, $3, $4, $5, $6); }
    ;

AssignArrayElement: Identifier '[' Literal ']'                      { $$ = createTree(AssignArrayElement, 4 , $1, $2, $3, $4); }
    ;

StmtWOTrailingStmt: Block     { $$ = createTree(StmtWOTrailingStmt, 1, $1); }
    | ';'                       { $$ = createTree(StmtWOTrailingStmt, 1, $1); }
    | ExprStmt                  { $$ = createTree(StmtWOTrailingStmt, 1, $1); }
    | SwitchStatement           { $$ = createTree(StmtWOTrailingStmt, 1, $1); }
    | BreakStmt                 { $$ = createTree(StmtWOTrailingStmt, 1, $1); }
    | ContinueStmt              { $$ = createTree(StmtWOTrailingStmt, 1, $1); }
    | ReturnStmt                { $$ = createTree(StmtWOTrailingStmt, 1, $1); }
    ;

Stmt: StmtWOTrailingStmt      { $$ = createTree(Stmt, 1, $1); }
    | IfThenStmt                { $$ = createTree(Stmt, 1, $1); }
    | IfThenElseStmt            { $$ = createTree(Stmt, 1, $1); }
    | WhileStmt                 { $$ = createTree(Stmt, 1, $1); }
    | ForStmt                   { $$ = createTree(Stmt, 1, $1); }
    ;

StmtNoShortIf: StmtWOTrailingStmt     { $$ = createTree(StmtNoShortIf, 1, $1); } 
    | IfThenElseStmtNoShortIf           { $$ = createTree(StmtNoShortIf, 1, $1); }
    | WhileStmtNoShortIf                { $$ = createTree(StmtNoShortIf, 1, $1); }
    | ForStmtNoShortIf                  { $$ = createTree(StmtNoShortIf, 1, $1); }
    ;


ExprStmt: StmtExpr ';'        { $$ = createTree(ExprStmt, 2, $1, $2); }
    ;

StmtExpr: Assignment          { $$ = createTree(StmtExpr, 1, $1); }
    | MethodCall                { $$ = createTree(StmtExpr, 1, $1); }
    ;
    

IfThenStmt: IF '(' Expr ')' Stmt
    { $$ = createTree(IfThenStmt, 5 , $1, $2, $3, $4, $5); }
    ;

IfThenElseStmt: IF '(' Expr ')' StmtNoShortIf ELSE Stmt
    { $$ = createTree(IfThenElseStmt, 7 , $1, $2, $3, $4, $5, $6, $7); }
    ;

IfThenElseStmtNoShortIf: IF '(' Expr ')' StmtNoShortIf ELSE StmtNoShortIf
    { $$ = createTree(IfThenElseStmtNoShortIf, 7 , $1, $2, $3, $4, $5, $6, $7); }
    ;


WhileStmt: WHILE '(' Expr ')' Stmt
    { $$ = createTree(WhileStmt, 5 , $1, $2, $3, $4, $5); }
    ;

WhileStmtNoShortIf: WHILE '(' Expr ')' StmtNoShortIf
    { $$ = createTree(WhileStmtNoShortIf, 5 , $1, $2, $3, $4, $5); }
    ;


ForStmt: FOR '(' ForInit ';' ExprOpt ';' ForUpdate ')' Stmt
    { $$ = createTree(ForStmt, 9 , $1, $2, $3, $4, $5, $6, $7, $8, $9); }
    ;

ForStmtNoShortIf: FOR '(' ForInit ';' ExprOpt ';' ForUpdate ')' StmtNoShortIf
    { $$ = createTree(ForStmtNoShortIf, 9 , $1, $2, $3, $4, $5, $6, $7, $8, $9); }
    ;

ForInit: StmtExprList         { $$ = createTree(ForInit, 1, $1); }
    | LocalVarDecl              { $$ = createTree(ForInit, 1, $1); }
    |                           { $$ = NULL; }
    ;

ExprOpt: Expr                 { $$ = createTree(ExprOpt, 1, $1); }
    |                           { $$ = NULL; }
    ;

ForUpdate: StmtExprList       { $$ = createTree(ForUpdate, 1, $1); }
    |                           { $$ = NULL; }
    ;

StmtExprList: StmtExpr                    { $$ = createTree(StmtExprList, 1, $1); }
    | StmtExprList ',' StmtExpr     { $$ = createTree(StmtExprList, 3, $1, $2, $3); }
    ;

BreakStmt: BREAK ';'        { $$ = createTree(BreakStmt, 2, $1, $2); }
    | BREAK Identifier ';'    { $$ = createTree(BreakStmt, 3, $1, $2, $3); }
    ;


ReturnStmt: RETURN ExprOpt ';'    { $$ = createTree(ReturnStmt, 3, $1, $2, $3); }
    |RETURN ';'    { $$ = createTree(ReturnStmt, 1, $1, $2); }
    ;

ContinueStmt: CONTINUE ';'      { $$ = createTree(ContinueStmt, 2, $1, $2); }
    | CONTINUE Identifier ';'     { $$ = createTree(ContinueStmt, 3, $1, $2, $3); }
    ;

Primary: Literal              { $$ = createTree(Primary, 1, $1); }
    | ArrLiteral                { $$ = createTree(Primary, 1, $1); }
    | '(' Expr ')'              { $$ = createTree(Primary, 1, $2); }
    | FieldAccess               { $$ = createTree(Primary, 1, $1); }
    ;


InstantiationExpr: Identifier Name '=' NEW Name '(' ArgListOpt ')'    { $$ = createTree(InstantiationExpr, 8, $1, $2, $3, $4, $5, $6, $7, $8); }    
    ;


ArgList: Expr                   { $$ = createTree(ArgList, 1, $1); } 
    | Expr ',' ArgList          { $$ = createTree(ArgList, 3, $1, $2, $3); } 
    ;

FieldAccess: Primary '.' Identifier     { $$ = createTree(FieldAccess, 3, $1, $2, $3); }
    ;


MethodCall: Name '(' ArgListOpt ')'     { $$ = createTree(MethodCall, 4, $1, $2, $3, $4); }           
    | Name '{' ArgListOpt '}'           { $$ = createTree(MethodCall, 4, $1, $2, $3, $4); } // not sure, but I think this is invalid
    | FieldAccess '(' ArgListOpt ')'    { $$ = createTree(MethodCall, 4, $1, $2, $3, $4); }
    | FieldAccess '{' ArgListOpt '}'    { $$ = createTree(MethodCall, 4, $1, $2, $3, $4); } // and this...
    ;


PostFixExpr: Primary            { $$ = createTree(PostFixExpr, 1, $1); }
    | Name                      { $$ = createTree(PostFixExpr, 1, $1); }
    ;

UnaryExpr: '-' UnaryExpr        { $$ = createTree(UnaryExpr, 2, $1, $2); } 
    | NOT UnaryExpr             { $$ = createTree(UnaryExpr, 2, $1, $2); } 
    | PostFixExpr               { $$ = createTree(UnaryExpr, 1, $1); }
    ;

MulExpr: UnaryExpr              { $$ = createTree(MulExpr, 1, $1); }
    | MulExpr '*' UnaryExpr     { $$ = createTree(MulExpr, 3, $1, $2, $3); }
    | MulExpr '/' UnaryExpr     { $$ = createTree(MulExpr, 3, $1, $2, $3); }                 
    | MulExpr '%' UnaryExpr     { $$ = createTree(MulExpr, 3, $1, $2, $3); }
    | MulExpr '*' AssignArrayElement        { $$ = createTree(MulExpr, 3, $1, $2, $3); } 
    | MulExpr '/' AssignArrayElement        { $$ = createTree(MulExpr, 3, $1, $2, $3); }
    | MulExpr '%' AssignArrayElement        { $$ = createTree(MulExpr, 3, $1, $2, $3); } 
    ;


AddExpr: MulExpr                { $$ = createTree(AddExpr, 1, $1); }
    | AddExpr '+' MulExpr       { $$ = createTree(AddExpr, 3, $1, $2, $3); }
    | AddExpr '-' MulExpr       { $$ = createTree(AddExpr, 3, $1, $2, $3); }
    | AddExpr '+' AssignArrayElement        { $$ = createTree(AddExpr, 3, $1, $2, $3); }
    | AddExpr '-' AssignArrayElement        { $$ = createTree(AddExpr, 3, $1, $2, $3); }
    ;


RelOp: LEQ                      { $$ = createTree(RelOp, 1, $1); }
    | GEQ                       { $$ = createTree(RelOp, 1, $1); } 
    | LT                        { $$ = createTree(RelOp, 1, $1); }
    | GT                        { $$ = createTree(RelOp, 1, $1); }
    ;


RelExpr: AddExpr                { $$ = createTree(RelExpr, 1, $1); }
    | RelExpr RelOp AddExpr     { $$ = createTree(RelExpr, 3, $1, $2, $3); }
    ;

EqExpr: RelExpr             { $$ = createTree(EqExpr, 1, $1); }                         
    | EqExpr EQ RelExpr     { $$ = createTree(EqExpr, 3, $1, $2, $3); }
    | EqExpr NEQ RelExpr    { $$ = createTree(EqExpr, 3, $1, $2, $3); }
    ;


CondAndExpr: EqExpr             { $$ = createTree(CondAndExpr, 1, $1); }
    | CondAndExpr AND EqExpr    { $$ = createTree(CondAndExpr, 3, $1, $2, $3); }
    ;


CondOrExpr: CondAndExpr         { $$ = createTree(CondOrExpr, 1, $1); }
    | CondOrExpr OR CondAndExpr { $$ = createTree(CondOrExpr, 3, $1, $2, $3); }



Expr: CondOrExpr                { $$ = createTree(Expr, 1, $1); } 
    | Assignment                { $$ = createTree(Expr, 1, $1); }
    | MethodCall                { $$ = createTree(Expr, 1, $1); }
    | CreateArray               { $$ = createTree(Expr, 1, $1); }
    ;


Assignment: LeftHandSide AssignOp Expr          { $$ = createTree(Assignment, 3, $1, $2, $3); }
    | LeftHandSide INC                          { $$ = createTree(Assignment, 2, $1, $2); }
    | LeftHandSide DEC                          { $$ = createTree(Assignment, 2, $1, $2); }



LeftHandSide: Name  { $$ = createTree(LeftHandSide, 1, $1); }
    | FieldAccess   { $$ = createTree(LeftHandSide, 1, $1); } 
    ;

AssignOp: '='       { $$ = createTree(AssignOp, 1, $1); }
    | ADDASS        { $$ = createTree(AssignOp, 1, $1); }
    | SUBASS        { $$ = createTree(AssignOp, 1, $1); }
    ;

Literal:
      intLit        { $$ = createTree(Literal, 1, $1); }
    | longLit       { $$ = createTree(Literal, 1, $1); }
    | charLit       { $$ = createTree(Literal, 1, $1); }
    | floatLit      { $$ = createTree(Literal, 1, $1); }
    | doubleLit     { $$ = createTree(Literal, 1, $1); }
    | stringLit     { $$ = createTree(Literal, 1, $1); }
    | booleanLit    { $$ = createTree(Literal, 1, $1); }
    | NULLVAL       { $$ = createTree(Literal, 1, $1); }
    ;

ArrLiteral:
      intArrLit     { $$ = createTree(ArrLiteral, 1, $1); }
    | longArrLit    { $$ = createTree(ArrLiteral, 1, $1); }
    | charArrLit    { $$ = createTree(ArrLiteral, 1, $1); }
    | floatArrLit   { $$ = createTree(ArrLiteral, 1, $1); }
    | doubleArrLit  { $$ = createTree(ArrLiteral, 1, $1); }
    | stringArrLit  { $$ = createTree(ArrLiteral, 1, $1); }
    | boolArrLit    { $$ = createTree(ArrLiteral, 1, $1); }
    | NullArr       { $$ = createTree(ArrLiteral, 1, $1); }
    ;

NullArr: '{' '}'      { $$ = createTree(NullArr, 2 , $1, $2); }
    ;
intLit: INTLIT          { $$ = createTree(intLit, 1, $1); }
    ;

intArrLit: '{' intArrOpt '}'    { $$ = createTree(intArrLit, 3 , $1, $2, $3); }
    ;
intArrOpt: INTLIT               { $$ = createTree(intArrOpt, 1 , $1); }
    | intArrOpt ',' INTLIT      { $$ = createTree(intArrOpt, 3 , $1, $2, $3); }
    ;

longLit: LONGLIT    { $$ = createTree(longLit, 1, $1); }
    ;

longArrLit: '{' longArrOpt '}'  { $$ = createTree(longArrLit, 3 , $1, $2, $3); }
    ;
longArrOpt: LONGLIT             { $$ = createTree(longArrOpt, 1 , $1); }
    | longArrOpt ',' LONGLIT    { $$ = createTree(longArrOpt, 3 , $1, $2, $3); }
    ;

charLit: CHARLIT        { $$ = createTree(charLit, 1, $1); }
    ;

charArrLit: '{' charArrOpt '}'  { $$ = createTree(charArrLit, 3 , $1, $2, $3); }
    ;
charArrOpt: CHARLIT             { $$ = createTree(charArrOpt, 1 , $1); }
    | charArrOpt ',' CHARLIT    { $$ = createTree(charArrOpt, 3 , $1, $2, $3); }
    ;

floatLit: FLOATLIT  { $$ = createTree(floatLit, 1, $1); }
    ;

floatArrLit: '{' floatArrOpt '}'  { $$ = createTree(floatArrLit, 3 , $1, $2, $3); }
    ;
floatArrOpt: FLOATLIT             { $$ = createTree(floatArrOpt, 1 , $1); }
    | floatArrOpt ',' FLOATLIT  { $$ = createTree(floatArrOpt, 3 , $1, $2, $3); }
    ;

doubleLit: DOUBLELIT  { $$ = createTree(doubleLit, 1, $1); }
    ;

doubleArrLit: '{' doubleArrOpt '}'  { $$ = createTree(doubleArrLit, 3 , $1, $2, $3); }
    ;
doubleArrOpt: DOUBLELIT             { $$ = createTree(doubleArrOpt, 1 , $1); }
    | doubleArrOpt ',' DOUBLELIT    { $$ = createTree(doubleArrOpt, 3 , $1, $2, $3); }
    ;

stringLit: STRINGLIT  { $$ = createTree(stringLit, 1, $1); }
    ;

stringArrLit: '{' stringArrOpt '}'  { $$ = createTree(stringArrLit, 3 , $1, $2, $3); }
    ;

stringArrOpt: STRINGLIT             { $$ = createTree(stringArrOpt, 1 , $1); }
    | stringArrOpt ',' STRINGLIT      { $$ = createTree(stringArrOpt, 3 , $1, $2, $3); }
    ;

booleanLit: BOOLLIT   { $$ = createTree(booleanLit, 1, $1); }
    ;

boolArrLit: '{' boolArrOpt '}'  { $$ = createTree(boolArrLit, 3 , $1, $2, $3); }
    ;

boolArrOpt: BOOLLIT             { $$ = createTree(boolArrOpt, 1 , $1); }
    | boolArrOpt ',' BOOLLIT      { $$ = createTree(boolArrOpt, 3 , $1, $2, $3); }
    ;

SwitchStatement: SWITCH '(' Expr ')' '{' SwitchBody '}'
        { $$ = createTree(SwitchStatement, 7, $1, $2, $3, $4, $5, $6, $7); }
    ;

SwitchBody: SwitchCases SwitchCaseBlock     { $$ = createTree(SwitchBody, 2, $1, $2); } 
    ;

SwitchCases: SwitchCaseBlock                { $$ = createTree(SwitchCases, 1, $1); }
    | SwitchCases SwitchCaseBlock               { $$ = createTree(SwitchCases, 2, $1, $2); }                
    ;


SwitchCaseBlock: SwitchCase BlockStmtsOpt   { $$ = createTree(SwitchCaseBlock, 2, $1, $2); } 
    ;

SwitchCase: CASE Literal ':'                { $$ = createTree(SwitchCase, 3, $1, $2, $3); }
    | DEFAULT ':'   Stmt                        { $$ = createTree(SwitchCase, 3, $1, $2, $3); }                             
    ;

%%

void yyerror(char *s)
{
    PRINTERR("%s:%d: %s before '%s' token\n", yyfilename, yylineno, s, yytext);
    exit(ERROR_SYNTAX);
}