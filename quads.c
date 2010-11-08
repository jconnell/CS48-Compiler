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
#include "assemblygen.h"

#define DOASSEMBLY 1

//GLOBAL VARIABLES
int currentQuad = -1;		//index into quads
int tempCount = 0;			//for unique temp names

int goffset = 0;			//global offset for dmem addresses
int foffset = -4;			//function offset for dmem addresses
//must remember in function declaration to change foffset back to -4

Quad **quads;				//array of Quads
SymbolTable *symtab;		//symbol table

char namesOfOps[][10] = {"rd", "gotoq", "if_f", "asn", "lab", "mul", "divi", "add", "sub", "eq", "wri", "halt", "neq",
"lt", "gt", "gteq", "lteq", "sym", "ret", "ens", "exs"}; 

//Get the wider of the two - int or double
int MaxType(Address a, Address b)
{
	printf("Called MaxType\n");
	SymNode *an;
	SymNode *bn;
	if (a.kind == String) 
	{
		printf("a.kind is string\n");
		an = LookupInSymbolTable(symtab, a.contents.name);
		printf("looked up a\n");
		typeenum te = GetTypeAttr(an);
		if (te == DouT)
			return 8;
	}
	
	if (b.kind == String) 
	{
		printf("b.kind is string\n");
		bn = LookupInSymbolTable(symtab, b.contents.name);
		typeenum te = GetTypeAttr(bn);
		if (te == DouT)
			return 8;
	}
	
	//set the result to the right type
	if (a.kind == DouConst || b.kind == DouConst) 
	{
		return 8;
	}
	
	
	printf("Finished MaxType\n");
	
	return 4;
}

