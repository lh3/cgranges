## Introduction

cgranges is a small C library for genomic interval overlap queries: given a
genomic region *r* and a set of regions *R*, finding all regions in *R* that
overlaps *r*. cgranges implements an implicit interval tree. In comparison to
other implementations, it runs faster, has a lower memory footprint and takes
less code.

## Usage

### Test with BED coveragae

For testing purposes, this repo implements the [bedtools coverage][bedcov] tool
with cgranges. The source code is located in the [test/](test) directory. You
can compile and run the test with:
```sh
cd test && make
./bedcov-cr test1.bed test2.bed
```
The first BED file is loaded into RAM and indexed. The depth and the breadth of
coverage of each region in the second file is computed by query the index of
the first file.

The [test/](test) directory also contains a few other implementations based on
[IntervalTree.h][ekg-itree] in C++, [quicksect][quicksect] in Cython and
[ncls][ncls] in Cython.

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
