/****************************************************/
/* File: tm.c                                       */
/* The TM ("Tiny Machine") computer                 */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
/* added CS48 features by Alex Colvin and Tom Cormen
 *	byte addressing
 *	Instructions:
 *        LDB, STB, INB, OUTB
 *        ADDF, SUBF, MULF, DIVF, CVTIF, CVTFI, LDF, STF
 *        JFLT, JFLE, JFGE, JFGT, JFEQ, JFNE, INF, OUTF, LDFC
 *      .STRING, .INT, .FLOAT directives
 *	srUNALIGNED exception
 *	d[bcf] and r[f] commands
 *      v command for verbosity
 *      b command for breakpoints
 *      e command for extra tracing
 *	startup options
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#ifndef TRUE
#define TRUE !FALSE
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define CS48

/******* const *******/

#define   IADDR_SIZE  64*1024		/* increase for large programs */
#define   DADDR_SIZE  1024*1024		/* increase for large programs */

#define   NO_REGS 8
#define   PC_REG  7

#define   WORD_SIZE	(sizeof (int))		/* a register */
#define   DOUBLE_SIZE	(sizeof (double))	/* floating point */

/******* type  *******/

typedef enum {
   opclRR,     /* reg operands r,s,t */
   opclRM,     /* reg r, mem d+s */
   opclRA,     /* reg r, int d+s */
   opclRF      /* freg r, double df */
   } OPCLASS;

typedef enum {
   /* RR instructions */
   opHALT,    /* RR     halt, operands are ignored */
   opIN,      /* RR     read into reg(r); s and t are ignored */
   opOUT,     /* RR     write from reg(r), s and t are ignored */
   opADD,    /* RR     reg(r) = reg(s)+reg(t) */
   opSUB,    /* RR     reg(r) = reg(s)-reg(t) */
   opMUL,    /* RR     reg(r) = reg(s)*reg(t) */
   opDIV,    /* RR     reg(r) = reg(s)/reg(t) */
#ifdef	CS48
   opINB,      /* RR     read byte into reg(r); s and t are ignored */
   opOUTB,     /* RR     write byte from reg(r), s and t are ignored */
   opCVTIF,   /* RR	freg(r) = reg(s) */
   opINF,     /* RR	read into freg(r) */
   opOUTF,    /* RR	write from freg(r) */
   opADDF,    /* RR	freg(r) = freg(s)+freg(t) */
   opSUBF,    /* RR	freg(r) = freg(s)-freg(t) */
   opMULF,    /* RR	freg(r) = freg(s)*freg(t) */
   opDIVF,    /* RR	freg(r) = freg(s)/freg(t) */
   opCVTFI,   /* RR	reg(r) = freg(s) */
#endif
   opRRLim,   /* limit of RR opcodes */

   /* RM instructions */
   opLD,      /* RM     reg(r) = mem(d+reg(s)...) */
   opST,      /* RM     mem(d+reg(s)...) = reg(r) */
#ifdef	CS48
   opLDB,     /* RM     reg(r) = mem(d+reg(s)) */
   opSTB,     /* RM     mem(d+reg(s)) = reg(r) */
   opLDF,     /* RM     freg(r) = mem(d+reg(s)...) */
   opSTF,     /* RM     mem(d+reg(s)...) = freg(r) */
#endif
   opRMLim,   /* Limit of RM opcodes */

   /* RA instructions */
   opLDA,     /* RA     reg(r) = d+reg(s) */
   opLDC,     /* RA     reg(r) = d ; reg(s) is ignored */
   opJLT,     /* RA     if reg(r)<0 then reg(7) = d+reg(s) */
   opJLE,     /* RA     if reg(r)<=0 then reg(7) = d+reg(s) */
   opJGT,     /* RA     if reg(r)>0 then reg(7) = d+reg(s) */
   opJGE,     /* RA     if reg(r)>=0 then reg(7) = d+reg(s) */
   opJEQ,     /* RA     if reg(r)==0 then reg(7) = d+reg(s) */
   opJNE,     /* RA     if reg(r)!=0 then reg(7) = d+reg(s) */
#ifdef	CS48
   opJFLT,     /* RA     if freg(r)<0 then reg(7) = d+reg(s) */
   opJFLE,     /* RA     if freg(r)<=0 then reg(7) = d+reg(s) */
   opJFGT,     /* RA     if freg(r)>0 then reg(7) = d+reg(s) */
   opJFGE,     /* RA     if freg(r)>=0 then reg(7) = d+reg(s) */
   opJFEQ,     /* RA     if freg(r)==0 then reg(7) = d+reg(s) */
   opJFNE,     /* RA     if freg(r)!=0 then reg(7) = d+reg(s) */
#endif
   opRALim,   /* Limit of RA opcodes */
#ifdef	CS48
   opLDFC,	/* RF    freg(r) = fd */
#endif
   opLim ,   /* Limit of opcodes */
   } OPCODE;

