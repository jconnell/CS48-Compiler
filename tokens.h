/*
 * tokens.h
 * 
 * CS 48 - Project Part 4
 * Jonathan Connell & David Kopec
 *
 */

#define IF			258
#define ELSE		259
#define READ		260
#define	WRITE		261
#define BANG		262
#define EQUALS		263
#define NOTEQUALS	264
#define GREATER		265
#define GREATEREQ	266
#define LESS		267
#define LESSEQ		268
#define DO			269
#define WHILE		270
#define FOR			271
#define	LPAREN		272
#define	RPAREN		273
#define	LBRACKET	274
#define	RBRACKET	275
#define AND			276
#define OR			277
#define SEMI		278
#define	INT			279
#define DOUBLE		280
#define CHAR		281
#define SWITCH		282
#define CASE		283
#define CONST		284
#define	CONTINUE    285
#define	DEFAULT		286
#define RETURN		287
#define SIZEOF		288
#define STATIC		289
#define VOID		290
#define BREAK		291
#define INCR		292
#define DECR		293
#define COMMENT		294
#define	NUM			295
#define	ID			296

#define MAXTOKENLEN 100

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#ifndef TRUE 1
#endif

typedef int TokenType;

extern FILE* source;	/* Source code text file */
extern FILE* listing;	/* Listing output text file */

extern int TraceScan;	/* TRUE causes token info to be printed to the 
						 listing file as each token is recognized by scanner.*/
extern int lineno;		/* Source file's current line number */

