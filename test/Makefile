CXXFLAGS=	-g -Wall -O3
CFLAGS=		-g -Wall -O2 -Wc++-compat
LIBS=		-lz
EXE=		bedcov-cr bedcov-itree bedcov-iitree bedcov-iitree-bfs

ifneq ($(asan),)
	CFLAGS+=-fsanitize=address
	CXXFLAGS+=-fsanitize=address
	LIBS+=-fsanitize=address
endif

all:$(EXE)

extra:all bedcov-itree

bedcov-cr:bedcov-cr.c ../cgranges.c
		$(CC) $(CFLAGS) -I.. $^ $(LIBS) -o $@

bedcov-iitree:bedcov-iitree.cpp ../cpp/IITree.h
		$(CXX) $(CXXFLAGS) -std=c++98 -I.. -I../cpp $< $(LIBS) -o $@

bedcov-iitree-bfs:bedcov-iitree.cpp ../cpp/IITreeBFS.h
		$(CXX) $(CXXFLAGS) -D_USE_BFS -std=c++98 -I.. -I../cpp $< $(LIBS) -o $@

bedcov-itree:bedcov-itree.cpp
		$(CXX) $(CXXFLAGS) -std=c++11 -I.. $< $(LIBS) -o $@

clean:
		rm -fr *.o a.out *.dSYM $(EXE)
