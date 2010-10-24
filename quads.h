/* Symtab.h
 * Declarations for the symbol table.
 * Written by THC for CS 48.
 * You should extend the structs and functions as appropriate.
 */

#include "ast.h"

#ifndef QUADS_H_
#define QUADS_H_

//Going with modified versions of the Louden book's suggested quad structure

typedef enum {rd, gt, if_f, asn, lab, mul, sub, eq, wri, halt} OpKind;

typedef enum {Empty, IntConst, DoubleConst, String} AddrKind;

typedef struct
{
	AddrKind kind;
	union {
		int val;
		char *name;
	} contents;
} Address;

typedef struct
{
	OpKind op;
	Address addr1, addr2, addr3;
} Quad;


int CG(ast_node n);
int GenQuad(OpKind o, Address a, Address b, Address c);
void PatchQuad(Quad q, int l, Address n);
int NextQuad();


#endif
