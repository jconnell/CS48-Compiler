CC = gcc
CFLAGS =

.SUFFIXES: .c

SRCS = Symtab.c TestSymtab.c
OBJS = $(SRCS:.c=.o)

.c.o:
	$(CC) -c $(CFLAGS) $<

symtab: $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) symtab

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.

Symtab.o: /usr/include/stdlib.h /usr/include/features.h
Symtab.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
Symtab.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
Symtab.o: /usr/include/sys/types.h /usr/include/bits/types.h
Symtab.o: /usr/include/bits/typesizes.h /usr/include/time.h
Symtab.o: /usr/include/endian.h /usr/include/bits/endian.h
Symtab.o: /usr/include/sys/select.h /usr/include/bits/select.h
Symtab.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Symtab.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Symtab.o: /usr/include/alloca.h /usr/include/string.h /usr/include/stdio.h
Symtab.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
Symtab.o: /usr/include/bits/wchar.h /usr/include/gconv.h
Symtab.o: /usr/include/bits/stdio_lim.h /usr/include/bits/sys_errlist.h
Symtab.o: Symtab.h
TestSymtab.o: /usr/include/stdio.h /usr/include/features.h
TestSymtab.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
TestSymtab.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
TestSymtab.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
TestSymtab.o: /usr/include/libio.h /usr/include/_G_config.h
TestSymtab.o: /usr/include/wchar.h /usr/include/bits/wchar.h
TestSymtab.o: /usr/include/gconv.h /usr/include/bits/stdio_lim.h
TestSymtab.o: /usr/include/bits/sys_errlist.h Symtab.h
