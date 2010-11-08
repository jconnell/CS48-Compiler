/*
 *  assemblygen.h
 *  Assembly Generation from quads.
 *  Written by Jonathan Connell and David Kopec
 */

#include "ast.h"
#include "quads.h"

#ifndef QUADS_H_
#define QUADS_H_

typedef enum {math, inequality} AssemKind;

//the main assembly generation function
void AssemblyGen(Quad** q, FILE* file, SymbolTable* s);

#endif