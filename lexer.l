

#include "tokens.h"

digit 		[0-9]
number 		{digit}+
letter 		[a-zA-Z]
identifier 	{letter}+
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
"char"		{return CHAR;}
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

"="		{return '=';}
"+"		{return '+';}
"-"		{return '-';}
"*"		{return '*';}
"/"		{return '/';}
">"		{return '>';}
"<"		{return '<';}
"("		{return '(';}
")"		{return ')';}
"{"		{return '{';}
";"		{return ';';}
	

%%


