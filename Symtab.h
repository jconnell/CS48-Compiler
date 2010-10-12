/* Symtab.h
 * Declarations for the symbol table.
 * Written by THC for CS 48.
 * You should extend the structs and functions as appropriate.
 */

#ifndef SYMTAB_H_
#define SYMTAB_H_

#define NOHASHSLOT -1

/* Node in a linked list within the symbol table. */

typedef struct SymNode SymNode;
struct SymNode {
  char *name;	    /* name in this SymNode */
  int level;	    /* scope level, same for all in this hash table */
  SymNode *next;    /* next SymNode in list */
  /* Other attributes go here. */
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

#endif
