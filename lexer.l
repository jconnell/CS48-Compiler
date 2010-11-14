%{
#include "util.h"
#include "scan.h"
#include "tokens.h"

char tokenString[MAXTOKENLEN +1]
	
	
digit 		[0-9]
number 		{digit}+
letter 		[a-zA-Z]
identifier 	{letter}({letter}|{digit})+
newline 	\n
whitespace	[ \t]+

%%

"if"		{return IF;}
"else"		{return ELSE;}
"read"		{return READ;}
"write"		{return WRITE;}
"do"		{return DO;}
"while"		{return WHILE;}
"for"		{return FOR;}
"int"		{return INT;}
"double"	{return DOUBLE;}
"switch"	{return SWITCH;}
"case"		{return CASE;}
"const"		{return CONST;}
"continue"	{return CONTINUE;}
"default"	{return DEFAULT;}
"return"	{return RETURN;}
"sizeof"	{return SIZEOF;}
"void"		{return VOID;}
"break"		{return BREAK;}
"static"	{return STATIC;}

"/*"		{return COMMENT;}
"=="		{return EQUALS;}
"!="		{return NOTEQUALS;}
">="		{return GREATEREQ;}
"<="		{return LESSEQ;}
"&&"		{return AND;}
"||"		{return OR;}
"++"		{return INCR;}
"--"		{return DECR;}

"="			{return '=';}
"+"			{return '+';}
"-"			{return '-';}
"*"			{return '*';}
"/"			{return '/';}
">"			{return '>';}
"<"			{return '<';}
"("			{return '(';}
")"			{return ')';}
"{"			{return '{';}
";"			{return ';';}

{number}		{return NUM;}
{identifier}		{return ID;}
	

%%

/*straight out of Louden*/
int getToken(void) {
	static int firstTime = 1;
	int currentToken;
	if (firstTime) {
		firstTime = 0;
		lineno++;
		yyin = source;
		yyout = listing;
	}
	currentToken = yylex();
	strncpy(tokenString, yytext, MAXTOKENLEN);
	if (TraceScan) {
		fprintf(listing, "\t%d: ", lineno);
		printf("%d  %d\n", currentToken, tokenString);
	}
	return currentToken;
}
