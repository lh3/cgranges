all:tests example

extra:tests example
	cd test && make extra

tests:
	cd test && make 

example:cgranges.c example.c
	$(CC) -O2 $^ -o $@ -lz

clean:
	rm -fr *.o *.dSYM example; cd test && make clean
