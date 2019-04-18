#!/usr/bin/env python

import cgranges as cr
from timeit import default_timer as timer
import sys

def main(argv):
	if len(argv) < 3:
		print("Usage: bedcov.py <loaded.bed> <streamed.bed>")
		sys.exit(1)

	bed = cr.cgranges()
	start = timer()
	with open(argv[1]) as fp:
		for line in fp:
			t = line[:-1].split("\t")
			bed.add(t[0], int(t[1]), int(t[2]))
	sys.stderr.write("Read in {} sec\n".format(timer() - start))
	start = timer()
	bed.index()
	sys.stderr.write("Index in {} sec\n".format(timer() - start))
	start = timer()
	with open(argv[2]) as fp:
		for line in fp:
			t = line[:-1].split("\t")
			st1, en1 = int(t[1]), int(t[2])
			cov, cnt = bed.coverage(t[0], st1, en1)
			print("{}\t{}\t{}\t{}\t{}".format(t[0], st1, en1, cnt, cov))
	sys.stderr.write("Query in {} sec\n".format(timer() - start))

if __name__ == "__main__":
	main(sys.argv)
