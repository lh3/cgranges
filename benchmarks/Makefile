all:bedcov-itree bedcov-iitree

bedcov-itree:bedcov-itree.cpp
		$(CXX) -g -O3 -std=c++11 -I.. -I../examples $< -lz -o $@

bedcov-iitree:bedcov-iitree.cpp
		$(CXX) -g -O3 -std=c++98 -I.. -I../examples -I../cpp $< -lz -o $@

clean:
		rm -fr gmon.out *.o a.out *~ *.a *.dSYM bedcov-itree bedcov-iitree
