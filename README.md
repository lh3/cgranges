## Introduction

cgranges is a small C library for genomic interval overlap queries: given a
genomic region *r* and a set of regions *R*, finding all regions in *R* that
overlaps *r*. Although this library is based on [interval tree][itree], a well
known data structure, the core algorithm of cgranges is distinct from all
existing implementations to the best of our knowledge.  Specifically, the
interval tree in cgranges is implicitly encoded as a plain sorted array
(similar to [binary heap][bheap] but packed differently). Tree
traversal is achieved by jumping between array indices. This treatment makes
cgranges very efficient and compact in memory. The core algorithm can be
implemented in ~50 lines of C++ code, much shorter than others as well. Please
see the code comments in [cpp/IITree.h](cpp/IITree.h) for details.

## Usage

### Test with BED coverage

For testing purposes, this repo implements the [bedtools coverage][bedcov] tool
with cgranges. The source code is located in the [test/](test) directory. You
can compile and run the test with:
```sh
cd test && make
./bedcov-cr test1.bed test2.bed
```
The first BED file is loaded into RAM and indexed. The depth and the breadth of
coverage of each region in the second file is computed by query against the
index of the first file.

The [test/](test) directory also contains a few other implementations based on
[IntervalTree.h][ekg-itree] in C++, [quicksect][quicksect] in Cython and
[ncls][ncls] in Cython. The table below shows timing and peak memory on two
test BEDs available in the release page. The first BED contains GenCode
annotations with ~1.2 million lines, mixing all types of features. The second
contains ~10 million direct-RNA mappings. Time1/Mem1 indexes the GenCode BED
into memory. Time2/Mem2 indexes the RNA-mapping BED into memory. Python
implementations only count overlapping regions, but do not compute coverage.

|Program      |Description     |Time1 |Mem1    |Time2 |Mem2     |
|:------------|:---------------|-----:|-------:|-----:|--------:|
|bedcov-cr    |cgranges        |10.6s |19.1Mb  |5.3s  |153.9Mb  |
|bedcov-iitree|cpp/IITree.h    |13.4s |22.4Mb  |7.1s  |179.7Mb  |
|bedcov-itree |IntervalTree.h  |19.2s |26.8Mb  |11.4s |209.7Mb  |
|bedcov-cr.py |python cgranges |62.8s |23.4Mb  |47.8s |158.4Mb  |
|bedcov-qs    |python quicksect|61.5s |220.6Mb |188.0s|1802.2Mb |
|bedcov-ncls  |python ncls     |69.6s |209.6Mb |55.0s |1626.3Mb |
|bedtools     |                |232.8s|478.9Mb |173.8s|3821.0Mb |

Here, [bedcov-cr](test/bedcov-cr.c) and [bedcov-iitree](test/bedcov-iitree.cpp)
implement the same core algorithm, but but the latter is less careful about
memory management and memory locality. The comparison between them shows how
much extra code affects performance. [bedcov-iitree](test/bedcov-iitree.cpp)
and [bedcov-itree](test/bedcov-itree.cpp) have similar object structures, but
the latter uses a more standard implementation of [centered interval
trees][citree]. The comparison between them shows the effect of the core
interval tree algorithms. [bedcov-cr.py](test/bedcov-cr.py) uses much less
memory in comparison to other python implementations, but it is not faster.
Cython imposes significant overhead. [bedcov-qs](test/bedcov-qs.py) is
probably the only
implementation here that builds the interval tree dynamically. This slows down
indexing at a cost. [bedcov-ncls](test/bedcov-ncls.py) is the only one based
nest containment lists. Bedtools is designed for a variety of other tasks and
may include extra information in its internal data structures. This
micro-benchmark may be unfair to bedtools.

### Use cgranges as a C library

```c
cgranges_t *cr = cr_init(); // initialize a cgranges_t object
cr_add(cr, "chr1", 20, 30, 0); // add a genomic interval
cr_add(cr, "chr2", 10, 30, 1);
cr_add(cr, "chr1", 10, 25, 2);
cr_index(cr); // index

int64_t i, n, *b = 0, max_b = 0;
n = cr_overlap(cr, "chr1", 15, 22, &b, &max_b); // overlap query; output array b[] can be reused
for (i = 0; i < n; ++i) // traverse overlapping intervals
	printf("%d\t%d\t%d\n", cr_start(cr, b[i]), cr_end(cr, b[i]), cr_label(cr, b[i]));
free(b); // b[] is allocated by malloc() inside cr_overlap(), so needs to be freed with free()

cr_destroy(cr);
```

### Use IITree as a C++ library

```cpp
IITree<int, int> tree;
tree.add(12, 34, 0); // add an interval
tree.add(0, 23, 1);
tree.add(34, 56, 2);
tree.index(); // index
std::vector<size_t> a;
tree.overlap(22, 25, a); // retrieve overlaps
for (size_t i = 0; i < a.size(); ++i)
	printf("%d\t%d\t%d\n", tree.start(a[i]), tree.end(a[i]), tree.data(a[i]));
```

[bedcov]: https://bedtools.readthedocs.io/en/latest/content/tools/coverage.html
[ekg-itree]: https://github.com/ekg/intervaltree
[quicksect]: https://github.com/brentp/quicksect
[ncls]: https://github.com/hunt-genes/ncls
[citree]: https://en.wikipedia.org/wiki/Interval_tree#Centered_interval_tree
[itree]: https://en.wikipedia.org/wiki/Interval_tree
[bheap]: https://en.wikipedia.org/wiki/Binary_heap
