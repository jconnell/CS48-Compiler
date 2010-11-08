/*
 *  assemblygen.h
 *  Assembly Generation from quads.
 *  Written by Jonathan Connell and David Kopec
 */

#include "ast.h"
#include "quads.h"
#include "Symtab.h"
#include <stdio.h>


#ifndef ASSEM_H_
#define ASSEM_H_

typedef enum {math, inequality} AssemKind;

//the main assembly generation function
void AssemblyGen(Quad** q, FILE* file, SymbolTable* s);

#endif