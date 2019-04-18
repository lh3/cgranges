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

bedcov:cgranges.o examples/bedcov.o
		$(CC) -o $@ $^ $(LIBS)

examples/bedcov.o:examples/bedcov.c
		$(CC) -c $(CFLAGS) -I. -Iexamples $< -o $@

clean:
		rm -fr gmon.out *.o examples/*.o a.out *.dSYM bedcov

depend:
		(LC_ALL=C; export LC_ALL; makedepend -Y -- $(CFLAGS) $(CPPFLAGS) -- *.c examples/*.c)

# DO NOT DELETE
