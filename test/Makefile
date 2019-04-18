CXXFLAGS=	-g -Wall -O3
CFLAGS=		-g -Wall -O2 -Wc++-compat
LIBS=		-lz

ifneq ($(asan),)
	CFLAGS+=-fsanitize=address
	CXXFLAGS+=-fsanitize=address
	LIBS+=-fsanitize=address
endif

all:bedcov-cr bedcov-iitree

extra:all bedcov-itree

bedcov-cr:bedcov-cr.c ../cgranges.c
		$(CC) $(CFLAGS) -I.. $^ $(LIBS) -o $@

bedcov-iitree:bedcov-iitree.cpp
		$(CXX) $(CXXFLAGS) -std=c++98 -I.. -I../cpp $< $(LIBS) -o $@

bedcov-itree:bedcov-itree.cpp
		$(CXX) $(CXXFLAGS) -std=c++11 -I.. $< $(LIBS) -o $@

clean:
		rm -fr *.o a.out *.dSYM bedcov-cr bedcov-itree bedcov-iitree
