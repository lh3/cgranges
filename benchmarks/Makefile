CFLAGS=		-g -Wall -O3 -I..
CXXFLAGS=	$(CFLAGS) -std=c++11
LIBS=		-lz

bedcov-itree:it-ranges.o ../examples/bedcov.o
		$(CXX) -o $@ $^ $(LIBS)

bedcov.o:../examples/bedcov.c
		$(CC) -c $(CFLAGS) -I.. -I../examples $< -o $@

it-ranges.o:it-ranges.cpp
		$(CXX) -c $(CXXFLAGS) -I.. -I../examples $< -o $@

clean:
		rm -fr gmon.out *.o a.out *~ *.a *.dSYM bedcov-itree
