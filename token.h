/**
 * @file token.h
 * @author Saugat Sharma & Joseph Coston
 * @brief contains definitions of the token & tokenlist structs, and descriptions of function  or operating on said structures
 * @version HW6
 * @date 2022-04-06
 */

#ifndef TOKEN_H
#define TOKEN_H

#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define GETNAME(variable) #variable


/*=====structure declaration====*/

/*definition of a token*/
typedef struct token
{
    int category;   // the integer code returned by yylex
    char *text;     // the actual string (lexeme) matched
    int lineno;     // the line number on which the token occurs
    char *filename; // the source file in which the token occurs
    union literal_u
    {
        char cval;   // for character literals, store here
        int ival;    // for integer and boolean literals, store here
        long lval;   // for long integer literals, store here
        float fval;  // for single-precision floating point literals, store here
        double dval; // for real literals, store binary value here
        char *sval;  // for string literals, malloc space, de-escape, store
                     // the string (less quotes and after escapes) here
    } literal;
} Token;

/*linked list to hold tokenized input*/
typedef struct tokenlist{
    Token* t;
    struct tokenlist* link;
} TokenList;

/*helper functions*/
TokenList* insert_head(TokenList* new_node, TokenList* head);
void print_list(TokenList** head);
void print_token(Token *t);
TokenList* free_list(TokenList* head);
void print_nodes(TokenList *new_node);
void found_error(int error);

/*external functions and variables*/
extern void yyerror(char *s);
extern int yylex();
extern char* yytext;
extern int yylineno;
extern FILE* yyin;


/*=== non tokens ===*/
#define ClassDeclaration        1000
#define Identifier              1001
#define ClassBody               1002
#define ClassBodyDeclaration    1003
#define FieldDeclaration        1004
#define Type                    1005
#define Types                   1006
#define ArraySubscript          1007
#define Name                    1008
#define QualifiedName           1009
#define Punctuation             1010
#define VarDeclarator           1011
#define MethodReturnVal         1012
#define MethodDecl              1013
#define MethodHeader            1014
#define MethodDeclarator        1015
#define FormalParmListOpt       1016
#define FormalParmList          1017
#define FormalParm              1018
#define ConstructorDecl         1019
#define ConstructorDeclarator   1020
#define ArgListOpt              1021
#define Block                   1022
#define BlockStmtsOpt           1023
#define BlockStmts              1024
#define BlockStmt               1025
#define LocalVarDeclStmt        1026
#define LocalVarDecl            1027
#define Stmt                    1028
#define StmtNoShortIf           1029         
#define StmtWOTrailingStmt      1030
#define ExprStmt                1031
#define StmtExpr                1032
#define IfThenStmt              1033
#define IfThenElseStmt          1034
#define IfThenElseStmtNoShortIf 1035
#define WhileStmtNoShortIf      1036
#define ForStmtNoShortIf        1037
#define WhileStmt               1038
#define ForStmt                 1039
#define ForInit                 1040
#define ExprOpt                 1041
#define ForUpdate               1042
#define StmtExprList            1043
#define BreakStmt               1044
#define Primary                 1045
#define InstantiationExpr       1046
#define ArgList                 1047
#define FieldAccess             1048
#define MethodCall              1049
#define PostFixExpr             1050
#define UnaryExpr               1051
#define MulExpr                 1052
#define AddExpr                 1053
#define RelOp                   1054
#define RelExpr                 1055
#define EqExpr                  1056
#define CondAndExpr             1057
#define CondOrExpr              1058
#define Expr                    1059
#define Assignment              1060
#define LeftHandSide            1061
#define AssignOp                1062
#define Literal                 1063
#define ArrLiteral              1064
#define NullArr                 1065
#define intLit                  1066
#define intArrLit               1067
#define intArrOpt               1068
#define longLit                 1069
#define longArrLit              1070
#define longArrOpt              1071
#define charLit                 1072
#define charArrLit              1073
#define charArrOpt              1074
#define floatLit                1075
#define floatArrLit             1076
#define floatArrOpt             1077
#define doubleLit               1078
#define doubleArrLit            1079
#define doubleArrOpt            1080
#define stringLit               1081
#define stringArrLit            1082
#define stringArrOpt            1083
#define booleanLit              1084
#define boolArrLit              1085
#define boolArrOpt              1086
#define SwitchStatement         1087
#define SwitchBody              1088
#define SwitchCase              1089
#define SwitchDefault           1090
#define VarDecls                1091
#define ArrVarDeclarator        1092
#define ReturnStmt              1093
#define ContinueStmt            1094
#define ClassBodyDeclarations   1095
#define String                  1096

#define packageDeclaration              1097
#define packageName                     1098
#define importDeclarations              1099
#define importDeclaration               1100
#define singleTypeImportDeclaration     1101
#define typeImportOnDemandDeclaration   1102
#define typeName                        1103
#define typeDeclarations                1104
#define typeDeclaration                 1105
#define compilationUnit                 1106
#define multiImportDeclaration          1107
#define SwitchCases                     1108
#define SwitchCaseBlock                 1109
#define CreateArray                     1110
#define InitializeArrayElement          1111
#define AssignArrayElement              1112
                 


#endif
