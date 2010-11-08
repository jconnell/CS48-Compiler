CC = gcc
CFLAGS =

.SUFFIXES: .c

SRCS = Symtab.c ast.c quads.c assemblygen.c
OBJS = $(SRCS:.c=.o)

.c.o:
	$(CC) -c $(CFLAGS) $<

quads: $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) symtab

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
Symtab.o: Symtab.h
ast.o: ast.h
assemblygen.o: assemblygen.h 
quads.o: quads.h ast.h Symtab.h assemblygen.h

