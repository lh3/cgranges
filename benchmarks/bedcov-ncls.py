#!/usr/bin/env python

from ncls import NCLS
import numpy as np
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
				bed[t[0]] = [[], [], [], None]
			bed[t[0]][0].append(t[1])
			bed[t[0]][1].append(t[2])
			bed[t[0]][2].append(i)
			i += 1
	sys.stderr.write("Read in {} sec\n".format(timer() - start))
	start = timer()
	for ctg in bed:
		bed[ctg][3] = NCLS(np.array(bed[ctg][0], dtype=np.long), np.array(bed[ctg][1], dtype=np.long), np.array(bed[ctg][2], dtype=np.long))
	sys.stderr.write("Index in {} sec\n".format(timer() - start))
	start = timer()
	with open(argv[2]) as fp:
		for line in fp:
			t = line[:-1].split("\t")
			if not t[0] in bed:
				print("{}\t{}\t{}\t0".format(t[0], t[1], t[2]))
			else:
				cnt = 0
				it = bed[t[0]][3].find_overlap(long(t[1]), long(t[2]))
				for r in it: cnt += 1
				print("{}\t{}\t{}\t{}".format(t[0], t[1], t[2], cnt))
	sys.stderr.write("Query in {} sec\n".format(timer() - start))

if __name__ == "__main__":
	main(sys.argv)
