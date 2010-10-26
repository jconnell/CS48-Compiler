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

//Actually creates the quads by recursing;
//We will return the quad that was the last result or -1 if we have no result
int CG(ast_node n)
{
	switch (n->node_type) {
			
		
		//Dave adds new cases BELOW here, Jon ABOVE
			
		//if it's a SEQ, we want to just recursively produce code for all the children
		case SEQ:
			//adapted from THC if code
			ast_node x = n->left_child;
			while (x != null) {
				CG(x);
				x = x->right_sibling;
			}
			break;
		
		//ROOT is nothing in itself, so we just start recursing down the tree
		case ROOT:
			CG(n->left_child);
			break;
		
		//Here we do the addition and then we return the position so higher up nodes can find the result
		case OP_PLUS:
			Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp();
			
			//left result needs to be put in
			int lrp = CG(n->left_child);
			ar2 = quads[lrp].addr1;
			
			//right child's result needs to be the other operand
			int rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp].addr1;
			
			return GenQuad(add, ar1, ar2, ar3);
			break;
			
		case OP_MINUS:
			Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp();
			
			//left result needs to be put in
			int lrp = CG(n->left_child);
			ar2 = quads[lrp].addr1;
			
			//right child's result needs to be the other operand
			int rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp].addr1;
			
			return GenQuad(sub, ar1, ar2, ar3);
			break;
			
		case OP_TIMES:
			Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp();
			
			//left result needs to be put in
			int lrp = CG(n->left_child);
			ar2 = quads[lrp].addr1;
			
			//right child's result needs to be the other operand
			int rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp].addr1;
			
			return GenQuad(mul, ar1, ar2, ar3);
			break;
			
		case OP_DIVIDE:
			Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp();
			
			//left result needs to be put in
			int lrp = CG(n->left_child);
			ar2 = quads[lrp].addr1;
			
			//right child's result needs to be the other operand
			int rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp].addr1;
			
			return GenQuad(div, ar1, ar2, ar3);
			break;

		//negate a number
		case OP_NEGATIVE:
			Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp();
			
			//this should only have one child
			int lrp = CG(n->left_child);
			ar2 = quads[lrp].addr1;
			
			//we need to subtract from 0
			ar3.kind = IntConst;
			ar3.contents.val = 0;
			
			return GenQuad(sub, ar1, ar3, ar2);
			break;

		case INT_LITERAL:
			Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp();
			
			//we are assigning this literal value to ar1
			ar2.kind = IntConst;
			ar2.contents.val = n->value.int_value;
			
			//we don't need ar3
			ar3.kind = Empty;
			
			return GenQuad(asn, ar1, ar2, ar3);
			break;
			
		case DOUBLE_LITERAL:
			Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp();
			
			//we are assigning this literal value to ar1
			ar2.kind = DouConst;
			ar2.contents.dval = n->value.double_value;
			
			//we don't need ar3
			ar3.kind = Empty;
			
			return GenQuad(asn, ar1, ar2, ar3);
			break;
			
		case STRING_LITERAL:
			Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp();
			
			//we are assigning this literal value to ar1
			ar2.kind = String;
			ar2.contents.name = n->value.string;
			
			//we don't need ar3
			ar3.kind = Empty;
			
			return GenQuad(asn, ar1, ar2, ar3);
			break;
			
		//we must insert in the symbol table the child's name
		case INT_DEC:
			InsertIntoSymbolTable(symtab, n->left_child->value.string);
			//PLACEHOLDER - Need to set the TYPE ATTRIBUTE in the symbol table to int
			break;
		
		//we must insert the child in the symbol table
		case DOU_DEC:
			InsertIntoSymbolTable(symtab, n->left_child->value.string);
			//PLACEHOLDER - Need to set the TYPE ATTRIBUTE in the symbol table to double
			break;
			
		case INT_ARRAY_DEC:
			InsertIntoSymbolTable(symtab, n->left_child->value.string);
			//PLACEHOLDER - Need to set the TYPE ATTRIBUTE in the symbol table to int array
			//WE NEED TO FIND A WAY TO STORE THE ARRAY DATA - HOW ARE WE DOING THAT???
			//A POINTER IN THE SYMBOL TABLE TO A HEAP STORE????
			break;
			
		case DOU_ARRAY_DEC:
			InsertIntoSymbolTable(symtab, n->left_child->value.string);
			//PLACEHOLDER - Need to set the TYPE ATTRIBUTE in the symbol table to double array
			//WE NEED TO FIND A WAY TO STORE THE ARRAY DATA - HOW ARE WE DOING THAT???
			//A POINTER IN THE SYMBOL TABLE TO A HEAP STORE????
			break;
			
		case DOU_ARRAY_DEC:
			InsertIntoSymbolTable(symtab, n->left_child->value.string);
			//PLACEHOLDER - Need to set the TYPE ATTRIBUTE in the symbol table to double array
			//WE NEED TO FIND A WAY TO STORE THE ARRAY DATA - HOW ARE WE DOING THAT???
			//A POINTER IN THE SYMBOL TABLE TO A HEAP STORE????
			break;

		//adapted from THC's code in class
		//MUST TYPE CHECK THAT t RETURNS AN INT FOR COMPARISON
		case WHILE_LOOP:
			int tq = NextQuad();
			int t = CG(n->left_child);
			
			Address tqa;
			tqa.kind = IntConst;
			tqa.contents.val = tq;
			
			Address ta = quads[t].addr1;
			
			Address topatch;
			topatch.kind = Empty;
			
			Address e;
			e.kind = Empty;
			
			//this is where we would switch the order for the dowhile
			int testq = GenQuad(if_f, ta, topatch, e);
			
			CG(n->left_child->right_sibling);
			
			GenQuad(gt, tqa, e, e);
			
			Address nq;
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(testq, 2, nq);
			break;
	
		//we're still assuming the comparison is the left child
		//not the left child->right sibling
		case DO_WHILE_LOOP:
			int tq = NextQuad();
			int t = CG(n->left_child);
			
			//just do the other stuff first and then we potentially loop or leave
			CG(n->left_child->right_sibling);
			
			Address tqa;
			tqa.kind = IntConst;
			tqa.contents.val = tq;
			
			Address ta = quads[t].addr1;
			
			Address topatch;
			topatch.kind = Empty;
			
			Address e;
			e.kind = Empty;
			
			//this is where we would switch the order for the dowhile
			int testq = GenQuad(if_f, ta, topatch, e);
			
			GenQuad(gt, tqa, e, e);
			
			Address nq;
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(testq, 2, nq);
			break;
			
		//the assembler has a built in read facility, so this is very simple
		//NEED TO DEAL WITH DOUBLES HERE TOO BASED ON TYPE CHECK of CHILD's ATTRIBUTE
		case READ:
			Address ar1, ar2, ar3;
			ar1.kind = IntConst;
			
			ar2.kind = Empty;
			
			ar3.kind = Empty;
			
			int gq = GenQuad(rd, ar1, ar2, ar3);
			
			//PUT VALUE IN SYMBOL TABLE UNDER READ's CHILD
			
			return gq;
			
			break;
			
		//the assembler has a built in write facility, so this is very simple
		//NEED TO HANDLE DOUBLES HERE TOO BASED ON TYPE CHECK of CHILD's TYPE ATTRIBUTE
		case WRITE:
			Address ar1, ar2, ar3;
			ar1.kind = IntConst;
			ar1.contents.val = n->left_child->value.int_value;
			
			ar2.kind = Empty;
			
			ar3.kind = Empty;
			
			GenQuad(wri, ar1, ar2, ar3);
			
			break;
			
		//MISSING CONST, RETURN, SWITCH, BREAK, CONTINUE
		//RETURN NEEDS TO GO TOGETHER WITH FUNCTIONS I THINK...  MAYBE WE NEED TO TYPE CHECK WITH ANOTHER
			//SWITCH WITHIN THE CASE OF A FUNCTION CALL

		default:
			break;
	}
	
	//default, we return -1 which means no result from the previous call...
	return -1;
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
	Quad *theQuad = &quads[q];
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
	
	//print all of our quads for debug purposes
	/*
	int i = 0;
	while(quads[i] != null)
	{
		printf("...");
		i++;
	}
	*/
}