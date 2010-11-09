/* ast.c
 *
 * Skeleton code to build an abstract syntax tree from an input file
 * describing the structure of the tree.  Each line of the input file
 * says one of three things:
 * 1. DOWN: Go down one level in the tree, making the next node the left
 * child of the most recent node.
 * 2. UP: Go up one level in the tree.
 * 3. NODE: Define a node, making it by default the immediate right
 * sibling of the most recent node.  (That is, it's the right sibling
 * unless preceded by a DOWN or UP line.)
 *
 * A NODE should immediately follow a DOWN.  Two DOWNs in a row don't
 * make sense, because there would be no intervening level of the
 * tree.  A DOWN followed up an UP is a no-op.
 *
 * A NODE line should include the type of node (a symbol such as = or
 * + for an operator, ID for an identifier, INTLIT or DOUBLELIT for an
 * int literal or double literal, etc.) and, if appropriate, a value
 * (the name of the ID, the value of the literal, etc.).
 *
 * Written by THC for CS 48.  Relies on an enum and a struct for
 * various node types that appear in ast.h.  You should modify the
 * enum and struct as appropriate.  Also assumes that all tokens in
 * the input file are separated by whitespace.  Input is free format,
 * so that line breaks don't matter.
 *
 * Top-level function is build_ast, which takes as input a file name
 * and returns a pointer to the root node of the abstract syntax tree.
 * Functions main and print_ast are for testing.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Type of line in the input file. */
typedef enum { DOWN, UP, NODE, ERROR, DONE } line_type;

#define LINE_SIZE 256
static char input_line[LINE_SIZE]; /* line read in from file */
static FILE *input_file;	   /* input file */

/* Returns the next whitespace-separated token in the input file.
 Newlines are considered whitespace.  Returns NULL if no more tokens
 in the input file. */
char *get_token() {
	static char *line = input_line; /* pointer to next token from input file */
	char *token;			  /* token to return */
	
	do {
		/* If first line, or if finished with previous line, read the next
		 line.  If there is no next line, return NULL to indicate
		 EOF. */
		if (line == NULL || *line == '\0') {
			if (fgets(input_line, LINE_SIZE, input_file) == NULL)
				return NULL;
			line = input_line;  /* start at beginning of the line read in */
		}
		
		/* Returns the token and adjusts line to skip to the next one. */
		token = strsep(&line, " \t\n");
	}
	while (*token == '\0'); /* if no more tokens on this line, read another */
	
	return token;
}

/* Read the next token from the input file and return its type. */
ast_node_type get_node_type() {
	char *token = get_token();
	if (token == NULL)
		return DONE;
	else if (strcmp(token, "DOWN") == 0)
		return DOWN;
	else if (strcmp(token, "UP") == 0)
		return UP;
	else if (strcmp(token, "NODE") == 0)
		return NODE;
	else {
		fprintf(stderr, "Unrecognized node type: %s\n", token);
		return ERROR;
	}
}

/* Define a table of token types and their associated strings.  You
 should modify this table as appropriate.  The order of entries
 should match the order of enumerated values in ast_node_type. */
struct token_lookup {
	char *token;
	ast_node_type node_type;
};
static struct token_lookup token_table[] = {
	{ "ROOT", ROOT },
	{ "SEQ", SEQ },
	{ "=", OP_ASSIGN },
	{ "+", OP_PLUS },
	{ "-", OP_MINUS },
	{ "*", OP_TIMES },
	{ "/", OP_DIVIDE },
	{ "==", OP_EQUALS },
	{ "IF", IF },
	{ "IFELSE", IF_ELSE },
	{ "ID", ID },
	{ "INTLIT", INT_LITERAL },
	{ "DOUBLELIT", DOUBLE_LITERAL },
	{ "FORLOOP", FOR_LOOP },
	{ "WHILELOOP", WHILE_LOOP },
	{ "DOWHILELOOP", DO_WHILE_LOOP },
	{ "!=", OP_NOT_EQUALS },
	{ ">", OP_GREATER_THAN },
	{ "<", OP_LESS_THAN },
	{ ">=", OP_GREATER_EQUALS },
	{ "<=", OP_LESS_EQUALS },
	{ "STRLIT", STRING_LITERAL },
	{ "!", OP_NOT },
	{ "PRE++", OP_PRE_INCR },
	{ "POST++", OP_POST_INCR },
	{ "PRE--", OP_PRE_DECR },
	{ "POST--", OP_POST_DECR },
	{ "NEG", OP_NEGATIVE },
	{ "&&", OP_AND },
	{ "||", OP_OR },
	{ "RETURNS", RETURN_S },
	{ "READ", READ },
	{ "WRITE", WRITE },
	{ "INT", INT_DEC},
	{ "DOUBLE", DOU_DEC},
	{ "INT[]", INT_ARRAY_DEC},
	{ "DOUBLE[]", DOU_ARRAY_DEC},
	{ "FUNCTIONDEFINITION", FUNC_DEF},
	{ "FUNCTIONPARAMETER", FUNC_PARAM},
	{ "FUNCTIONCALL", FUNC_CALL},
	{ "CONST", CONSTANT},
	{ "SWITCH", SWITCH_ST},
	{ "CASE", SWITCH_CASE},
	{ "BREAK", BREAK_ST},
	{ "CONTINUE", CONTINUE_ST},
	{ "SIZEOF", OP_SIZEOF},
	{ NULL, 0 }
};

