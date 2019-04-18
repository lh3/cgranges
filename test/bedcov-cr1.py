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
			cov_st, cov_en, cov, cnt = 0, 0, 0, 0
			for st0, en0, label in bed.overlap(t[0], st1, en1):
				if st0 < st1: st0 = st1
				if en0 > en1: en0 = en1
				if st0 > cov_en:
					cov += cov_en - cov_st
					cov_st, cov_en = st0, en0
				else:
					if cov_en < en0: cov_en = en0
				cnt += 1
			cov += cov_en - cov_st
			print("{}\t{}\t{}\t{}\t{}".format(t[0], st1, en1, cnt, cov))
	sys.stderr.write("Query in {} sec\n".format(timer() - start))

if __name__ == "__main__":
	main(sys.argv)