char * opCodeTab[]
        = {"HALT","IN","OUT","ADD","SUB","MUL","DIV",
#ifdef	CS48
	   "INB", "OUTB",
           "CVTIF","INF","OUTF","ADDF","SUBF","MULF","DIVF","CVTFI",
#endif
	   "????",	/* RR opcodes */
           "LD","ST",
#ifdef	CS48
	   "LDB","STB",			/* byte */
	   "LDF","STF",			/* floating */
#endif
	   "????",	/* RM opcodes */
           "LDA","LDC","JLT","JLE","JGT","JGE","JEQ","JNE",
#ifdef	CS48
	   "JFLT","JFLE","JFGT","JFGE","JFEQ","JFNE",
#endif
	   "????",	/* RA opcodes */
#ifdef	CS48
	   "LDFC",
#endif
	   "????"	/* all opcodes */
          };

typedef struct {
      int iop  ;
      int iarg1  ;
      union con { int i; double f; } iarg2  ;	/* constant field */
      int iarg3  ;
   } INSTRUCTION;

#ifdef CS48
typedef enum { STRING, INT, FLOAT } CONST_CLASS;

typedef struct const_info *const_entry;
struct const_info {
  CONST_CLASS type;
  int location;
  union {
    char *string_const;
    int int_const;
    double float_const;
  };
  const_entry next;
};

static const_entry consts = NULL;
#endif

#ifdef	CS48
typedef unsigned char	DATUM;		/* make data byte addressed */
#else
typedef int	DATUM;			/* leave as integer */
#endif
#define WORDMEM(M)	(*(int*)&dMem[M])	/* as int */
#define DOUBLEMEM(M)	(*(double*)&dMem[M])	/* as double */

typedef enum {
   srOKAY,
   srHALT,
   srIMEM_ERR,
   srDMEM_ERR,
   srZERODIVIDE,
   srUNALIGNED
   } STEPRESULT;

char * stepResultTab[]
        = {"OK","Halted","Instruction Memory Fault",
           "Data Memory Fault","Division by 0","unaligned access"
          };

/******** vars ********/
int iloc = 0 ;
int dloc = 0 ;
int traceflag = FALSE;
int extraTraceflag = FALSE;
int icountflag = FALSE;

INSTRUCTION iMem [IADDR_SIZE];
DATUM dMem [DADDR_SIZE];

int reg [NO_REGS];			/* integer regs */
#ifdef	CS48
double freg[NO_REGS];			/* floating regs */
#endif

int verbose = TRUE;			/* iorrhea */

#ifdef CS48
char breakpoint[IADDR_SIZE];
#endif

/********************************************/
int opClass( int c )
{ if      ( c <= opRRLim) return ( opclRR );
  else if ( c <= opRMLim) return ( opclRM );
  else if ( c <= opRALim) return ( opclRA );
  else return ( opclRF );
} /* opClass */

/********************************************/
void writeInstruction ( int loc )
{ printf( "%5d: ", loc) ;
  if ( (loc >= 0) && (loc < IADDR_SIZE) )
  { printf("%6s%3d,", opCodeTab[iMem[loc].iop], iMem[loc].iarg1);
    switch ( opClass(iMem[loc].iop) )
    { case opclRR: printf("%1d,%1d", iMem[loc].iarg2.i, iMem[loc].iarg3);
                   break;
      case opclRM:
      case opclRA: printf("%3d(%1d)", iMem[loc].iarg2.i, iMem[loc].iarg3);
                   break;
      case opclRF: printf("%6g(%1d)",iMem[loc].iarg2.f, iMem[loc].iarg3);
                   break;
    }
    printf ("\n") ;
  }
} /* writeInstruction */

/********************************************/

/* reset memory and registers */
void clear (void)
{ int regNo, loc;
  for (regNo = 0 ; regNo < NO_REGS ; regNo++)
    reg[regNo] = 0;
#ifdef	CS48
  for (regNo = 0 ; regNo < NO_REGS ; regNo++)
    freg[regNo] = 0.0;
#endif
  for (loc = 0 ; loc < DADDR_SIZE ; loc++)
      dMem[loc] = 0 ;
  WORDMEM(0) = DADDR_SIZE - 1 ;		/* save size--1 as first word */
}

/* clear out the instruction memory 
 */
void
clearInstructions()
{ int loc;
  for (loc = 0 ; loc < IADDR_SIZE ; loc++)
  { iMem[loc].iop = opHALT ;
    iMem[loc].iarg1 = 0 ;
    iMem[loc].iarg2.i = 0 ;
    iMem[loc].iarg3 = 0 ;
  }
}

/********************************************/
/* input scanners
 * transmogrified from the pascal
 */
#define   LINESIZE  256
#define   TOKENSIZE  LINESIZE

int  inNo;				/* input line count */
char inLine[LINESIZE];			/* input line */
int  inLen;				/* input length */
int  inCol;				/* input column */

#define inCh()	inLine[inCol]		/* current character */

/* read a line at a time
 * return true if any read
 */
