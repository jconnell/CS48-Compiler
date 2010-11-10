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

typedef enum {math, enterScope, exitScope, inequality, assignment, read, write, jumptoquad, iffalse, loadparam, func_ret, cont, other} AssemKind;

//the main assembly generation function
void AssemblyGen(Quad** q, FILE* file, SymbolTable* s);


#endif