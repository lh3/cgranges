CFLAGS=		-g -Wall -O2 -Wc++-compat #-Wextra
LIBS=		-lz

ifneq ($(asan),)
	CFLAGS+=-fsanitize=address
	LIBS+=-fsanitize=address
endif

.PHONY:clean depend
.SUFFIXES:.c .o

.c.o:
		$(CC) -c $(CFLAGS) $< -o $@

all:bedcov examples/example

bedcov:cgranges.o examples/bedcov.o
		$(CC) -o $@ $^ $(LIBS)

examples/example:cgranges.o examples/example.o
		$(CC) -o $@ $^ $(LIBS)

examples/bedcov.o:examples/bedcov.c
		$(CC) -c $(CFLAGS) -I. -Iexamples $< -o $@

examples/example.o:examples/example.c
		$(CC) -c $(CFLAGS) -I. -Iexamples $< -o $@

clean:
		rm -fr gmon.out *.o examples/*.o a.out *.dSYM bedcov

depend:
		(LC_ALL=C; export LC_ALL; makedepend -Y -- $(CFLAGS) $(CPPFLAGS) -- *.c examples/*.c)

# DO NOT DELETE

cgranges.o: cgranges.h khash.h
examples/bedcov.o: cgranges.h examples/kseq.h
examples/example.o: cgranges.h
