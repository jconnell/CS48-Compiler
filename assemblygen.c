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
//int currentQuad = -1;		//index into quads
//int tempCount = 0;			//for unique temp names
Quad *quads;				//array of Quads
//SymbolTable *symtab;		//symbol table

void AssemblyGen(Quad* quads, FILE* file) {//, SymbolTable* symtab) {
	int i = 0;
	while (i == 0) {
		switch (quads->op) {
			case add:
				if (quads->addr2.kind == IntConst) {
					if (quads->addr3.kind == IntConst) {
						fprintf(file,"%s","Derp");
					}
					else if (quads->addr3.kind == DouConst) {
						continue;
					}
				}
				else if (quads->addr2.kind == DouConst) {
					if (quads->addr3.kind == IntConst) {
						continue;
					}
					else if (quads->addr3.kind == DouConst) {
						continue;
					}
				}
			default:
				printf("DERP");
				
				
				
				//case /*SUB*/:
				//case /*DIV*/:
				//case /*MUL*/:
				//default:
		}
		i++;
	}
}

/*int GenQuad(OpKind o, Address a, Address b, Address c)
{
	//make the quad
	Quad *q = malloc(sizeof(Quad));
	q->op = o;
	q->addr1 = a;
	q->addr2 = b;
	q->addr3 = c;
	
	//currentQuad points to the current index into the array of the last
	//added quad
	int currentQuad = -1;
	currentQuad++;
	
	//add it to the array
	quads[currentQuad] = q;
	
	return currentQuad;
}

char *NewTemp()
{
	printf("Called NewTemp\n");
	char *tempName;
	tempName = malloc(sizeof(char) * 7);
	sprintf(tempName, "t%d", tempCount);  //$t#
	
	printf("Inserting into Symbol Table\n");
	InsertIntoSymbolTable(symtab, tempName);
	
	tempCount++;			//so next one has unique name
	
	printf("Leaving NewTemp\n");
	return tempName;
}*/


int main() {
	quads = malloc(sizeof(Quad *));
	
	FILE *file;
	file = fopen("tm48code.txt","a+");
	
	//char* a1 = malloc(sizeof(char) * 100);
	//char* a2 = malloc(sizeof(char) * 100);
	//char* a3 = malloc(sizeof(char) * 100);
	
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
	
	quads->op = o;
	quads->addr1 = a;
	quads->addr2 = b;
	quads->addr3 = c;
	
	
	AssemblyGen(quads, file);
	fclose(file);
	
}

