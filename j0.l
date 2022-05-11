/* Saugat Sharma & Joseph Coston
 * CSE 423 Homework 6
 */

%option noinput
%option nounput
%option noyywrap

%{
    #include "j0.l.c"

%}

whitespace      [ \t\r\v\f]+
multicomment	"/*"([^*]|"*"+[^/*])*"*"+"/" 
/* comment for syntax highlighting */
singlecomment	"//".*
newline         \r?\n

%%

{whitespace}    { ; }
{singlecomment} { ; }
{multicomment}  { for(int i=0;i<=strlen(yytext);i++){if(yytext[i]=='\n'){yylineno++;}}; }
{newline}	    { yylineno++; }
"#".*       { ; }
"/*"		{ yy_fatal_error("unterminated comment"); } /* comment for syntax highlighting */
package     { return tokenize(PACKAGE); }
import      { return tokenize(IMPORT); }    
boolean		{ return tokenize(BOOL); }
char		{ return tokenize(CHAR); }
String		{ return tokenize(STRING); }
double		{ return tokenize(DOUBLE); }
float		{ return tokenize(FLOAT); }
int			{ return tokenize(INT); }
long		{ return tokenize(LONG); }
break		{ return tokenize(BREAK); }
case		{ return tokenize(CASE); }
class		{ return tokenize(CLASS); }
continue 	{ return tokenize(CONTINUE); }
default		{ return tokenize(DEFAULT); }
if			{ return tokenize(IF); }
else		{ return tokenize(ELSE); }
for			{ return tokenize(FOR); }
new			{ return tokenize(NEW); }
return		{ return tokenize(RETURN); }
switch		{ return tokenize(SWITCH); }
public		{ return tokenize(PUBLIC); }
static		{ return tokenize(STATIC); }
void		{ return tokenize(VOID); }
while		{ return tokenize(WHILE); }
abstract|assert|byte|catch|const|do|enum|exports|extends|final|finally|goto|implements|interface|module|native|protected|requires|short|strictfp|super|synchronized|this|throw|throws|transient|try|var|volatile|private	{ yy_fatal_error("found Java reserved word not in Jzero:"); return NOTINJ0; }
"++"	{ return tokenize(INC); }
"--"	{ return tokenize(DEC); }
"%"		{ return tokenize('%'); }
"=="	{ return tokenize(EQ); }
"!="	{ return tokenize(NEQ); }
"<="	{ return tokenize(LEQ); }
">="	{ return tokenize(GEQ); }
"<"		{ return tokenize(LT); }
">" 	{ return tokenize(GT); }
"&&"	{ return tokenize(AND); }
"||"	{ return tokenize(OR); }
"!"		{ return tokenize('!'); }
"+="    { return tokenize(ADDASS); }
"-="    { return tokenize(SUBASS); }
"="		{ return tokenize('='); }
"+"		{ return tokenize('+'); }
"-"		{ return tokenize('-'); }
"*"		{ return tokenize('*'); }
"/"		{ return tokenize('/'); }
"["		{ return tokenize('['); }
"]"		{ return tokenize(']'); }
"("		{ return tokenize('('); /*)*/ }
")"		{ return tokenize(')'); }
"{"		{ return tokenize('{'); /*}*/ }
"}"		{ /*{*/  return tokenize('}'); }
":"		{ return tokenize(':'); }
";"		{ return tokenize(';'); }
","		{ return tokenize(','); }
"."		{ return tokenize('.'); }
"~"|"<<"|">>"|"&="|"|="|"^="|">>="|"<<="|"&"|"^"|"|"|"?"	{ yy_fatal_error("encountered Java operator not in Jzero"); return NOTINJ0; }
"NULL"                                  { return tokenize(NULLVAL); }
[0-9]+[l|L]						        { return tokenize(LONGLIT); }
[0-9]+								    { return tokenize(INTLIT); }
true|false								{ return tokenize(BOOLLIT); }
([0-9]*\.?[0-9]+|[0-9]+\.)[f|F]	        { return tokenize(FLOATLIT); }
([0-9]*\.?[0-9]+|[0-9]+\.)			    { return tokenize(DOUBLELIT); }
\'(([^'\\])|\\[\\nafr0\'])\'	        { return tokenize(CHARLIT); }
\"(\\.|[^\\"])*\"						{ return tokenize(STRINGLIT); }
[a-zA-Z_][a-zA-Z0-9_]*	                { return tokenize(ID); }
<<EOF>>     	                        { return tokenize(EOF); }
.										{ tokenError(); }
%%


void tokenError(){
    PRINTERR("LEXICAL ERROR IN %s ON LINE %d: Invalid Token %s\n", yyfilename, yylineno, yytext);
    exit(ERROR_LEXICAL);
}


/*
\"([^"\\]|\\.*)*\"						{ return tokenize(STRINGLIT); } // old
\"(\\.|[^\\"])*\"						{ return tokenize(STRINGLIT); } // new (fixed)
*/
