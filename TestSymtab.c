/*
 * TestSymtab.cc
 * Driver to test the symbol table functions.
 * Written by THC for CS 48.
 */

#include <stdio.h>
#include "Symtab.h"

int main() {
  SymbolTable *symtab = CreateSymbolTable();

  char command[100];
  char name[200];

  SymNode *x;

  printf("Now at level 0\n");

  do {
    printf("Command: ");
    scanf("%s", command);

    switch (command[0]) {
    case 'i':
      printf("insert name: ");
      scanf("%s", name);
      InsertIntoSymbolTable(symtab, name);
      break;

    case 'l':
      printf("lookup name: ");
      scanf("%s", name);
      x = LookupInSymbolTable(symtab, name);
      if (x == NULL)
	printf("%s not found\n", name);
      else
	printf("%s found at level %d\n", name, GetNodeLevel(x));
      break;

    case '{':
      EnterScope(symtab);
      printf("Now at level %d\n", symtab->innerScope->level);
      break;

    case '}':
      if (symtab->innerScope->level <= 0) {
	printf("Left outermost scope...bye\n");
	return 0;
      }
      else {
	LeaveScope(symtab);
	printf("Now at level %d\n", symtab->innerScope->level);
      }
      break;
      
    case 'q':
      break;
      
    default:
      printf("Valid commands are i (insert), l (lookup), "
	     "{ (enter scope), } (leave scope), q (quit)\n");
      break;
    }
  }
  while (command[0] != 'q');

  DestroySymbolTable(symtab);

  return 0;
}
