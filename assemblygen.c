/*
 *  assemblygen.c
 *  
 *
 *  Created by Jonathan Connell on 11/6/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "assemblygen.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "quads.h"
#include "ast.h"
#include "Symtab.h"

/*
 * Types:
 *
 *
 */
//GLOBAL VARIABLES

Quad **quads;				//array of Quads
SymbolTable *symtab;

void AssemblyGen(Quad** q, FILE* file, SymbolTable* s) {
	int i = 0;
	
	quads = q;
	symtab = s;
	
	OpKind op = quads[i]->op;
	Address a1 = quads[i]->addr1;
	Address a2 = quads[i]->addr2;
	Address a3 = quads[i]->addr3;
	
	SymNode *s1;
	SymNode *s2;
	SymNode *s3;
	
	while (i <= 20) {
		if (quads[i]->addr1.kind == String){
			s1 = LookupInSymbolTable(symtab, quads[i]->addr1.contents.name);
			printf(" %s", quads[i]->addr1.contents.name);
		}
		else if (quads[i]->addr1.kind == IntConst) {
			printf(" %d", quads[i]->addr1.contents.val);
		}
		else if (quads[i]->addr1.kind == DouConst) {
			printf(" %f", quads[i]->addr1.contents.dval);
		}
		else printf(" - ");
		
		if (quads[i]->addr2.kind == String){
			s2 = LookupInSymbolTable(symtab, quads[i]->addr2.contents.name);
			printf(" %s", quads[i]->addr2.contents.name);
			//printf(" %d\n", s2->level);
		}
		else if (quads[i]->addr2.kind == IntConst) {
			printf(" %d", quads[i]->addr2.contents.val);
		}
		else if (quads[i]->addr2.kind == DouConst) {
			printf(" %f", quads[i]->addr2.contents.dval);
		}
		else printf(" - ");
		
		if (quads[i]->addr3.kind == String){
			s3 = LookupInSymbolTable(symtab, quads[i]->addr3.contents.name);
			printf(" %s\n", quads[i]->addr3.contents.name);
		}
		else if (quads[i]->addr3.kind == IntConst) {
			printf(" %d\n", quads[i]->addr3.contents.val);
		}
		else if (quads[i]->addr3.kind == DouConst) {
			printf(" %f\n", quads[i]->addr3.contents.dval);
		}
		else printf(" - \n");
		
		AssemKind AK;
		char* AssemCommand = " ";
		
		if (quads[i]->op == add) {
			AK = math;
			AssemCommand = "ADD";
		}
		else if (quads[i]->op == sub) {
			AK = math;
			AssemCommand = "SUB";
		}
		else if (quads[i]->op == mul) {
			AK = math;
			AssemCommand = "MUL";
		}
		else if (quads[i]->op == divi) {
			AK = math;
			AssemCommand = "DIV";
		}
		else {
			AK = inequality;
			AssemCommand = "ERROR";
		}
		
		printf("%d %s %d \n", AK, AssemCommand, quads[i]->op);
		switch (AK) {
			case (math):
				if (quads[i]->addr2.kind == String) {
					printf("%d\n", s2->attrs->type);
					if (s2->attrs->type == IntT) {
						fprintf(file, "LD 0, %d(5)\n", s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "%s 0, 1, 0\n", AssemCommand);
							}
							else if (s3->attrs->type == DouT) {
								fprintf(file, "CVTIF 0, 0(0)\n");
								fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
							fprintf(file, "%s 0, 1, 0\n", AssemCommand);
						}
						else if (quads[i]->addr3.kind == DouConst) {
							fprintf(file, "CVTIF 0, 0(0)\n");
							fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (s2->attrs->type == DouT) {
						printf("I'm here.\n");
						fprintf(file, "LDF 0, %d(5)\n", s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "CVTIF 1, 0(1)\n");
								fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
							}
							else if (s3->attrs->type == DouT) {
								fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
							fprintf(file, "CVTIF 1, 0(1)\n");
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else if (quads[i]->addr3.kind == DouConst) {
							fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else {
						printf("I'm here.\n");
						fprintf(file, "ERROR\n");
					}
				}
				else if (quads[i]->addr2.kind == IntConst) {
					fprintf(file, "LDC 0, %d(0)\n", quads[i]->addr2.contents.val);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "%s 0, 1, 0\n", AssemCommand);
						}
						else if (s3->attrs->type == DouT) {
							fprintf(file, "CVTIF 0, 0(0)\n");
							fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
						fprintf(file, "%s 0, 1, 0\n", AssemCommand);
					}
					else if (quads[i]->addr3.kind == DouConst) {
						fprintf(file, "CVTIF 0, 0(0)\n");
						fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
						fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else if (quads[i]->addr2.kind == DouConst) {
					fprintf(file, "LDFC 0, %f(0)\n", quads[i]->addr2.contents.dval);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "CVTIF 1, 0(1)\n");
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else if (s3->attrs->type == DouT) {
							fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
						fprintf(file, "CVTIF 1, 0(1)\n");
						fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
					}
					else if (quads[i]->addr3.kind == DouConst) {
						fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
						fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else {
					fprintf(file, "ERROR\n");
				}
				break;
			default:
				printf("DERP\n");
				break;
				
		}
		i++;
	}
}

/*
int main() {
	quads = malloc(sizeof(Quad *));
	symtab = CreateSymbolTable();
	InsertIntoSymbolTable(symtab, "dog");
	
	FILE *file;
	file = fopen("tm48code.txt","a+");
	
	Address a;
	Address b;
	Address c;
	OpKind o;
	a.kind = IntConst;
	b.kind = IntConst;
	c.kind = IntConst;
	
	
	a.contents.val = 2;
	b.contents.val = 3;
	c.contents.val = 4;
	o = add;
	
	quads[i]->op = o;
	quads[i]->addr1 = a;
	quads[i]->addr2 = b;
	quads[i]->addr3 = c;
	
	
	AssemblyGen(quads, file, symtab);
	fclose(file);
	
}
*/
