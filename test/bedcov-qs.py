#!/usr/bin/env python

from quicksect import IntervalNode, Interval, IntervalTree
from timeit import default_timer as timer
import sys

def main(argv):
	if len(argv) < 3:
		print("Usage: bedcov.py <loaded.bed> <streamed.bed>")
		sys.exit(1)

	bed, i = {}, 0
	start = timer()
	with open(argv[1]) as fp:
		for line in fp:
			t = line[:-1].split("\t")
			if not t[0] in bed:
				bed[t[0]] = IntervalTree()
			bed[t[0]].add(int(t[1]) + 1, int(t[2]))
	sys.stderr.write("Read in {} sec\n".format(timer() - start))
	start = timer()
	with open(argv[2]) as fp:
		for line in fp:
			t = line[:-1].split("\t")
			if not t[0] in bed:
				print("{}\t{}\t{}\t0".format(t[0], t[1], t[2]))
			else:
				r = bed[t[0]].search(int(t[1]) + 1, int(t[2]))
				print("{}\t{}\t{}\t{}".format(t[0], t[1], t[2], len(r)))
	sys.stderr.write("Query in {} sec\n".format(timer() - start))

if __name__ == "__main__":
	main(sys.argv)
