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
	
	while (i <= 90) {
		
		char type_to_store = 'i';		// Type of variable to be stored to (default: int)
		char type_of_storage = 'i';		// Type of variable to be stored (default: int)
		
		/* TYPE DETECTION OF THE FIRST QUAD VARIABLE */
		if (quads[i]->addr1.kind == String){
			s1 = LookupInSymbolTable(symtab, quads[i]->addr1.contents.name);
			printf(" %s", quads[i]->addr1.contents.name);
			if (s1->attrs->type == DouT)
				type_of_storage = 'd';
		}
		else if (quads[i]->addr1.kind == IntConst) {
			printf(" %d", quads[i]->addr1.contents.val);
		}
		else if (quads[i]->addr1.kind == DouConst) {
			printf(" %f", quads[i]->addr1.contents.dval);
			type_of_storage = 'd';
		}
		else printf(" - ");
		
		/* TYPE DETECTION OF THE SECOND QUAD VARIABLE */
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
		
		/* TYPE DETECTION OF THE THIRD QUAD VARIABLE */
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
		
		
		
/****************************************************/
/****************** TM48 COMMANDS *******************/
/****************************************************/
		
		if (quads[i]->op == add) {			// Addition (+)
			AK = math;
			AssemCommand = "ADD";
		}
		else if (quads[i]->op == sub) {		// Subtraction (-)
			AK = math;
			AssemCommand = "SUB";
		}
		else if (quads[i]->op == mul) {		// Multiplication (*)
			AK = math;
			AssemCommand = "MUL";
		}
		else if (quads[i]->op == divi) {	// Division (/)
			AK = math;
			AssemCommand = "DIV";
		}
		else if (quads[i]->op == gt) {		// Greater Than (>)
			AK = inequality;
			AssemCommand = "GT";
		}
		else if (quads[i]->op == gteq) {	// Greater Than or Equal (>=)
			AK = inequality;
			AssemCommand = "GE";
		}
		else if (quads[i]->op == lt) {		// Less Than (<)
			AK = inequality;
			AssemCommand = "LT";
		}
		else if (quads[i]->op == lteq) {	// Less Than or Equal (<=)
			AK = inequality;
			AssemCommand = "LE";
		}
		else if (quads[i]->op == eq) {		// Equals (=)
			AK = inequality;
			AssemCommand = "EQ";
		}
		else if (quads[i]->op == neq) {		// Not Equal To (!=)
			AK = inequality;
			AssemCommand = "NE";
		}
		else if (quads[i]->op == asn) {		// Store value
			AK = assignment;
			AssemCommand = "ST";
		}
		else if (quads[i]->op == exs) {		// Exit Scope
			AK = other;
			AssemCommand = " ";
		}
		else if (quads[i]->op == ens) {		// Enter Scope
			AK = other;
			AssemCommand = " ";
		}
		else {
			AK = other;
			AssemCommand = "ERROR";
		}
/****************************************************/
/***************** CODE GENERATION ******************/
/****************************************************/
		
		printf("%d %s %d \n", AK, AssemCommand, quads[i]->op);
		switch (AK) {
			/*** MATH OPERATORS ***/
			case (math):  // Arithmetic Operations
				if (quads[i]->addr2.kind == String) {	
					printf("%d\n", s2->attrs->type);
					if (s2->attrs->type == IntT) {		
						fprintf(file, "LD 0, %d(5)\n", s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								// Operation between Int var and Int var
								fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "%s 0, 1, 0\n", AssemCommand);
							}
							else if (s3->attrs->type == DouT) {
								// Operation between Int var and Double var
								type_to_store = 'd';
								fprintf(file, "CVTIF 0, 0(0)\n");
								fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							// Operation between Int var and Int constant
							fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
							fprintf(file, "%s 0, 1, 0\n", AssemCommand);
						}
						else if (quads[i]->addr3.kind == DouConst) {
							// Operation between Int var and Double constant
							type_to_store = 'd';
							fprintf(file, "CVTIF 0, 0(0)\n");
							fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (s2->attrs->type == DouT) {
						type_to_store = 'd';
						fprintf(file, "LDF 0, %d(5)\n", s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								// Operation between Double var and Int var
								fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "CVTIF 1, 0(1)\n");
								fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
							}
							else if (s3->attrs->type == DouT) {
								// Operation between Double var and Double var
								fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							// Operation between Double var and Int constant
							fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
							fprintf(file, "CVTIF 1, 0(1)\n");
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else if (quads[i]->addr3.kind == DouConst) {
							// Operation between Double var and Double constant
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
				}
				else if (quads[i]->addr2.kind == IntConst) {
					fprintf(file, "LDC 0, %d(0)\n", quads[i]->addr2.contents.val);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							// Operation between Int constant and Int var
							fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "%s 0, 1, 0\n", AssemCommand);
						}
						else if (s3->attrs->type == DouT) {
							// Operation between Int constant and Double var
							type_to_store = 'd';
							fprintf(file, "CVTIF 0, 0(0)\n");
							fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						// Operation between Int constant and Int constant
						fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
						fprintf(file, "%s 0, 1, 0\n", AssemCommand);
					}
					else if (quads[i]->addr3.kind == DouConst) {
						// Operation between Int constant and Double constant
						type_to_store = 'd';
						fprintf(file, "CVTIF 0, 0(0)\n");
						fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
						fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else if (quads[i]->addr2.kind == DouConst) {
					type_to_store = 'd';
					fprintf(file, "LDFC 0, %f(0)\n", quads[i]->addr2.contents.dval);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							// Operation between Double constant and Int var
							fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "CVTIF 1, 0(1)\n");
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else if (s3->attrs->type == DouT) {
							// Operation between Double constant and Double var
							fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						// Operation between Double constant and Int constant
						fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
						fprintf(file, "CVTIF 1, 0(1)\n");
						fprintf(file, "%sF 0, 1, 0\n", AssemCommand);
					}
					else if (quads[i]->addr3.kind == DouConst) {
						// Operation between Double constant and Double constant
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
				
				if (type_to_store == 'd' && type_of_storage == 'd') {		// Store double in double
					fprintf(file, "STF 0, %d(5)\n", s1->attrs->memoffset);
				}
				else if (type_to_store == 'd' && type_of_storage == 'i') {	// Store double in int
					fprintf(file, "CVTFI 0, 0(0)\n");
					fprintf(file, "ST 0, %d(5)\n", s1->attrs->memoffset);
				}
				else if (type_to_store == 'i' && type_of_storage == 'd') {	// Store int in double
					fprintf(file, "CVTIF 0, 0(0)\n");
					fprintf(file, "STF 0, %d(5)\n", s1->attrs->memoffset);
				}
				else if (type_to_store == 'i' && type_of_storage == 'i') {	// Store int in int
					fprintf(file, "ST 0, %d(5)\n", s1->attrs->memoffset);
				}
				else {
					fprintf(file, "ERROR\n");
				} 
							
							
				break;
			/*** INEQUALITIES ***/
			case inequality:
				if (quads[i]->addr2.kind == String) {
					if (s2->attrs->type == IntT) {
						fprintf(file, "LD 0, %d(5)\n", s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								// Ineqality between Int var and Int var
								fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "SUB 0, 1, 0\n");
								fprintf(file, "J%s 0, 2(7)\n", AssemCommand);
								fprintf(file, "LDC 0, 0(0)\n");
								fprintf(file, "LDA 7, 1(7)\n");
								fprintf(file, "LDC 0, 1(0)\n");
							}
							else if (s3->attrs->type == DouT) {
								// Ineqality between Int var and Double var
								fprintf(file, "CVTIF 0, 0(0)\n");
								fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "SUBF 0, 1, 0\n");
								fprintf(file, "JF%s 0, 2(7)\n", AssemCommand);
								fprintf(file, "LDC 0, 0(0)\n");
								fprintf(file, "LDA 7, 1(7)\n");
								fprintf(file, "LDC 0, 1(0)\n");
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							// Ineqality between Int var and Int constant
							fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
							fprintf(file, "SUB 0, 1, 0\n");
							fprintf(file, "J%s 0, 2(7)\n", AssemCommand);
							fprintf(file, "LDC 0, 0(0)\n");
							fprintf(file, "LDA 7, 1(7)\n");
							fprintf(file, "LDC 0, 1(0)\n");
						}
						else if (quads[i]->addr3.kind == DouConst) {
							// Ineqality between Int var and Double constant
							fprintf(file, "CVTIF 0, 0(0)\n");
							fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
							fprintf(file, "SUBF 0, 1, 0\n");
							fprintf(file, "JF%s 0, 2(7)\n", AssemCommand);
							fprintf(file, "LDC 0, 0(0)\n");
							fprintf(file, "LDA 7, 1(7)\n");
							fprintf(file, "LDC 0, 1(0)\n");
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (s2->attrs->type == DouT) {
						fprintf(file, "LDF 0, %d(5)\n", s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								// Ineqality between Double var and Int var
								fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
								fprintf(file, "CVTIF 1, 0(1)\n");
							}
							else if (s3->attrs->type == DouT) {
								// Ineqality between Double var and Double var
								fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							// Ineqality between Double var and Int constant
							fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
							fprintf(file, "CVTIF 1, 0(1)\n");
						}
						else if (quads[i]->addr3.kind == DouConst) {
							// Ineqality between Double var and Double constant
							fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
						}
						else {
							fprintf(file, "ERROR\n");
						}
						fprintf(file, "SUBF 0, 1, 0\n");
						fprintf(file, "JF%s 0, 2(7)\n", AssemCommand);
						fprintf(file, "LDC 0, 0(0)\n");
						fprintf(file, "LDA 7, 1(7)\n");
						fprintf(file, "LDC 0, 1(0)\n");
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else if (quads[i]->addr2.kind == IntConst) {
					fprintf(file, "LDC 0, %d(0)\n", quads[i]->addr2.contents.val);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							// Ineqality between Int constant and Int var
							fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "SUB 0, 1, 0\n");
							fprintf(file, "J%s 0, 2(7)\n", AssemCommand);
							fprintf(file, "LDC 0, 0(0)\n");
							fprintf(file, "LDA 7, 1(7)\n");
							fprintf(file, "LDC 0, 1(0)\n");
						}
						else if (s3->attrs->type == DouT) {
							// Ineqality between Int constant and Double var
							fprintf(file, "CVTIF 0, 0(0)\n");
							fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "SUBF 0, 1, 0\n");
							fprintf(file, "JF%s 0, 2(7)\n", AssemCommand);
							fprintf(file, "LDC 0, 0(0)\n");
							fprintf(file, "LDA 7, 1(7)\n");
							fprintf(file, "LDC 0, 1(0)\n");
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						// Ineqality between Int constant and Int constant
						fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
						fprintf(file, "SUB 0, 1, 0\n");
						fprintf(file, "J%s 0, 2(7)\n", AssemCommand);
						fprintf(file, "LDC 0, 0(0)\n");
						fprintf(file, "LDA 7, 1(7)\n");
						fprintf(file, "LDC 0, 1(0)\n");
					}
					else if (quads[i]->addr3.kind == DouConst) {
						// Ineqality between Int constant and Double constant
						fprintf(file, "CVTIF 0, 0(0)\n");
						fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
						fprintf(file, "SUBF 0, 1, 0\n");
						fprintf(file, "JF%s 0, 2(7)\n", AssemCommand);
						fprintf(file, "LDC 0, 0(0)\n");
						fprintf(file, "LDA 7, 1(7)\n");
						fprintf(file, "LDC 0, 1(0)\n");
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else if (quads[i]->addr2.kind == DouConst) {
					fprintf(file, "LDFC 0, %f(0)\n", quads[i]->addr2.contents.dval);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							// Ineqality between Double constant and Int var
							fprintf(file, "LD 1, %d(5)\n", s3->attrs->memoffset);
							fprintf(file, "CVTIF 1, 0(1)\n");
						}
						else if (s3->attrs->type == DouT) {
							// Ineqality between Double constant and Double var
							fprintf(file, "LDF 1, %d(5)\n", s3->attrs->memoffset);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						// Ineqality between Double constant and Int constant
						fprintf(file, "LDC 1, %d(0)\n", quads[i]->addr3.contents.val);
						fprintf(file, "CVTIF 1, 0(1)\n");
					}
					else if (quads[i]->addr3.kind == DouConst) {
						// Ineqality between Double constant and Double constant
						fprintf(file, "LDFC 1, %f(0)\n", quads[i]->addr3.contents.dval);
					}
					else {
						fprintf(file, "ERROR\n");
					}
					fprintf(file, "SUBF 0, 1, 0\n");
					fprintf(file, "JF%s 0, 2(7)\n", AssemCommand);
					fprintf(file, "LDC 0, 0(0)\n");
					fprintf(file, "LDA 7, 1(7)\n");
					fprintf(file, "LDC 0, 1(0)\n");
				}
				else {
					fprintf(file, "ERROR\n");
				}
				break;
			/*** ASSIGNMENT ***/
			case assignment:
				if (type_of_storage == 'd') {
					if (quads[i]->addr2.kind == String) {
						if (s2->attrs->type == IntT) {
							// Store an Int variable in a Double variable
							fprintf(file, "LD 0, %d(5)\n", s2->attrs->memoffset);
							fprintf(file, "CVTIF 0, 0(0)\n");
							fprintf(file, "STF 0, %d(5)\n", s1->attrs->memoffset);
						}
						else if (s2->attrs->type == DouT) {
							// Store a Double variable in a Double variable
							fprintf(file, "LDF 0, %d(5)\n", s2->attrs->memoffset);
							fprintf(file, "STF 0, %d(5)\n", s1->attrs->memoffset);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr2.kind == DouConst) {
						// Store a Double constant in a Double variable
						fprintf(file, "LDFC 0, %f(0)\n", quads[i]->addr2.contents.dval);
						fprintf(file, "STF 0, %d(5)\n", s1->attrs->memoffset);
					}
					else if (quads[i]->addr2.kind == IntConst) {
						// Store an Int constant in a Double variable
						fprintf(file, "LDC 0, %d(0)\n", quads[i]->addr2.contents.val);
						fprintf(file, "CVTIF 0 0(0)\n");
						fprintf(file, "STF 0, %d(5)\n", s1->attrs->memoffset);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else { // If storage type is an integer
					if (quads[i]->addr2.kind == String) {
						if (s2->attrs->type == IntT) {
							// Store an Int variable in an Int variable
							fprintf(file, "LD 0, %d(5)\n", s2->attrs->memoffset);
							fprintf(file, "ST 0, %d(5)\n", s1->attrs->memoffset);
						}
						else if (s2->attrs->type == DouT) {
							// Store a Double variable in an Int variable
							fprintf(file, "LDF 0, %d(5)\n", s2->attrs->memoffset);
							fprintf(file, "CVTFI 0, 0(0)\n");
							fprintf(file, "ST 0, %d(5)\n", s1->attrs->memoffset);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr2.kind == DouConst) {
						// Store a Double constant in an Int variable
						fprintf(file, "LDFC 0, %f(0)\n", quads[i]->addr2.contents.dval);
						fprintf(file, "CVTFI 0 0(0)\n");
						fprintf(file, "ST 0, %d(5)\n", s1->attrs->memoffset);
					}
					else if (quads[i]->addr2.kind == IntConst) {
						// Store an Int constant in an Int variable
						fprintf(file, "LDC 0, %d(0)\n", quads[i]->addr2.contents.val);
						fprintf(file, "ST 0, %d(5)\n", s1->attrs->memoffset);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				break;
				
			case enterScope:
				//printf(" Level %d \n", GetNodeLevel(s1));
				//printf("ENTERING SCOPE\n");
				//EnterScope(symtab);
				//continue;
				break;
			case exitScope:
				//LeaveScope(symtab);
				//printf(" Level %d \n", GetNodeLevel(s1));
				//continue;
				break;
			default:
				printf("DERP\n");
				break;
				
		}
		i++;
	}
}