//Actually creates the quads by recursing;
//We will return the quad that was the last result or -1 if we have no result
int CG(ast_node n)
{
	printf("Called CG\n");
	
	Address ar1, ar2, ar3, ar4, ar5, tqa, ta, topatch, e, nq;
	int lrp, rrp;
	int tq, t, testq, gq;
	int typer;
	SymNode *sn;
	value v;
	
	ast_node x;
	
	if (n == NULL)
	{
		printf("Error CG called on NULL node\n");
		
		return -1;
	}
	
	switch (n->node_type) {
			
		//we have a function with parameters
		case FUNC_DEF:
			printf("In FUNC_DEF Case\n");
			//add the function name to the symbol table
			//
			
			e.kind = Empty;
			
			//we must reset f-offset for memory management purposes
			foffset = -4;
			
			//this will generate a sym quad which is the start of the function
			//it's also a dummy quad for integer defintions
			//we're using it like ENT in this case in Louden
			lrp = CG(n->left_child);
			//we expect an ID, so add it to the symbol table
			sn = InsertIntoSymbolTable(symtab, quads[lrp]->addr1.contents.name);
			//this is the quad where we'll need to jump if this function is called
			v.ival = lrp;
			SetValueAttr(sn, v);
			//we're going to ignore the parameters as per louden p 441
			ast_node next = n->left_child->right_sibling;
			while (next->node_type == FUNC_PARAM) 
			{
				next=next->right_sibling;
			}
			//now we're done so we should be at the SEQ of actual instructions
			CG(next);
			//finish it off with a ret
			GenQuad(ret, e, e, e);
			
			break;
		
		case RETURN:
			printf("In RETURN Case\n");
			gq = GenQuad(ret, e, e, e);
			
			//if we have a return after the statement
			if(n->left_child != NULL)
			{
				lrp = CG(n->left_child);
				PatchQuad(gq, 1, quads[lrp]->addr1);
			}
			
			break;
		
		// "==" (IS EQUAL TO) operation
		case OP_EQUALS:
			printf("In OP_EQUALS Case\n");
			//Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			//left result needs to be put in
			if (n->left_child == NULL) {
				printf("Error child is null\n");
			}
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			printf("%s the value\n", ar2.contents.name);
			
			//right child's result needs to be the other operand
			if (n->left_child->right_sibling == NULL) {
				printf("Error left_child->right sibling is null\n");
			}
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			printf("%s the value\n", ar3.contents.name);
			
			return GenQuad(eq, ar1, ar2, ar3);
			break;
			
			
		//ROOT is nothing in itself, so we just start recursing down the tree
		case ROOT:
			printf("In ROOT case\n");
			CG(n->left_child);
			break;

			
		// "!=" (IS NOT EQUAL TO) operation
		case OP_NOT_EQUALS:
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			
			return GenQuad(neq, ar1, ar2, ar3);
			break;
			
		// ">" (GREATER THAN) operation
		case OP_GREATER_THAN:
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			
			return GenQuad(gt, ar1, ar2, ar3);
			break;
		
		// "<" (LESS THAN) operation
		case OP_LESS_THAN:
			printf("LESS THAN Case in CG\n");
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			
			return GenQuad(lt, ar1, ar2, ar3);
			break;
			
		// ">=" (GREATER THAN OR EQUAL TO) operation
		case OP_GREATER_EQUALS:
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			
			return GenQuad(gteq, ar1, ar2, ar3);
			break;
			
		// "<=" (LESS THAN OR EQUAL TO) operation
		case OP_LESS_EQUALS:
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			
			return GenQuad(lteq, ar1, ar2, ar3);
			break;
		

		// "||" (OR) operation
		case OP_OR:
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			ar2.kind = IntConst;
			ar2.contents.val = 1;
			
			ar3.kind = IntConst;
			ar3.contents.val = 0;
			
			ar4.kind = String;
			ar4.contents.name = NewTemp(4);
			
			e.kind = Empty;
			
			lrp = CG(n->left_child);
			ta = quads[lrp]->addr1;
			
			gq = GenQuad(if_f, ta, e, e);
			
			GenQuad(asn, ar4, ar2, e);
			typer = GenQuad(gotoq, e, e, e);
			
			t = CG(n->left_child->right_sibling);
			ar5.kind = IntConst;
			ar5.contents.val = t;
			
			ta = quads[t]->addr1;
			
			testq = GenQuad(if_f, ta, e, e);
			
			GenQuad(asn, ar4, ar2, e);
			
			tq = GenQuad(gotoq, e, e, e);
			
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			
			PatchQuad(testq, 2, nq);
			
			
			GenQuad(asn, ar4, ar3, e);
			PatchQuad(typer, 1, nq);
			PatchQuad(gq, 2, ar5);
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(tq, 1, nq);
			
			rrp = GenQuad(asn, ar1, ar4, e);
			
			
			return rrp;
			
			break;
		
		// "&&" (AND) operation - based on THC code
		case OP_AND:
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			ar2.kind = IntConst;
			ar2.contents.val = 1;
			
			ar3.kind = IntConst;
			ar3.contents.val = 0;
			
			ar4.kind = String;
			ar4.contents.name = NewTemp(4);
			
			e.kind = Empty;
			
			lrp = CG(n->left_child);
			ta = quads[lrp]->addr1;
			
			gq = GenQuad(if_f, ta, e, e);
			
			t = CG(n->left_child->right_sibling);
			
			ta = quads[t]->addr1;
			
			testq = GenQuad(if_f, ta, e, e);
			
			GenQuad(asn, ar4, ar2, e);
			
			tq = GenQuad(gotoq, e, e, e);
			
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(gq, 2, nq);
			PatchQuad(testq, 2, nq);
			
			GenQuad(asn, ar4, ar3, e);
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(tq, 1, nq);
			rrp = GenQuad(asn, ar1, ar4, e);
			
			
			
			return rrp;
			break;		 
			
		case OP_NOT:
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			ar2.kind = IntConst;
			ar2.contents.val = 1;
			
			ar3.kind = IntConst;
			ar3.contents.val = 0;
			
			ar4.kind = String;
			ar4.contents.name = NewTemp(4);
			
			e.kind = Empty;
			
			t = CG(n->left_child);
			
			ta = quads[t]->addr1;
			
			//if false go to the end and give it a 0
			gq = GenQuad(if_f, ta, e, e);
			GenQuad(asn, ar4, ar2, e);
			
			tq = GenQuad(gotoq, e, e, e);
			
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(gq, 2, nq);
			GenQuad(asn, ar4, ar3, e);
			
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			
			PatchQuad(tq, 1, nq);
			rrp = GenQuad(asn, ar1, ar4, e);
			
			return rrp;
			break;		 
			
			 


			
		//if it's a SEQ, we want to just recursively produce code for all the children
		case SEQ:
			//adapted from THC if code
			e.kind = Empty;
			EnterScope(symtab);
			//we must tell the assembly generator we've entered a new scope
			GenQuad(ens, e, e, e);
			x = n->left_child;
			while (x != NULL) {
				CG(x);
				x = x->right_sibling;
			}
			GenQuad(exs, e, e, e);
			//we must tell the assembly generator we've exited a scope
			LeaveScope(symtab);
			break;

		
		//Here we do the addition and then we return the position so higher up nodes can find the result
		case OP_PLUS:
			printf("OP_PLUS Case in CG\n");
			ar1.kind = String;
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			
			ar1.contents.name = NewTemp(MaxType(ar2, ar3));
			
			return GenQuad(add, ar1, ar2, ar3);
			break;
			
		case OP_MINUS:
			ar1.kind = String;
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			
			ar1.contents.name = NewTemp(MaxType(ar2, ar3));
			
			return GenQuad(sub, ar1, ar2, ar3);
			break;
			
		case OP_TIMES:
			printf("OP_TIMES Case in CG\n");
			ar1.kind = String;
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			
			ar1.contents.name = NewTemp(MaxType(ar2, ar3));
			
			return GenQuad(mul, ar1, ar2, ar3);
			break;
			
		case OP_DIVIDE:
			ar1.kind = String;
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar3 = quads[rrp]->addr1;
			
			ar1.contents.name = NewTemp(MaxType(ar2, ar3));
			
			return GenQuad(divi, ar1, ar2, ar3);
			break;

		//negate a number
		case OP_NEGATIVE:
			ar1.kind = String;
			
			//this should only have one child
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//we need to subtract from 0
			ar3.kind = IntConst;
			ar3.contents.val = 0;
			
			ar1.contents.name = NewTemp(MaxType(ar2, ar3));
			
			return GenQuad(sub, ar1, ar3, ar2);
			break;
		
		case INT_LITERAL:
			printf("INT_LITERAL Case in CG\n");
			//Address ar1, ar2, ar3;
			ar1.kind = String;
			ar1.contents.name = NewTemp(4);
			
			//we are assigning this literal value to ar1
			ar2.kind = IntConst;
			printf("n->value.int_value %d \n", n->value.int_value);
			ar2.contents.val = n->value.int_value;
			
			//we don't need ar3
			ar3.kind = Empty;
			
			return GenQuad(asn, ar1, ar2, ar3);
			break;
		
		
		case DOUBLE_LITERAL:
			printf("DOUBLE_LITERAL Case in CG\n");
			ar1.kind = String;
			ar1.contents.name = NewTemp(8);
			
			//we are assigning this literal value to ar1
			ar2.kind = DouConst;
			ar2.contents.dval = n->value.double_value;
			
			//we don't need ar3
			ar3.kind = Empty;
			
			return GenQuad(asn, ar1, ar2, ar3);
			break;
			
		case STRING_LITERAL:
			ar1.kind = String;
			ar1.contents.name = NewTemp(240);
			
			//we are assigning this literal value to ar1
			ar2.kind = String;
			ar2.contents.name = n->value.string;
			
			//we don't need ar3
			ar3.kind = Empty;
			
			return GenQuad(asn, ar1, ar2, ar3);
			break;
			
			
		
		//we must insert in the symbol table the child's name
		case INT_DEC:
			printf("INT_DEC Case in CG\n");
			lrp = CG(n->left_child);
			ar1 = quads[lrp]->addr1;
			
			ar2.kind = Empty;
			
			ar3.kind = Empty;
			
			//going to insert into symbol table
			printf("inserting int dec id into sym table \n");
			printf("%s is the symbol going in\n", ar1.contents.name);
			sn = InsertIntoSymbolTable(symtab, ar1.contents.name);
			printf("Setting type attribute for that\n");
			SetTypeAttr(sn, IntT);
			printf("Set type successfully\n");
			
			if (sn->level == 1) {
				SetOffsetAttr(sn, goffset-4);
				goffset -= 4;
			}
			else {
				SetOffsetAttr(sn, foffset-4);
				foffset -= 4;
			}
			//PLACEHOLDER - Need to set the TYPE ATTRIBUTE in the symbol table to int
			
			break;
		
		//we must insert the child in the symbol table
		case DOU_DEC:
			printf("DOU_DEC Case in CG\n");
			lrp = CG(n->left_child);
			ar1 = quads[lrp]->addr1;
			
			ar2.kind = Empty;
			
			ar3.kind = Empty;
			
			sn = InsertIntoSymbolTable(symtab, ar1.contents.name);
			SetTypeAttr(sn, DouT);
			
			if (sn->level == 1) {
				SetOffsetAttr(sn, goffset-4);
				goffset -= 4;
			}
			else {
				SetOffsetAttr(sn, foffset-4);
				foffset -= 4;
			}
			//PLACEHOLDER - Need to set the TYPE ATTRIBUTE in the symbol table to double
			break;
		
		/*	
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
		*/	

		case FOR_LOOP:
			printf("FORLOOP Case in CG\n");
			
			//the first statement is always executed
			CG(n->left_child);
			
			//the inequality is statement two
			t = CG(n->left_child->right_sibling);
			ta = quads[t]->addr1;
			testq = GenQuad(if_f, ta, topatch, e);
			
			//we do the sequence before the final statement
			CG(n->left_child->right_sibling->right_sibling->right_sibling);
			
			//now we do the third statement in the for-loop header
			CG(n->left_child->right_sibling->right_sibling);
			
			//then we loop around
			e.kind = Empty;
			
			nq.kind = IntConst;
			nq.contents.val = t;
			GenQuad(gotoq, nq, e, e);
			
			//finally we must patch the inequality		
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(testq, 2, nq);
			
			break;
		
		case OP_PRE_INCR:
			printf("PRE++ Case in CG\n");
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			ar3.kind = IntConst;
			ar3.contents.val = 1;
			
			return GenQuad(add, ar2, ar2, ar3);
			break;
		
		case OP_POST_INCR:
			printf("POST++ Case in CG\n");
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			ar3.kind = IntConst;
			ar3.contents.val = 1;
			
			return GenQuad(add, ar2, ar2, ar3);
			break;
		
		case OP_PRE_DECR:
			printf("PRE-- Case in CG\n");
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			ar3.kind = IntConst;
			ar3.contents.val = 1;
			
			return GenQuad(sub, ar2, ar2, ar3);
			break;
			
		case OP_POST_DECR:
			printf("POST-- Case in CG\n");
			
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar2 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			ar3.kind = IntConst;
			ar3.contents.val = 1;
			
			return GenQuad(sub, ar2, ar2, ar3);
			break;
		
		case IF_ELSE:
			printf("IF_ELSE Case in CG\n");
			t = CG(n->left_child);
			
			ta = quads[t]->addr1;
			
			topatch.kind = Empty;
			
			e.kind = Empty;
			
			//this is where we would switch the order for the dowhile
			testq = GenQuad(if_f, ta, topatch, e);
			
			CG(n->left_child->right_sibling);
			gq = GenQuad(gotoq, e, e, e);
			
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(testq, 2, nq);
					
			CG(n->left_child->right_sibling->right_sibling);
					
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(gq, 1, nq);
			
			break;
		
		//adapted from THC's code in class
		//MUST TYPE CHECK THAT t RETURNS AN INT FOR COMPARISON
		case WHILE_LOOP:
			printf("WHILE Case in CG\n");
			tq = NextQuad();
			t = CG(n->left_child);
			
			tqa.kind = IntConst;
			tqa.contents.val = tq;
			
			ta = quads[t]->addr1;
			
			topatch.kind = Empty;
			
			e.kind = Empty;
			
			//this is where we would switch the order for the dowhile
			testq = GenQuad(if_f, ta, topatch, e);
			
			CG(n->left_child->right_sibling);
			
			GenQuad(gotoq, tqa, e, e);
			
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(testq, 2, nq);
			break;
	
		//we're still assuming the comparison is the left child
		//not the left child->right sibling
		case DO_WHILE_LOOP:
			tq = NextQuad();
			t = CG(n->left_child);
			
			//just do the other stuff first and then we potentially loop or leave
			CG(n->left_child->right_sibling);
			
			tqa.kind = IntConst;
			tqa.contents.val = tq;
			
			ta = quads[t]->addr1;
			
			topatch.kind = Empty;
			
			e.kind = Empty;
			
			//this is where we would switch the order for the dowhile
			testq = GenQuad(if_f, ta, topatch, e);
			
			GenQuad(gotoq, tqa, e, e);
			
			nq.kind = IntConst;
			nq.contents.val = NextQuad();
			PatchQuad(testq, 2, nq);
			break;
			
		//the assembler has a built in read facility, so this is very simple
		//NEED TO DEAL WITH DOUBLES HERE TOO BASED ON TYPE CHECK of CHILD's ATTRIBUTE
		case READ:
			printf("READ Case in CG\n");
			lrp = CG(n->left_child);
			printf("read back in for real\n");
			ar1 = quads[lrp]->addr1;
			
			printf("read back in\n");
			
			ar2.kind = Empty;
			
			ar3.kind = Empty;
			
			gq = GenQuad(rd, ar1, ar2, ar3);
			
			//PUT VALUE IN SYMBOL TABLE UNDER READ's CHILD
			
			return gq;
			
			break;
			
		//the assembler has a built in write facility, so this is very simple
		//NEED TO HANDLE DOUBLES HERE TOO BASED ON TYPE CHECK of CHILD's TYPE ATTRIBUTE
		case WRITE:
			printf("WRITE Case in CG\n");
			lrp = CG(n->left_child);
			ar1 = quads[lrp]->addr1;
			
			ar2.kind = Empty;
			
			ar3.kind = Empty;
			
			GenQuad(wri, ar1, ar2, ar3);
			
			break;
			
		//we put in a dummy just so that higher ups can use it, also put in symbol table
		case ID:
			printf("ID Case in CG\n");
			
			ar1.contents.name = strdup(n->value.string);
			
			printf("%s is the id\n", ar1.contents.name);
			
			printf("tried strdup\n");
			//if (ar1.contents.name == NULL) {
		//		printf("NULL STRING?!\n");
		//	}
			
			printf("%s is the id\n", ar1.contents.name);
			
			ar1.kind = String;
			//ar1.contents.name = n->value.string;
			//printf("%s is the id", n->value.string);
			
			ar2.kind = Empty;
			
			ar3.kind = Empty;
			
			
			
			return GenQuad(sym, ar1, ar2, ar3);
			break;
			
			//we put in a dummy just so that higher ups can use it, also put in symbol table
		case OP_ASSIGN:
			//left result needs to be put in
			lrp = CG(n->left_child);
			ar1 = quads[lrp]->addr1;
			
			//right child's result needs to be the other operand
			rrp = CG(n->left_child->right_sibling);
			ar2 = quads[rrp]->addr1;
			
			ar3.kind = Empty;
			
			return GenQuad(asn, ar1, ar2, ar3);
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
	currentQuad++;
	
	//add it to the array
	quads[currentQuad] = q;
	
	
	//DEBUG
	char* a1 = malloc(sizeof(char) * 100);
	char* a2 = malloc(sizeof(char) * 100);
	char* a3 = malloc(sizeof(char) * 100);
	switch (q->addr1.kind) 
	{
		case Empty:
			a1 = " - ";
			break;
		case IntConst:
			sprintf(a1,"%d",q->addr1.contents.val);
			break;
		case DouConst:
			sprintf(a1,"%f",q->addr1.contents.dval);
			break;
		case String:
			//printf("%s", quads[i]->addr1.contents.name);
			a1 = q->addr1.contents.name;
			//printf("%s", a1);
			break;
		default:
			break;
	}
	
	switch (q->addr2.kind) 
	{
		case Empty:
			a2 = " - ";
			break;
		case IntConst:
			sprintf(a2,"%d",q->addr2.contents.val);
			break;
		case DouConst:
			sprintf(a2,"%f",q->addr2.contents.dval);
			break;
		case String:
			//printf("%s", quads[i]->addr2.contents.name);
			a2 = q->addr2.contents.name;
			break;
		default:
			break;
	}
	
	switch (q->addr3.kind) 
	{
		case Empty:
			a3 = " - ";
			break;
		case IntConst:
			sprintf(a3,"%d",q->addr3.contents.val);
			break;
		case DouConst:
			sprintf(a3,"%f",q->addr3.contents.dval);
			break;
		case String:
			//printf("%s", quads[i]->addr3.contents.name);
			a3 = q->addr3.contents.name;
			break;
		default:
			break;
	}
	
	//printf("Finished switches, printing quad details\n");
	//printf("%s",a1);
	//printf("%s",a2);
	//printf("%s",a3);
	printf("(%s,%s,%s,%s)\n",namesOfOps[q->op],a1,a2,a3);

	//END DEBUG
	
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
char *NewTemp(int siz)
{
	//printf("Called NewTemp\n");
	char *tempName;
	tempName = malloc(sizeof(char) * 7);
	sprintf(tempName, "$t%d", tempCount);  //$t#
	
	//printf("Inserting into Symbol Table\n");
	SymNode *thesn = InsertIntoSymbolTable(symtab, tempName);
	
	if (thesn->level == 1) {
		SetOffsetAttr(thesn, goffset-siz);
		goffset -= siz;
	}
	else {
		SetOffsetAttr(thesn, foffset-siz);
		foffset -= siz;
	}

	if (siz == 8) {
		SetTypeAttr(thesn, DouT);
	}
	else if (siz == 240)
	{
		SetTypeAttr(thesn, StrT);
	}
	else {
		SetTypeAttr(thesn, IntT);
	}

	
	tempCount++;			//so next one has unique name
	
	//printf("Leaving NewTemp\n");
	return tempName;
}

//main function - call with the name of the AST input file
int main(int argc, char **argv) 
{
	//generate the array of quads (okay we have a limit of 10,000 - we really don't think people
	//in C48 will get beyond this using our compiler - consider it a "compiler limit"
	quads = calloc(10000, sizeof(Quad *));
	
	//CODE FROM THC's ast.c
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return -1;
	}
	
	ast_node root;
	
	printf("Building AST\n");
	
	root = build_ast(argv[1]); /* build an abstract syntax tree */
	
	printf("Printing AST\n");
	
	print_ast(root, 0);		      /* and print it out */
	
	printf("Creating Symbol Table\n");
	
	//OUR CODE AGAIN
	//create symbol table
	symtab = CreateSymbolTable();
	//now we call CG of the root node
	
	InsertIntoSymbolTable(symtab, "dog");
	
	printf("Calling CG on root node\n");
	
	CG(root);
	
	//print all of our quads for debug purposes
	int i = 0;
	//char a1[5];
	//char a2[5];
	//char a3[5];
	
	
	
	//SEGFAULT HAPPENING IN FOLLOWING CODE
	
	printf("Entering Debug Printing While Loop - we have %d quads and they are:\n", currentQuad);
	
	while(quads[i] != NULL)
	{
		char* a1 = malloc(sizeof(char) * 15);
		char* a2 = malloc(sizeof(char) * 15);
		char* a3 = malloc(sizeof(char) * 15);
		printf("Printing quad %d", i);
		//printf("entered while\n");
		switch (quads[i]->addr1.kind) 
		{
			case Empty:
				a1 = " - ";
				break;
			case IntConst:
				sprintf(a1,"%d",quads[i]->addr1.contents.val);
				break;
			case DouConst:
				sprintf(a1,"%f",quads[i]->addr1.contents.dval);
				break;
			case String:
				//printf("%s", quads[i]->addr1.contents.name);
				a1 = quads[i]->addr1.contents.name;
				//printf("%s", a1);
				break;
			default:
				break;
		}
		
		switch (quads[i]->addr2.kind) 
		{
			case Empty:
				a2 = " - ";
				break;
			case IntConst:
				sprintf(a2,"%d",quads[i]->addr2.contents.val);
				break;
			case DouConst:
				sprintf(a2,"%f",quads[i]->addr2.contents.dval);
				break;
			case String:
				//printf("%s", quads[i]->addr2.contents.name);
				a2 = quads[i]->addr2.contents.name;
				break;
			default:
				break;
		}
		
		switch (quads[i]->addr3.kind) 
		{
			case Empty:
				a3 = " - ";
				break;
			case IntConst:
				sprintf(a3,"%d",quads[i]->addr3.contents.val);
				break;
			case DouConst:
				sprintf(a3,"%f",quads[i]->addr3.contents.dval);
				break;
			case String:
				//printf("%s", quads[i]->addr3.contents.name);
				a3 = quads[i]->addr3.contents.name;
				break;
			default:
				break;
		}
		
		//printf("Finished switches, printing quad details\n");
		//printf("%s",a1);
		//printf("%s",a2);
		//printf("%s",a3);
		printf("(%s,%s,%s,%s)\n",namesOfOps[quads[i]->op],a1,a2,a3);
		i++;
	}
	
	if(DOASSEMBLY)
	{
		//File for final assembly output
		FILE *file;
		file = fopen("tm48code.txt","a+");
		
		//Send over to the assembly generator, the quads array and symbol table
		AssemblyGen(quads, file, symtab);
					
					
		fclose(file);
	}
	 
	return 0;
}