int
getLine(FILE *src)
{ inNo++;				/* count lines */
  inCol = 0;
  if (!fgets(inLine,LINESIZE,src))	/* line-oriented io */
  { inLine[inLen = 0] = '\0';
    return FALSE;
  }
  inLen = strlen(inLine);
  if (0<inLen && inLine[inLen-1]=='\n')	/* squish EOL */
    inLine[--inLen] = '\0' ;
  return TRUE;
}    

/* interactive input
 * return true if any read
 */
int
getUser(char *prompt)
{ if (prompt)
    printf("%s", prompt);
  return getLine(stdin);
}

/* skip blanks */
void
getPrint (void)
{ while (inCol<inLen && !isgraph(inCh()))
    inCol++;
}

/* get a specific character */
int
getPast(char c)
{ getPrint();
  if (inCh()!=c)
    return FALSE;
  inCol++;
  return TRUE;
}

/* see if at EOL */
int
getEOL(void)
{ getPrint();
  return inCol==inLen;
}

/* grab the next input token 
 * returns length
 */
int
getToke(char token[TOKENSIZE])
{ int len;
  getPrint();				/* skip leading space */
  for (len = 0; len<TOKENSIZE-1; inCol++)
  { char c = inCh();
    if (c && isgraph(c) && !strchr(":,()",c))	/* alphanumeric or numbers */
      token[len++] = c;
    else
      break;
  }
  token[len] = '\0';
  return len!=0;			/* got any? */
}

/* get an integer iff there is one */
int
getInt(int *p)
{ char toke[TOKENSIZE];			/* number text */
  int n;
  getToke(toke);
  if (sscanf(toke,"%i",&n)!=1)		/* any int format */
    return FALSE;
  *p = n;
  return TRUE;
}

/* get a float(double) iff there is one */
int
getFlt(double *p)
{ char toke[TOKENSIZE];			/* number text */
  double d;
  getToke(toke);
  if (sscanf(toke,"%lg",&d)!=1)		/* float format */
    return FALSE;
  *p = d;
  return TRUE;
}


/********************************************/
/* get a register number */
int
getReg(int *r)
{ return getInt(r) && 0<=*r && *r<NO_REGS;
}

/* report syntax error & return failure status */
int
InstError(char *msg,int instNo)
{ printf("Line %d",inNo);
  if (instNo >= 0)
    printf(" (Instruction %d)",instNo);
  printf(": %s\n",msg);
  return FALSE;
}

#ifdef CS48

/* Process an int declaration.  It has the form
   .INT (starting in column 0)
   N (location in memory where the int should start)
   K (an integer)
   Whitespace separates .STRING, N, and K.
*/
int processInt() {
  char token[TOKENSIZE];
  int loc, value;
  const_entry new_int;

  getToke(token);		/* consume .INT */

  if (!getInt(&loc))
    return InstError("Missing location", -1);

  if (!getInt(&value))
    return InstError("Missing int value", -1);

  /* At this point, we have a location and an int to store.  If the
     location is good, store the int in the list of ints to be
     loaded. */
  if (loc < 0 || loc + WORD_SIZE > DADDR_SIZE)
    return InstError("Integer does not reside entirely in memory", -1);
  else if (loc % WORD_SIZE != 0)
    return InstError("Integer does align properly", -1);
  else {
    new_int = malloc(sizeof(struct const_info));
    new_int->type = INT;
    new_int->location = loc;
    new_int->int_const = value;
    new_int->next = consts;
    consts = new_int;
    return TRUE;
  }
}

/* Process a float declaration.  It has the form
   .FLOAT (starting in column 0)
   N (location in memory where the int should start)
   X (a double)
   Whitespace separates .STRING, N, and X.
*/
int processFlt() {
  char token[TOKENSIZE];
  int loc;
  double value;
  const_entry new_float;

  getToke(token);		/* consume .INT */

  if (!getInt(&loc))
    return InstError("Missing location", -1);

  if (!getFlt(&value))
    return InstError("Missing double value", -1);

  /* At this point, we have a location and a double to store.  If the
     location is good, store the double in the list of constants to be
     loaded. */
  if (loc < 0 || loc + DOUBLE_SIZE > DADDR_SIZE)
    return InstError("Float does not reside entirely in memory", -1);
  else if (loc % DOUBLE_SIZE != 0)
    return InstError("Float does align properly", -1);
  else {
    new_float = malloc(sizeof(struct const_info));
    new_float->type = FLOAT;
    new_float->location = loc;
    new_float->float_const = value;
    new_float->next = consts;
    consts = new_float;
    return TRUE;
  }
}

