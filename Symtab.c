/* Symtab.c
 * Functions for the symbol table.
 * Written by THC for CS 48.
 * You should extend the functions as appropriate.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Symtab.h"


/* Create a SymNode and return a pointer to it. */
static SymNode *CreateSymNode(char *name) {
	SymNode *node = malloc(sizeof(SymNode));
	node->name = strdup(name);
	node->level = -1;
	node->next = NULL;
	/*set attributes to default values*/
	node->attrs = malloc(sizeof(NodeAttrs));
	node->attrs->size = 0;
	node->attrs->type = 'u';
	node->attrs->address.intAddress = NULL;
	node->attrs->flags = NULL;
	return node;
}

/* Destroy a SymNode. */
static void DestroySymNode(SymNode *node) {
	free(node->name);
	/*we had to malloc flags since it's a string, so we must free it*/
	free(node->attrs->flags);
	free(node->attrs);
	free(node);
}

/* set the attributes of a node*/
void SetNodeAttributes(SymNode *node, NodeAttrs *na)
{
	node->attrs = na;
}

/* set the attributes of a node*/
NodeAttrs* LookupAttributes(SymNode *node)
{
	return node->attrs;
}

/* Set the name in this node. */
void SetNodeName(SymNode *node, char *name) {
	if (node->name != NULL)
		free(node->name);
	node->name = strdup(name);
}

/* Does the identifier in this node equal otherName? */
int NameIsEqual(SymNode *node, char *name) {
	return !strcmp(node->name, name);
}

/* Return the level of this node. */
int GetNodeLevel(SymNode *node) {
	return node->level;
}



/* Create an empty SymHashTable and return a pointer to it.  The
 parameter entries gives the initial size of the table. */
static SymHashTable *CreateSymHashTable(int entries) {
	SymHashTable *hashTable = malloc(sizeof(SymHashTable));
	hashTable->size = entries;
	hashTable->table = calloc(entries, sizeof(SymNode *));
	int i;
	for (i = 0; i < entries; i++)
		hashTable->table[i] = NULL;
	hashTable->outerScope = NULL;
	hashTable->level = -1;
	return hashTable;
}


/* Destroy a SymHashTable. */
static void DestroySymHashTable(SymHashTable *hashTable) {
	int i;
	for (i = 0; i < hashTable->size; i++) {
		SymNode *node, *next;
		
		for (node = hashTable->table[i]; node != NULL; node = next) {
			next = node->next;
			DestroySymNode(node);
		}
	}
}


/* Peter Weinberger's hash function, from Aho, Sethi, & Ullman
 p. 436. */
static int HashPJW(char *s, int size) {
	unsigned h = 0, g;
	char *p;
	
	for (p = s; *p != '\0'; p++) {
		h = (h << 4) + *p;
		if ((g = (h & 0xf0000000)) != 0)
			h ^= (g >> 24) ^ g;
    }
	
	return h % size;
}

/* Look up an entry in a SymHashTable, returning either a pointer to
 the SymNode for the entry or NULL.  slot is where to look; if slot
 == NOHASHSLOT, then apply the hash function to figure it out. */
static SymNode *LookupSymHashTable(SymHashTable *hashTable, char *name,
								   int slot) {
	SymNode *node;
	
	if (slot == NOHASHSLOT)
		slot = HashPJW(name, hashTable->size);
	
	for (node = hashTable->table[slot];
		 node != NULL && !NameIsEqual(node, name);
		 node = node->next)
		;
	
	return node;
}

/* Insert a new entry into a SymHashTable, but only if it is not
 already present. */
static SymNode *InsertIntoSymHashTable(SymHashTable *hashTable, char *name) {
	int slot = HashPJW(name, hashTable->size);
	SymNode *node = LookupSymHashTable(hashTable, name, slot);
	
	if (node == NULL) {
		node = CreateSymNode(name);
		node->level = hashTable->level;
		node->next = hashTable->table[slot];
		hashTable->table[slot] = node;
	}
	
	return node;
}


static const int HASHSIZE = 211;

/* Create an empty symbol table. */
SymbolTable *CreateSymbolTable() {
	SymbolTable *symtab = malloc(sizeof(SymbolTable));
	symtab->innerScope = CreateSymHashTable(HASHSIZE);
	symtab->innerScope->outerScope = NULL;
	symtab->innerScope->level = 0;
	return symtab;
}

/* Destroy a symbol table. */
void DestroySymbolTable(SymbolTable *symtab) {
	SymHashTable *table, *outer;
	
	for (table = symtab->innerScope; table != NULL; table = outer) {
		outer = table->outerScope;
		DestroySymHashTable(table);
	}
}

/* Insert an entry into the innermost scope of symbol table.  First
 make sure it's not already in that scope.  Return a pointer to the
 entry.Insert a new entry into a SymbolTable.  Assumes that this
 entry is not already present. */
SymNode *InsertIntoSymbolTable(SymbolTable *symtab, char *name) {
	if (symtab->innerScope == NULL) {
		fprintf(stderr, "Error: inserting into an empty symbol table\n");
		exit(1);
	}
	
	SymNode *node = LookupSymHashTable(symtab->innerScope, name, NOHASHSLOT);
	
	if (node == NULL)
		node = InsertIntoSymHashTable(symtab->innerScope, name);
	
	return node;
}

/* Lookup an entry in a symbol table.  Return a pointer to it if
 found, NULL otherwise. */
SymNode *LookupInSymbolTable(SymbolTable *symtab, char *name) {
	SymNode *node;
	SymHashTable *hashTable;
	
	for (node = NULL, hashTable = symtab->innerScope;
		 node == NULL && hashTable != NULL;
		 hashTable = hashTable->outerScope)
		node = LookupSymHashTable(hashTable, name, NOHASHSLOT);
	
	return node;
}

/* Enter a new scope. */
void EnterScope(SymbolTable *symtab) {
	SymHashTable *hashTable = CreateSymHashTable(HASHSIZE);
	hashTable->outerScope = symtab->innerScope;
	hashTable->level = symtab->innerScope->level + 1;
	symtab->innerScope = hashTable;
}

/* Leave a scope. */
void LeaveScope(SymbolTable *symtab) {
	SymHashTable *hashTable = symtab->innerScope;
	symtab->innerScope = hashTable->outerScope;
	DestroySymHashTable(hashTable);
}
