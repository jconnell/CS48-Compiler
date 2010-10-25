/* quads.c
 * Basic quad fucntions and code generation
 * Written by DK and JC for CS 48.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "quads.h"
#include "ast.h"
#include "Symtab.h"

//GLOBAL VARIABLES
int currentQuad = 0;		//index into quads
int tempCount = 0;			//for unique temp names
Quad *quads;				//array of Quads
SymbolTable *symtab;		//symbol table

int CG(ast_node n)
{
	
}

//makes a quad out of the actual parameters, adds it to the array of quads
//that we're using to store our total set of quads in memory
//returns the index of that added quad
int GenQuad(OpKind o, Address a, Address b, Address c)
{
	//make the quad
	Quad *q = malloc(sizeof(Quad));
	q->op = o;
	q->addr1 = a;
	q->addr2 = b;
	q->addr3 = c;
	
	//currentQuad points to the current index into the array of the last
	//added quad
	if (currentQuad != 0) {
		currentQuad++;
	}
	
	//add it to the array
	quads[currentQuad] = q;
	
	return currentQuad;
}

//Changes the address in position l(1,2, or 3) to n
//in the quad at index q
void PatchQuad(int q, int l, Address n)
{
	Quad *theQuad = quads[q];
	switch (l) {
		case 1:
			theQuad->addr1 = n;
			break;
		case 2:
			theQuad->addr2 = n;
			break;
		case 3:
			theQuad->addr3 = n;
			break;
		default:
			break;
	}
}

//returns the next quad after the last added one
int NextQuad()
{
	return currentQuad+1;
}

//adds a new temp to the symbol table and return its name
char *NewTemp()
{
	char tempName[5];
	sprintf(tempName, "$t%d", tempCount);  //$t#
	
	InsertIntoSymbolTable(symtab, tempName);
	
	tempCount++;			//so next one has unique name
	
	return tempName;
}

//main function
int main() 
{
	//generate the array of quads (okay we have a limit of 10,000 - we really don't think people
	//in C48 will get beyond this using our compiler - consider it a "compiler limit"
	quads = malloc(10000 * sizeof(Quad));
	
	//create symbol table
	SymbolTable *symtab = CreateSymbolTable();
	//now we call CG of some node....
}