/* Process a string declaration.  It has the form
   .STRING (starting in column 0)
   N (location in memory where the string should start)
   CHARS (characters in the string, delimited by double-quotes, with \"
          as an escape character for a double-quote)
   Whitespace separates .STRING, N, and CHARS.
*/
int
processString() {
  char token[TOKENSIZE];
  int starting_loc, string_start, orig_string_len, real_string_len;
  int saw_escape = FALSE;
  const_entry new_string;
  int i;
  char *c;

  getToke(token);  /* consume .STRING */

  if (!getInt(&starting_loc))
    return InstError("Missing location", -1);

  /* Find the start of the string. */
  do
    inCol++;
  while (inCol < LINESIZE && inLine[inCol] != '\"' && inLine[inCol] != '\0');

  if (inCol >= LINESIZE - 1 || inLine[inCol] == '\0') /* subtract 1 because need room for closing quote */
    return InstError("Missing string", -1);

  string_start = inCol + 1;  /* where the string starts */
  for (inCol = inCol + 1;
       inCol < LINESIZE
	 && (saw_escape || inLine[inCol] != '\"');
       inCol++)
    saw_escape = inLine[inCol] == '\\';

  if (inCol >= LINESIZE)
    return InstError("Bad string", -1);

  /* At this point, we have a good string.  Add it to the list of constants
     if it fits in memory. */
  orig_string_len = inCol - string_start;
  new_string = malloc(sizeof(struct const_info));
  new_string->type = STRING;
  new_string->location = starting_loc;
  new_string->next = consts;
  new_string->string_const = malloc(orig_string_len + 1);
  saw_escape = FALSE;
  for (i = 0, c = inLine + string_start, real_string_len = 0;
       i < orig_string_len;
       i++, c++) {
    if (saw_escape) {
      switch (*c) {
      case 'a':
	new_string->string_const[real_string_len] = '\a';
	break;

      case 'b':
	new_string->string_const[real_string_len] = '\b';
	break;

      case 'f':
	new_string->string_const[real_string_len] = '\f';
	break;

      case 'n':
	new_string->string_const[real_string_len] = '\n';
	break;

      case 'r':
	new_string->string_const[real_string_len] = '\r';
	break;

      case 't':
	new_string->string_const[real_string_len] = '\t';
	break;

      case 'v':
	new_string->string_const[real_string_len] = '\v';
	break;

      case '0':  /* but why would you put \0 in the middle of a string?? */
	new_string->string_const[real_string_len] = '\0';
	break;

      default:
	new_string->string_const[real_string_len] = *c;
	break;
      }
    }
    else
      new_string->string_const[real_string_len] = *c;

    saw_escape = (!saw_escape) && *c == '\\';
    if (!saw_escape)
      real_string_len++;
  }

  new_string->string_const[real_string_len] = '\0';

  if (starting_loc < 0 || starting_loc + real_string_len >= DADDR_SIZE) {
    free(new_string->string_const);
    free(new_string);
    return InstError("String does not reside entirely in memory", -1);
  }

  consts = new_string;

  return TRUE;
}

/* Load the constants into memory. */
void
loadConsts() {
  const_entry s;
  for (s = consts; s != NULL; s = s->next) {
    switch (s->type) {
    case STRING:
      strcpy(dMem + s->location, s->string_const);
      break;

    case INT:
      WORDMEM(s->location) = s->int_const;
      break;

    case FLOAT:
      DOUBLEMEM(s->location) = s->float_const;
      break;

    default:
      break;
    }
  }
}
#endif

/* read in assembly language for one instruction
 */
int
getInst()
{ char opname[TOKENSIZE];
  OPCODE op;
  int loc;
  int arg1, arg3;
  union con arg2;			/* constant or reg */

  if (getEOL())				/* ignore empty line */
    return TRUE;
  if (getPast('*'))			/* ignore comment line */
    return TRUE;  

#ifdef CS48
  getPrint();  /* skip leading whitespace */
  if (strncmp(inLine + inCol, ".STRING", 7) == 0)
    return processString();
  else if (strncmp(inLine + inCol, ".INT", 4) == 0)
    return processInt();
  else if (strncmp(inLine + inCol, ".FLOAT", 6) == 0)
    return processFlt();
#endif

  if (!getInt(&loc))			/* find loc */
    return InstError("Bad location",-1);
  if (loc<0 || IADDR_SIZE <=loc)
    return InstError("Location out of range",loc);
  if (!getPast(':'))
    return InstError("Missing colon",loc);

  /* get op & look up in op list */
  if (!getToke(opname))
    return InstError("Missing opcode",loc);
  for (op = opHALT; op < opLim; op++)
  { if (strcmp(opCodeTab[op],opname)==0)
      break;
  }
  if (op==opLim)
      return InstError("Illegal opcode",loc);

  /* all instructions take a first regiser */
  if (!getReg(&arg1))
    return InstError("Bad first register",loc);
  if (!getPast(','))
      return InstError("Missing comma", loc);

  /* do the opcodes by class */
  switch (opClass(op))
  {
  case opclRR:
    if (!getReg(&arg2.i))
        return InstError("Bad second register", loc);
    if (!getPast(',')) 
        return InstError("Missing comma",loc);
    if (!getReg(&arg3))
        return InstError("Bad third register",loc);
    break;

  case opclRM:
  case opclRA:
    if (!getInt(&arg2.i))
        return InstError("Bad displacement",loc);
    if (!getPast('('))
        return InstError("Missing '('",loc);
    if (!getReg(&arg3))
        return InstError("Bad second register",loc);
    if (!getPast(')'))
        return InstError("Missing ')'",loc);
    break;

    case opclRF:
    if (!getFlt(&arg2.f))
        return InstError("Bad floating point number",loc);
  }

  /* store the instruction */
  iMem[loc].iop = op;
  iMem[loc].iarg1 = arg1;
  iMem[loc].iarg2 = arg2;
  iMem[loc].iarg3 = arg3;
  return TRUE;
}

