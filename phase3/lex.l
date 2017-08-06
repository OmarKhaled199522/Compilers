%{
	#include "y.tab.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	int lineCounter = 1;
%}

letter  [a-zA-Z]
digit [0-9]
digits {digit}+
id {letter}({letter}|{digit})*
num "-"?{digits}
fnum "-"?{digits}.{digits}("E"{digits}+)?
relop "=="|"!="|">"|">="|"<"|"<="
boolop "&&"|"||"
op "+"|"-"|"*"|"/"|"&"|"|"
binary "true"|"false"

%%
[ \t]	;
\n    {lineCounter++;}
"System.out.println"	{return SYSTEM_OUT;}
"if"	{return IF_WORD;}
"else"	{return ELSE_WORD;}
"while"	{return WHILE_WORD;}
"for"	{return FOR_WORD;}
"int"	{return INT_WORD;}
"float"	{return FLOAT_WORD;}
"boolean" {return BOOLEAN_WORD;}
{op}	{yylval.aopValue = strdup(yytext); return ARITH_OP;}
{num}	{yylval.intValue = atoi(yytext); return INT;}
{fnum}	{yylval.floatValue = atof(yytext); return FLOAT;}
{relop} {yylval.aopValue = strdup(yytext); return RELA_OP;}
{boolop} {yylval.aopValue = strdup(yytext); return BOOL_OP;}
{binary} {if(!strcmp(yytext,"true")){ yylval.boolValue = 1;}
					else { yylval.boolValue = 0;} return BOOL;}
{id}	{yylval.idValue = strdup(yytext);return IDENTIFIER;}
[\(\)\{\};=]  { return yytext[0];}

%%


int yywrap() {
   return -1;
}