/* Set the attributes for a node that come from the input file. */
void process_node(ast_node node) {
	char *type = get_token();	/* get the node type as a string */
	
	/* Look up the node type in the token table. */
	int i;
	for (i = 0; 
		 token_table[i].token != NULL && strcmp(token_table[i].token, type) != 0;
		 i++)
		;
	
	if (token_table[i].token == NULL)
		fprintf(stderr, "Error: Unknown node type %s\n", type);
	else {
		node->node_type = token_table[i].node_type;	/* set the node type */
		
		/* Do actions specific to the node type.  You should add actions
		 as appropriate. */
		switch (node->node_type) {
			case ID:  /* get and store the actual id */
				node->value.string = strdup(get_token());  
				printf("%s in ast the string val for id", node->value.string);
				break;
				
			case INT_LITERAL:  /* get and store the int literal value */
				sscanf(get_token(), "%d", &(node->value.int_value));
				break;
				
			case DOUBLE_LITERAL:  /* get and store the double literal value */
				sscanf(get_token(), "%lf", &(node->value.double_value));
				break;
			
			case STRING_LITERAL: /* get and store the string literal value */
				node->value.string =strdup(get_token());
				break;
				
			default:
				break;
		}
	}
}

/* Build the subtree of the abstract syntax tree, given the root of
 the subtree. */
void build_ast_subtree(ast_node root) {
	ast_node_type node_type;	/* type of the next node */
	ast_node most_recent = root;  /* most recent node added */
	ast_node new_node;		/* new node to add */
	int is_leftmost = 1; /* is the next node to add a leftmost child? */
	
	while ((node_type = get_node_type()) != DONE) {
		switch (node_type) {
			case DOWN:
				/* Build a subtree whose root is the node most recently
				 added. */
				build_ast_subtree(most_recent);
				break;
				
			case UP:
				/* Done with this subtree, so return to the recursive call. */
				return;
				break;
				
			case NODE:
				/* Create a new node, and position it correctly in the tree. */
				new_node = malloc(sizeof(struct ast_node_struct));
				
				/* If we're adding a leftmost child, we have to adjust the
				 left_child pointer in the root.  Otherwise, we have to adjust
				 the right_sibling pointer in the most recently added node. */
				if (is_leftmost)
					root->left_child = new_node;
				else
					most_recent->right_sibling = new_node;
				
				/* No leftmost child or right sibling yet. */
				new_node->left_child = new_node->right_sibling = NULL;
				
				/* The next node can't be the leftmost child of its parent. */
				is_leftmost = 0;
				
				most_recent = new_node; /* this node is now most recently added */
				
				/* Set the attributes that come from the input file. */
				process_node(new_node);
				break;
				
			default:
				break;
		}
	}
}

/* Build an abstact syntax tree, based on the structure in a file.
 Returns the root of the abstract syntax tree that it builds. */
ast_node build_ast(char *file_name) {
	input_line[0] = '\0';		      /* no input tokens yet */
	input_file = fopen(file_name, "r"); /* open the file */
	
	/* Create the root and initialize it. */
	ast_node root;
	root = malloc(sizeof(struct ast_node_struct));
	root->node_type = ROOT;
	root->left_child = root->right_sibling = NULL;
	
	/* Read the input file, and build the abstract syntax tree. */
	build_ast_subtree(root);
	
	fclose(input_file);		/* all done with the file */
	return root;
}

/* Print the contents of a subtree of an abstract syntax tree, given
 the root of the subtree and the depth of the subtree root. */
void print_ast(ast_node root, int depth) {
	/* Print two spaces for every level of depth. */
	int i;
	for (i = 0; i < depth; i++)
		printf("  ");
	
	/* Print the node type. */
	printf("%s ", token_table[root->node_type].token);
	
	/* Print attributes specific to node types. */
	switch (root->node_type) {
		case ID:			/* print the id */
			printf("%s", root->value.string);
			break;
			
		case INT_LITERAL:		/* print the int literal */
			printf("%d", root->value.int_value);
			break;
			
		case DOUBLE_LITERAL:		/* print the double literal */
			printf("%f", root->value.double_value);
			break;
			
		case STRING_LITERAL:		/* print the string literal */
			printf("%s", root->value.string);
			break;
			
		default:
			break;
	}
	
	printf("\n");
	
	/* Recurse on each child of the subtree root, with a depth one
     greater than the root's depth. */
	ast_node child;
	for (child = root->left_child; child != NULL; child = child->right_sibling)
		print_ast(child, depth + 1);
}  