/* read in assembly language and load the instruction memory
 */
int
readInstructions(char *src)
{ FILE *pgm = NULL;
#ifdef CS48
  /* make 'em spell it out */
#else
  char name[LINESIZE];
  if (!strchr(src,'.') && strlen(src)<255-3)	/* supply suffix? */
  { strcpy(name,src);
    strcat(name,".tm");
    src = name;				/* use with suffix */
  }
#endif
  pgm = fopen(src,"r");
  if (pgm)
  { inNo = 0;				/* reset line count */
    if (verbose)
      printf("assembling %s\n",src);
    while (!feof(pgm))			/* assemble file */
    { getLine(pgm);
      if (!getInst())
	return FALSE;
    }
    fclose(pgm);
    return TRUE;
  }
  
  printf("%s not found\n",src);
  return FALSE;
}


/********************************************/

STEPRESULT stepTM (void)
{ INSTRUCTION currentinstruction  ;
  int pc  ;
  int r,s,t,m  ;
  double df;
  
  pc = reg[PC_REG] ;
  if ( (pc < 0) || (pc > IADDR_SIZE)  )
      return srIMEM_ERR ;
  reg[PC_REG] = pc + 1 ;
  currentinstruction = iMem[ pc ] ;
  switch (opClass(currentinstruction.iop) )
  { case opclRR :
    /***********************************/
      r = currentinstruction.iarg1 ;
      s = currentinstruction.iarg2.i ;
      t = currentinstruction.iarg3 ;
      break;

    case opclRM :
    /***********************************/
      r = currentinstruction.iarg1 ;
      s = currentinstruction.iarg3 ;
      m = currentinstruction.iarg2.i + reg[s] ;
      if ( (m < 0) || (m > DADDR_SIZE))
         return srDMEM_ERR ;
      break;

    case opclRA :
    /***********************************/
      r = currentinstruction.iarg1 ;
      s = currentinstruction.iarg3 ;
      m = currentinstruction.iarg2.i + reg[s] ;
      break;

    case opclRF:
    /***********************************/
      r = currentinstruction.iarg1 ;
      df = currentinstruction.iarg2.f ;
      break;
  } /* case */

  switch ( currentinstruction.iop)
  { /* RR instructions */
    case opHALT :
    /***********************************/
      printf("\nHALT at location %d\n", pc);
      return srHALT ;
      /* break; */

    case opIN :
    /***********************************/
      for (;;) 
      { getUser(verbose ? "Enter value for IN instruction: " : NULL);
        if (getInt(&reg[r]))
	  break;
	printf ("Illegal value\n");
      }
      break;

    case opOUT :  
      printf ((verbose ? "OUT instruction prints: %d\n" : "%d"), reg[r] ) ;
      break;

    case opADD :  reg[r] = reg[s] + reg[t] ;  break;
    case opSUB :  reg[r] = reg[s] - reg[t] ;  break;
    case opMUL :  reg[r] = reg[s] * reg[t] ;  break;

    case opDIV :
    /***********************************/
      if ( reg[t] != 0 ) reg[r] = reg[s] / reg[t];
      else return srZERODIVIDE ;
      break;

#ifdef CS48
    case opINB:	
      if (verbose)
      { getUser("Enter character for INB instruction: ");
        reg[r] = inCh();
      }
      else				/* try to allow real I/O */
        reg[r] = getchar();
      break;

    case opINF:
      for (;;)
      { getUser(verbose ? "Enter value for INF instruction: " : NULL);
        if (getFlt(&freg[r]))
	  break;
	printf ("Illegal value\n");
       }
      break;

    case opOUTB :  
      printf ((verbose ? "OUTB instruction prints: %c\n" : "%c"), reg[r] ) ;
    /* floating point */
    case opCVTIF : freg[r] = reg[s]; break;
    case opOUTF :  
      printf ((verbose ? "OUTF instruction prints: %g\n" : "%g"), freg[r] ) ;
      break;
    case opADDF : freg[r] = freg[s] + freg[t]; break;
    case opSUBF : freg[r] = freg[s] - freg[t]; break;
    case opMULF : freg[r] = freg[s] * freg[t]; break;
    case opDIVF : if (freg[t] == 0) return srZERODIVIDE;
                  freg[r] = freg[s] / freg[t];
		  break;
    case opCVTFI : reg[r] = freg[s]; break;

    case opLDFC : freg[r] = df; break;
#endif

    /*************** RM instructions ********************/
#ifdef	CS48
    case opLDB :   reg[r] = (unsigned char)dMem[m] ;  break; /* 0-fill */
    case opSTB :   dMem[m] = reg[r] ;  break;
    case opLDF :   if (m&(DOUBLE_SIZE-1)) return srUNALIGNED;
    		   freg[r] = DOUBLEMEM(m);
		   break;
    case opSTF :    if (m&(DOUBLE_SIZE-1)) return srUNALIGNED;
    		   DOUBLEMEM(m) = freg[r];
		   break;
#endif
    case opLD :	   if (m&(WORD_SIZE-1)) return srUNALIGNED;
    		   reg[r] = WORDMEM(m);
		   break;
    case opST :    if (m&(WORD_SIZE-1)) return srUNALIGNED;
    		   WORDMEM(m) = reg[r];
		   break;

    /*************** RA instructions ********************/
    case opLDA :    reg[r] = m ; break;
    case opLDC :    reg[r] = currentinstruction.iarg2.i ;   break;
    case opJLT :    if ( reg[r] <  0 ) reg[PC_REG] = m ; break;
    case opJLE :    if ( reg[r] <=  0 ) reg[PC_REG] = m ; break;
    case opJGT :    if ( reg[r] >  0 ) reg[PC_REG] = m ; break;
    case opJGE :    if ( reg[r] >=  0 ) reg[PC_REG] = m ; break;
    case opJEQ :    if ( reg[r] == 0 ) reg[PC_REG] = m ; break;
    case opJNE :    if ( reg[r] != 0 ) reg[PC_REG] = m ; break;
#ifdef	CS48
    case opJFLT :    if ( freg[r] <  0.0 ) reg[PC_REG] = m ; break;
    case opJFLE :    if ( freg[r] <=  0.0 ) reg[PC_REG] = m ; break;
    case opJFGT :    if ( freg[r] >  0.0 ) reg[PC_REG] = m ; break;
    case opJFGE :    if ( freg[r] >=  0.0 ) reg[PC_REG] = m ; break;
    case opJFEQ :    if ( freg[r] == 0.0 ) reg[PC_REG] = m ; break;
    case opJFNE :    if ( freg[r] != 0.0 ) reg[PC_REG] = m ; break;
#endif

    /* end of legal instructions */
  } /* case */
  return srOKAY ;
} /* stepTM */

