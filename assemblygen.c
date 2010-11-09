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
	int i = 0; // Quad number
	int AssemNum = 0; // Assembly code line number
	
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
		else if (quads[i]->op == rd) {		// Read from STD IN
			AK = read;
			AssemCommand = "IN";
		}
		else if (quads[i]->op == wri) {		// Write to STD OUT
			AK = write;
			AssemCommand = "OUT";
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
						fprintf(file, "%d LD 0, %d(5)\n", AssemNum++, s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								// Operation between Int var and Int var
								fprintf(file, "%d LD 1, %d(5)\n", AssemNum++, s3->attrs->memoffset); 
								fprintf(file, "%d %s 0, 1, 0\n", AssemNum++, AssemCommand);
							}
							else if (s3->attrs->type == DouT) {
								// Operation between Int var and Double var
								type_to_store = 'd';
								fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
								fprintf(file, "%d LDF 1, %d(5)\n", AssemNum++, s3->attrs->memoffset); 
								fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							// Operation between Int var and Int constant
							fprintf(file, "%d LDC 1, %d(0)\n", AssemNum++, quads[i]->addr3.contents.val);
							fprintf(file, "%d %s 0, 1, 0\n", AssemNum++, AssemCommand);
						}
						else if (quads[i]->addr3.kind == DouConst) {
							// Operation between Int var and Double constant
							type_to_store = 'd';
							fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d LDFC 1, %f(0)\n", AssemNum++, quads[i]->addr3.contents.dval); 
							fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (s2->attrs->type == DouT) {
						type_to_store = 'd';
						fprintf(file, "%d LDF 0, %d(5)\n", AssemNum++, s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								// Operation between Double var and Int var
								fprintf(file, "%d LD 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
								fprintf(file, "%d CVTIF 1, 0(1)\n", AssemNum++);
								fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
							}
							else if (s3->attrs->type == DouT) {
								// Operation between Double var and Double var
								fprintf(file, "%d LDF 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
								fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							// Operation between Double var and Int constant
							fprintf(file, "%d LDC 1, %d(0)\n", AssemNum++, quads[i]->addr3.contents.val);
							fprintf(file, "%d CVTIF 1, 0(1)\n", AssemNum++);
							fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
						}
						else if (quads[i]->addr3.kind == DouConst) {
							// Operation between Double var and Double constant
							fprintf(file, "%d LDFC 1, %f(0)\n", AssemNum++, quads[i]->addr3.contents.dval);
							fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
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
					fprintf(file, "%d LDC 0, %d(0)\n", AssemNum++, quads[i]->addr2.contents.val);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							// Operation between Int constant and Int var
							fprintf(file, "%d LD 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
							fprintf(file, "%d %s 0, 1, 0\n", AssemNum++, AssemCommand);
						}
						else if (s3->attrs->type == DouT) {
							// Operation between Int constant and Double var
							type_to_store = 'd';
							fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d LDF 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
							fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						// Operation between Int constant and Int constant
						fprintf(file, "%d LDC 1, %d(0)\n", AssemNum++, quads[i]->addr3.contents.val);
						fprintf(file, "%d %s 0, 1, 0\n", AssemNum++, AssemCommand);
					}
					else if (quads[i]->addr3.kind == DouConst) {
						// Operation between Int constant and Double constant
						type_to_store = 'd';
						fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
						fprintf(file, "%d LDFC 1, %f(0)\n", AssemNum++, quads[i]->addr3.contents.dval);
						fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else if (quads[i]->addr2.kind == DouConst) {
					type_to_store = 'd';
					fprintf(file, "%d LDFC 0, %f(0)\n", AssemNum++, quads[i]->addr2.contents.dval);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							// Operation between Double constant and Int var
							fprintf(file, "%d LD 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
							fprintf(file, "%d CVTIF 1, 0(1)\n", AssemNum++);
							fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
						}
						else if (s3->attrs->type == DouT) {
							// Operation between Double constant and Double var
							fprintf(file, "%d LDF 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
							fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						// Operation between Double constant and Int constant
						fprintf(file, "%d LDC 1, %d(0)\n", AssemNum++, quads[i]->addr3.contents.val);
						fprintf(file, "%d CVTIF 1, 0(1)\n", AssemNum++);
						fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
					}
					else if (quads[i]->addr3.kind == DouConst) {
						// Operation between Double constant and Double constant
						fprintf(file, "%d LDFC 1, %f(0)\n", AssemNum++, quads[i]->addr3.contents.dval);
						fprintf(file, "%d %sF 0, 1, 0\n", AssemNum++, AssemCommand);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else {
					fprintf(file, "ERROR\n");
				}
				
				if (type_to_store == 'd' && type_of_storage == 'd') {		// Store double in double
					fprintf(file, "%d STF 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
				}
				else if (type_to_store == 'd' && type_of_storage == 'i') {	// Store double in int
					fprintf(file, "%d CVTFI 0, 0(0)\n", AssemNum++);
					fprintf(file, "%d ST 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
				}
				else if (type_to_store == 'i' && type_of_storage == 'd') {	// Store int in double
					fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
					fprintf(file, "%d STF 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
				}
				else if (type_to_store == 'i' && type_of_storage == 'i') {	// Store int in int
					fprintf(file, "%d ST 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
				}
				else {
					fprintf(file, "ERROR\n");
				} 
				break;
				

			/*** INEQUALITIES ***/
			case inequality:
				if (quads[i]->addr2.kind == String) {
					if (s2->attrs->type == IntT) {
						fprintf(file, "%d LD 0, %d(5)\n", AssemNum++, s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								// Ineqality between Int var and Int var
								fprintf(file, "%d LD 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
								fprintf(file, "%d SUB 0, 1, 0\n", AssemNum++);
								fprintf(file, "%d J%s 0, 2(7)\n", AssemNum++, AssemCommand);
								fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
								fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
								fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
							}
							else if (s3->attrs->type == DouT) {
								// Ineqality between Int var and Double var
								fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
								fprintf(file, "%d LDF 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
								fprintf(file, "%d SUBF 0, 1, 0\n", AssemNum++);
								fprintf(file, "%d JF%s 0, 2(7)\n", AssemNum++, AssemCommand);
								fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
								fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
								fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							// Ineqality between Int var and Int constant
							fprintf(file, "%d LDC 1, %d(0)\n", AssemNum++, quads[i]->addr3.contents.val);
							fprintf(file, "%d SUB 0, 1, 0\n", AssemNum++);
							fprintf(file, "%d J%s 0, 2(7)\n", AssemNum++, AssemCommand);
							fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
							fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
						}
						else if (quads[i]->addr3.kind == DouConst) {
							// Ineqality between Int var and Double constant
							fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d LDFC 1, %f(0)\n", AssemNum++, quads[i]->addr3.contents.dval);
							fprintf(file, "%d SUBF 0, 1, 0\n", AssemNum++);
							fprintf(file, "%d JF%s 0, 2(7)\n", AssemNum++, AssemCommand);
							fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
							fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (s2->attrs->type == DouT) {
						fprintf(file, "%d LDF 0, %d(5)\n", AssemNum++, s2->attrs->memoffset);
						if (quads[i]->addr3.kind == String) {
							if (s3->attrs->type == IntT) {
								// Ineqality between Double var and Int var
								fprintf(file, "%d LD 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
								fprintf(file, "%d CVTIF 1, 0(1)\n", AssemNum++);
							}
							else if (s3->attrs->type == DouT) {
								// Ineqality between Double var and Double var
								fprintf(file, "%d LDF 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
							}
							else {
								fprintf(file, "ERROR\n");
							}
						}
						else if (quads[i]->addr3.kind == IntConst) {
							// Ineqality between Double var and Int constant
							fprintf(file, "%d LDC 1, %d(0)\n", AssemNum++, quads[i]->addr3.contents.val);
							fprintf(file, "%d CVTIF 1, 0(1)\n", AssemNum++);
						}
						else if (quads[i]->addr3.kind == DouConst) {
							// Ineqality between Double var and Double constant
							fprintf(file, "%d LDFC 1, %f(0)\n", AssemNum++, quads[i]->addr3.contents.dval);
						}
						else {
							fprintf(file, "ERROR\n");
						}
						fprintf(file, "%d SUBF 0, 1, 0\n", AssemNum++);
						fprintf(file, "%d JF%s 0, 2(7)\n", AssemNum++, AssemCommand);
						fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
						fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
						fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else if (quads[i]->addr2.kind == IntConst) {
					fprintf(file, "%d LDC 0, %d(0)\n", AssemNum++, quads[i]->addr2.contents.val);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							// Ineqality between Int constant and Int var
							fprintf(file, "%d LD 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
							fprintf(file, "%d SUB 0, 1, 0\n", AssemNum++);
							fprintf(file, "%d J%s 0, 2(7)\n", AssemNum++, AssemCommand);
							fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
							fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
						}
						else if (s3->attrs->type == DouT) {
							// Ineqality between Int constant and Double var
							fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d LDF 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
							fprintf(file, "%d SUBF 0, 1, 0\n", AssemNum++);
							fprintf(file, "%d JF%s 0, 2(7)\n", AssemNum++, AssemCommand);
							fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
							fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						// Ineqality between Int constant and Int constant
						fprintf(file, "%d LDC 1, %d(0)\n", AssemNum++, quads[i]->addr3.contents.val);
						fprintf(file, "%d SUB 0, 1, 0\n", AssemNum++);
						fprintf(file, "%d J%s 0, 2(7)\n", AssemNum++, AssemCommand);
						fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
						fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
						fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
					}
					else if (quads[i]->addr3.kind == DouConst) {
						// Ineqality between Int constant and Double constant
						fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
						fprintf(file, "%d LDFC 1, %f(0)\n", AssemNum++, quads[i]->addr3.contents.dval);
						fprintf(file, "%d SUBF 0, 1, 0\n", AssemNum++);
						fprintf(file, "%d JF%s 0, 2(7)\n", AssemNum++, AssemCommand);
						fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
						fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
						fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else if (quads[i]->addr2.kind == DouConst) {
					fprintf(file, "%d LDFC 0, %f(0)\n", AssemNum++, quads[i]->addr2.contents.dval);
					if (quads[i]->addr3.kind == String) {
						if (s3->attrs->type == IntT) {
							// Ineqality between Double constant and Int var
							fprintf(file, "%d LD 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
							fprintf(file, "%d CVTIF 1, 0(1)\n", AssemNum++);
						}
						else if (s3->attrs->type == DouT) {
							// Ineqality between Double constant and Double var
							fprintf(file, "%d LDF 1, %d(5)\n", AssemNum++, s3->attrs->memoffset);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr3.kind == IntConst) {
						// Ineqality between Double constant and Int constant
						fprintf(file, "%d LDC 1, %d(0)\n", AssemNum++, quads[i]->addr3.contents.val);
						fprintf(file, "%d CVTIF 1, 0(1)\n", AssemNum++);
					}
					else if (quads[i]->addr3.kind == DouConst) {
						// Ineqality between Double constant and Double constant
						fprintf(file, "%d LDFC 1, %f(0)\n", AssemNum++, quads[i]->addr3.contents.dval);
					}
					else {
						fprintf(file, "ERROR\n");
					}
					fprintf(file, "%d SUBF 0, 1, 0\n", AssemNum++);
					fprintf(file, "%d JF%s 0, 2(7)\n", AssemNum++, AssemCommand);
					fprintf(file, "%d LDC 0, 0(0)\n", AssemNum++);
					fprintf(file, "%d LDA 7, 1(7)\n", AssemNum++);
					fprintf(file, "%d LDC 0, 1(0)\n", AssemNum++);
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
							fprintf(file, "%d LD 0, %d(5)\n", AssemNum++, s2->attrs->memoffset);
							fprintf(file, "%d CVTIF 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d STF 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
						}
						else if (s2->attrs->type == DouT) {
							// Store a Double variable in a Double variable
							fprintf(file, "%d LDF 0, %d(5)\n", AssemNum++, s2->attrs->memoffset);
							fprintf(file, "%d STF 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr2.kind == DouConst) {
						// Store a Double constant in a Double variable
						fprintf(file, "%d LDFC 0, %f(0)\n", AssemNum++, quads[i]->addr2.contents.dval);
						fprintf(file, "%d STF 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
					}
					else if (quads[i]->addr2.kind == IntConst) {
						// Store an Int constant in a Double variable
						fprintf(file, "%d LDC 0, %d(0)\n", AssemNum++, quads[i]->addr2.contents.val);
						fprintf(file, "%d CVTIF 0 0(0)\n", AssemNum++);
						fprintf(file, "%d STF 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else { // If storage type is an integer
					if (quads[i]->addr2.kind == String) {
						if (s2->attrs->type == IntT) {
							// Store an Int variable in an Int variable
							fprintf(file, "%d LD 0, %d(5)\n", AssemNum++, s2->attrs->memoffset);
							fprintf(file, "%d ST 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
						}
						else if (s2->attrs->type == DouT) {
							// Store a Double variable in an Int variable
							fprintf(file, "%d LDF 0, %d(5)\n", AssemNum++, s2->attrs->memoffset);
							fprintf(file, "%d CVTFI 0, 0(0)\n", AssemNum++);
							fprintf(file, "%d ST 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
						}
						else {
							fprintf(file, "ERROR\n");
						}
					}
					else if (quads[i]->addr2.kind == DouConst) {
						// Store a Double constant in an Int variable
						fprintf(file, "%d LDFC 0, %f(0)\n", AssemNum++, quads[i]->addr2.contents.dval);
						fprintf(file, "%d CVTFI 0 0(0)\n", AssemNum++);
						fprintf(file, "%d ST 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
					}
					else if (quads[i]->addr2.kind == IntConst) {
						// Store an Int constant in an Int variable
						fprintf(file, "%d LDC 0, %d(0)\n", AssemNum++, quads[i]->addr2.contents.val);
						fprintf(file, "%d ST 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				break;
			
			/*** READ FROM STANDARD INPUT ***/
			case read:
				if (s1->attrs->type == IntT) {
					fprintf(file, "%d IN 0, 0, 0\n", AssemNum++);
				}
				else if (s1->attrs->type == DouT) {
					fprintf(file, "%d INF 0, 0, 0\n", AssemNum++);
				}
				else {
					fprintf(file, "ERROR\n");
				}
				break;
			
			/*** WRITE TO STANDARD OUTPUT ***/
			case write:
				if (quads[i]->addr1.kind == String) {
					if (s1->attrs->type == IntT) {
						fprintf(file, "%d LD 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
						fprintf(file, "%d OUT 0, 0, 0\n", AssemNum++);
					}
					else if (s1->attrs->type == DouT) {
						fprintf(file, "%d LDF 0, %d(5)\n", AssemNum++, s1->attrs->memoffset);
						fprintf(file, "%d OUTF 0, 0, 0\n", AssemNum++);
					}
					else {
						fprintf(file, "ERROR\n");
					}
				}
				else if (quads[i]->addr1.kind == IntConst) {
					fprintf(file, "%d LDC 0, %d(0)\n", AssemNum++, quads[i]->addr1.contents.val);
					fprintf(file, "%d OUT 0, 0, 0\n", AssemNum++);
				}
				else if (quads[i]->addr1.kind == DouConst) {
					fprintf(file, "%d LDFC 0, %f(0)\n", AssemNum++, quads[i]->addr1.contents.dval);
					fprintf(file, "%d OUTF 0, 0, 0\n", AssemNum++);
				}
				else {
					fprintf(file, "ERROR\n");
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

