/* Symtab.h
 * Declarations for the symbol table.
 * Written by THC for CS 48.
 * You should extend the structs and functions as appropriate.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef SYMTAB_H_
#define SYMTAB_H_

#define NOHASHSLOT -1


//Types in the symbol table

typedef enum {None, IntT, DouT, StrT, FunT, IArT, DArT} typeenum;

/* Node in a linked list within the symbol table. */

typedef struct NodeAttrs NodeAttrs;

typedef union val{
	int ival;
	double dval;
	int *iaaddr;
	double *daaddr;
	char *sval;
} value;

/*we pass around this struct to refer to all of the attributes for a given node*/
struct NodeAttrs {
	int size;		/* the size of the data stored in the variable*/
	typeenum type;		/* a char representing the data type (i = int; d = double; 
					 p = int *; q = double *; c = char* (for string literals)
					 f = function; u = undeclared type*/
	value v;			/*the value associated with the symbol*/
	int flags;		/*is it a constant, static, etc*/	
};

typedef struct SymNode SymNode;
struct SymNode {
	char *name;	    /* name in this SymNode */
	int level;	    /* scope level, same for all in this hash table */
	SymNode *next;    /* next SymNode in list */
	/* Other attributes go here. */
	NodeAttrs *attrs;
};



/* Set the name in this node. */
void SetNodeName(SymNode *node, char *name);

/* Does the identifier in this node equal name? */
int NameIsEqual(SymNode *node, char *name);

/* Return the level of this node. */
int GetNodeLevel(SymNode *node);


/* Hash table for a given scope in a symbol table. */

typedef struct SymHashTable SymHashTable;
struct SymHashTable {
	int size;			/* size of hash table */
	SymNode **table;		/* hash table */
	SymHashTable *outerScope;	/* ptr to SymHashTable in next outer scope */
	int level;			/* level of scope, 0 is outermost */
};

/* Symbol table for all levels of scope. */

typedef struct {
	SymHashTable *innerScope;	/* pointer to SymHashTable of innermost scope */
} SymbolTable;

/* Create an empty symbol table. */
SymbolTable *CreateSymbolTable();

/* Destroy a symbol table. */
void DestroySymbolTable(SymbolTable *symtab);

/* Insert an entry into the innermost scope of symbol table.  First
 make sure it's not already in that scope.  Return a pointer to the
 entry. */
SymNode *InsertIntoSymbolTable(SymbolTable *symtab, char *name);

/* Lookup an entry in a symbol table.  Return a pointer to it if
 found, NULL otherwise. */
SymNode *LookupInSymbolTable(SymbolTable *symtab, char *name);

/* Enter a new scope. */
void EnterScope(SymbolTable *symtab);

/* Leave a scope. */
void LeaveScope(SymbolTable *symtab);

void SetNodeAttributes(SymNode *node, NodeAttrs *na);

NodeAttrs* LookupAttributes(SymNode *node);

void SetSizeAttr(SymNode *node, int s);
int GetSizeAttr(SymNode *node);
void SetTypeAttr(SymNode *node, typeenum te);
typeenum GetTypeAttr(SymNode *node);
void SetFlagsAttr(SymNode *node, int f);
int GetFlagsAttr(SymNode *node);
void SetValueAttr(SymNode *node, value v);
value GetValueAttr(SymNode *node);

#endif