#ifdef CS48
int hitBreakpoint = FALSE;

int
defineBreakpoint(int i, char val) {
  if (i >= 0 && i < IADDR_SIZE) {
    breakpoint[i] = val;
    return 1;
  }
  else {
    printf("Location %d is an invalid instruction address.\n", i);
    return 0;
  }
}

int
setBreakpoint(int i) {
  return defineBreakpoint(i, 1);
}

int
clearBreakpoint(int i) {
  return defineBreakpoint(i, 0);
}

void
clearAllBreakpoints(void) {
  int i;
  for (i = 0; i < IADDR_SIZE; i++)
    clearBreakpoint(i);
}

int
isBreakpointSet(int i) {
  if (i >= 0 && i < IADDR_SIZE)
    return breakpoint[i];
  else {
    printf("Location %d is an invalid instruction address.\n", i);
    return 0;
  }
}

void
showAllBreakpoints(void) {
  int i;
  for (i = 0; i < IADDR_SIZE; i++)
    if (isBreakpointSet(i))
      writeInstruction(i);
}
#endif

void
displayIntRegs(void) {
  int i;
  for (i = 0; i < NO_REGS; i++)
    { printf("r%1d: %d    ", i,reg[i]);
      if ( (i % 4) == 3 ) printf ("\n");
    }
}

#ifdef CS48
void
displayFloatRegs(void) {
  int i;
  for (i = 0; i < NO_REGS; i++)
    { printf("f%1d: %g    ", i,freg[i]);
      if ( (i % 2) == 1 ) printf ("\n");
    }
}
#endif

/********************************************/
char *toggle[2] = {"off","on"};
char command[TOKENSIZE] = "h";			/* save last command */
  
