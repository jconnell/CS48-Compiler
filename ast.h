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
	INT_LITERAL, DOUBLE_LITERAL,
	FOR_LOOP, WHILE_LOOP, DO_WHILE_LOOP,
	OP_NOT_EQUALS, OP_GREATER_THAN, OP_LESS_THAN, OP_GREATER_EQUALS, OP_LESS_EQUALS,
	STRING_LITERAL,
	OP_NOT, OP_PRE_INCR, OP_POST_INCR, OP_PRE_DECR, OP_POST_DECR, OP_NEGATIVE,
	OP_AND, OP_OR,
	RETURN_S, READ, WRITE,
	INT_DEC, DOU_DEC, INT_ARRAY_DEC, DOU_ARRAY_DEC,
	FUNC_DEF, FUNC_PARAM, FUNC_CALL, CONSTANT,
	SWITCH_ST, SWITCH_CASE, BREAK_ST, CONTINUE_ST,
	OP_SIZEOF
	} ast_node_type;

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

ast_node build_ast(char *file_name);

#endif
