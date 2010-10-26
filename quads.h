/* quads.h
 * Declarations for the quad functions/code generation
 * Written by DK and JC for CS 48.
 */

#include "ast.h"

#ifndef QUADS_H_
#define QUADS_H_

//Going with modified versions of the Louden book's suggested quad structure

typedef enum {rd, gt, if_f, asn, lab, mul, div, add, sub, eq, wri, halt} OpKind;

typedef enum {Empty, IntConst, DouConst, String} AddrKind;

typedef struct
{
	AddrKind kind;
	union {
		int val;
		double dval;
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
void PatchQuad(int q, int l, Address n);
int NextQuad();
char *NewTemp();


#endif