int doCommand (void)
{ int stepcnt=0;
  int printcnt;
  int stepResult;
#ifdef CS48
  int breakpointNumber, settingBreakpoint, clearAll;
  char breakpointToken[TOKENSIZE];
#endif
  
  if (!getUser("Enter command: "))
    return FALSE;				/* no user */
  
  if (!getEOL())				/* default to last command */
      getToke(command);

  switch ( command[0] )
  { case 'h' :
    /***********************************/
      printf("Commands are:\n");
      printf("   s(tep <n>      "\
             "Execute n (default 1) TM instructions\n");
      printf("   g(o            "\
             "Execute TM instructions until HALT\n");
#ifdef CS48
      printf("   r(egs          "\
             "Print the contents of the integer registers\n");
#else
      printf("   r(egs          "\
             "Print the contents of the registers\n");
#endif
#ifdef CS48
      printf("   f(loat         "\
	     "Print the contents of the floating-point registers\n");
#endif
      printf("   i(Mem <b <n>>  "\
             "Print n iMem locations starting at b\n");
#ifdef CS48
      printf("   d(Mem <b <n>>  "\
	     "Print n dMem locations starting at b as integers\n");
      printf("   df(Mem <b <n>> "\
	     "Print n dMem locations starting at b as floating-point numbers\n");
      printf("   db(Mem <b <n>> "\
	     "Print n dMem locations starting at b as bytes\n");
      printf("   dc(Mem <b <n>> "\
	     "Print n dMem locations starting at b as characters\n");
#else
      printf("   d(Mem <b <n>>  "\
             "Print n dMem locations starting at b\n");
#endif
      printf("   t(race         "\
             "Toggle instruction trace\n");
#ifdef CS48
      printf("   e(xtra trace   "\
	     "Toggle extra trace, showing all instructions and registers\n");
      printf("   b(reak         "\
	     "Show all breakpoints\n");
      printf("   b(reak <n1> <n2> ... "\
	     "Sets or clears breakpoints.\n"
	     "                  If an argument is -n, where n is a nonnegative integer,\n"
	     "                  clears breakpoint at location n\n"
	     "                  If an argument is n, where n is a nonnegative integer,\n"
	     "                  sets breakpoint at location n\n"
	     "                  If an argument is the letter x, clears all breakpoints\n");
#endif
      printf("   p(rint         "\
             "Toggle print of total instructions executed"\
             " ('go' only)\n");
      printf("   v(erbose       "\
             "Toggle prompts for input\n");
      printf("   c(lear         "\
             "Reset simulator for new execution of program\n");
      printf("   h(elp          "\
             "Cause this list of commands to be printed\n");
      printf("   q(uit          "\
             "Terminate the simulation\n");
      
      break;

    case 't' :
    /***********************************/
      traceflag = ! traceflag ;
      printf("Tracing now %s\n",toggle[traceflag]);
      break;

#ifdef CS48
    case 'e' :
      extraTraceflag = ! extraTraceflag;
      printf("Extra tracing now %s\n",toggle[extraTraceflag]);
      break;
#endif

    case 'p' :
    /***********************************/
      icountflag = ! icountflag ;
      printf("Printing instruction count now %s\n",toggle[icountflag]);
      break;

    case 'v' :
      verbose = ! verbose;
      printf("Verbose prompts now %s\n",toggle[verbose]);
      break;

    case 's' :
    /***********************************/
      stepcnt = 1;
      if (!getEOL() && !getInt(&stepcnt))
        printf("Step count?\n");
      break;

#ifdef CS48
    case 'b':
      if (!getToke(breakpointToken))
	showAllBreakpoints();
      else {
	do {
	  breakpointNumber = -1;
	  settingBreakpoint = TRUE;
	  clearAll = FALSE;
	  if (breakpointToken[0] == '-')
	    settingBreakpoint = FALSE;
	  else if (breakpointToken[0] == 'x') {
	    clearAll = TRUE;
	    settingBreakpoint = FALSE;
	  }
	  if (!clearAll) {
	    sscanf(breakpointToken, "%i", &breakpointNumber);
	    breakpointNumber = abs(breakpointNumber);
	  }
	  if (settingBreakpoint) {
	    if (isBreakpointSet(breakpointNumber))
	      printf("Breakpoint already set at location %d\n",
		     breakpointNumber);
	    else {
	      if (setBreakpoint(breakpointNumber)) {
		printf("Set breakpoint at ");
		writeInstruction(breakpointNumber);
	      }
	    }
	  }
	  else if (clearAll) {
	    clearAllBreakpoints();
	    printf("Cleared all breakpoints\n");
	  }
	  else {
	    if (isBreakpointSet(breakpointNumber)) {
	      if (clearBreakpoint(breakpointNumber)) {
		printf("Cleared breakpoint at ");
		writeInstruction(breakpointNumber);
	      }
	    }
	    else
	      printf("No breakpoint set at location %d\n", breakpointNumber);
	  }
	}
	while (getToke(breakpointToken));
      }
      break;
#endif

    case 'g' :   stepcnt = 1 ;     break;

    case 'r' :
      displayIntRegs();
      break;

    /***********************************/
#ifdef	CS48
    case 'f':
      displayFloatRegs();
      break;
#endif

    case 'i' :
    /***********************************/
      printcnt = 1 ;
      if (getInt(&iloc))
        getInt(&printcnt);
      if (!getEOL())
        printf ("Instruction locations?\n");
      else
      { while ((iloc >= 0) && (iloc < IADDR_SIZE)
                && (printcnt > 0) )
        { writeInstruction(iloc);
          iloc++ ;
          printcnt-- ;
        }
      }
      break;

    case 'd' :
    /***********************************/
      printcnt = 1 ;
      if (getInt(&dloc))
      getInt(&printcnt);
      if (!getEOL())
        printf("Data locations?\n");
#ifdef CS48
      else if (command[1] == 'f')
      { dloc = (dloc|(DOUBLE_SIZE-1))-(DOUBLE_SIZE-1);	/* align */
        while ((dloc >= 0) && (dloc < DADDR_SIZE)
                  && (printcnt > 0))
        { printf("%5d: %g\n",dloc,DOUBLEMEM(dloc));
          dloc += DOUBLE_SIZE;
          printcnt--;
        }
      }
      else if (command[1] == 'b')  /* print bytes */
      { while ((dloc >= 0) && (dloc < DADDR_SIZE)
                  && (printcnt > 0))
        { printf("%5d: %5d\n",dloc,dMem[dloc]);
          dloc++;
          printcnt--;
        }
      }
      else if (command[1] == 'c')  /* print chars */
      { while ((dloc >= 0) && (dloc < DADDR_SIZE)
                  && (printcnt > 0))
        { printf("%5d: %c\n",dloc,dMem[dloc]);
          dloc++;
          printcnt--;
        }
      }
#endif
      else
      { dloc = (dloc|(WORD_SIZE-1))-(WORD_SIZE-1);	/* align */
        while ((dloc >= 0) && (dloc < DADDR_SIZE)
                  && (printcnt > 0))
        { printf("%5d: %5d\n",dloc,WORDMEM(dloc));
          dloc += WORD_SIZE;
          printcnt--;
        }
      }
      break;

    case 'c' :
    /***********************************/
      iloc = 0;
      dloc = 0;
      stepcnt = 0;
      clear();
#ifdef CS48
      loadConsts();
#endif
      break;

    case 'q' : return FALSE;  /* break; */

    default : printf("Command %s unknown.\n", command); break;
    
  }  /* case */

  stepResult = srOKAY;
  if ( stepcnt > 0 )
  { if ( command[0] == 'g' )
    { stepcnt = 0;
#ifdef CS48
      while (stepResult == srOKAY
	     && (hitBreakpoint 
		 || (!hitBreakpoint && !isBreakpointSet(reg[PC_REG]))))
#else
      while (stepResult == srOKAY)
#endif
      { 
#ifdef CS48
	hitBreakpoint = FALSE;
#endif
	iloc = reg[PC_REG] ;
#ifdef CS48
	if (extraTraceflag) {
	  writeInstruction(iloc);
	  displayIntRegs();
	  displayFloatRegs();
	}
	else
#endif
        if ( traceflag ) writeInstruction( iloc ) ;
        stepResult = stepTM ();
        stepcnt++;
      }
#ifdef CS48
      if (stepResult == srOKAY && isBreakpointSet(reg[PC_REG])) {
	hitBreakpoint = TRUE;
	printf("Hit breakpoint at ");
	writeInstruction(reg[PC_REG]);
      }
#endif
      if ( icountflag )
        printf("Number of instructions executed = %d\n",stepcnt);
    }
    else
    { 
#ifdef CS48
      while (stepcnt > 0 && stepResult == srOKAY
	     && (hitBreakpoint 
		 || (!hitBreakpoint && !isBreakpointSet(reg[PC_REG]))))
#else
      while (stepcnt > 0 && stepResult == srOKAY)
#endif
      { 
#ifdef CS48
	hitBreakpoint = FALSE;
#endif
	iloc = reg[PC_REG] ;
#ifdef CS48
	if (extraTraceflag) {
	  writeInstruction(iloc);
	  displayIntRegs();
	  displayFloatRegs();
	}
	else
#endif
        if ( traceflag ) writeInstruction( iloc ) ;
        stepResult = stepTM ();
        stepcnt-- ;
      }
#ifdef CS48
      if (stepResult == srOKAY && isBreakpointSet(reg[PC_REG])) {
	hitBreakpoint = TRUE;
	printf("Hit breakpoint at ");
	writeInstruction(reg[PC_REG]);
      }
#endif
    }
    printf( "%s\n",stepResultTab[stepResult] );
  }
  return TRUE;
} /* doCommand */

/********************************************/
/* E X E C U T I O N   B E G I N S   H E R E */
/********************************************/

/* load files named on the command line
 * also options -q|-v
 */

int
main( int argc, char * argv[] )
{ int a;
  clear();				/* reset the machine */
  clearInstructions();			/* erase the ROM */
#ifdef CS48
  clearAllBreakpoints();
#endif

  for (a = 1; a<argc; ++a)		/* scan args */
  { char *arg = argv[a];

    if (arg[0] != '-')			/* not option */
      readInstructions(arg);		/* file to load */
    else if (arg[1] == 'q')		/* nonverbose */
      verbose = FALSE;
    else if (arg[1] == 'v')		/* verbose */
      verbose = TRUE;
  }

  /* switch input file to terminal */
  /* reset( input ); */
  /* read-eval-print */
#ifdef	CS48
  printf("TM48 simulation (enter h for help)...\n");
#else
  printf("TM  simulation (enter h for help)...\n");
#endif

#ifdef CS48
  loadConsts();		  /* load strings and constants into memory */
#endif

  while (doCommand ())
    ;
  printf("Simulation done.\n");

  return 0;
}
