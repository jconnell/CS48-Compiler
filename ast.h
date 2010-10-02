/* 
 * ast.h
 *
 * File defining an enum and a struct for node types in an abstract
 * syntax tree.
 *
 * Written by THC for CS 48.  You should modify this file as
 * appropriate.
 *
 */

#ifndef AST_H_
#define AST_H_

/* You should fill in the various node types.  The following are given
   to you to start with.  You may add or remove node types as you
   wish. */
typedef enum { ROOT,
	       SEQ,
	       OP_ASSIGN, OP_PLUS, OP_MINUS, OP_TIMES, OP_DIVIDE,
	       OP_EQUALS,
	       IF, IF_ELSE,
	       ID, 
	       INT_LITERAL, DOUBLE_LITERAL } ast_node_type;

/* Structure for nodes of the abstract syntax tree.  You should
   add or remove fields as appropriate. */
typedef struct ast_node_struct *ast_node;
struct ast_node_struct {
  ast_node_type node_type;
  ast_node left_child, right_sibling;
  union {
    char * string;		/* for ID */
    int int_value;		/* for INT_LITERAL */
    double double_value;	/* for DOUBLE_LITERAL */
  } value;
};

#endif
