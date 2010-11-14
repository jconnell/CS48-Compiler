/*
 * JONATHAN CONNELL & DAVID KOPEC
 * CS 48
 * PROJECT PART 4
 *
 */


#include <stdio.h>

extern int yylex(void); extern char *yytext;
extern int yyleng;
extern FILE *yyin;

int main(int argc, char *argv[])
{
    int n;

    yyin = fopen("in", "r");

	while(n = yylex()) {
	   printf("wtf: %s (%d chars)\n", yytext, yyleng);
    }

    fclose(yyin);

    return 0;
}

