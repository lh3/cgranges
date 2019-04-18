#include <stdio.h>
#include <stdlib.h>
#include "cgranges.h"

int main(void)
{
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
	return 0;
}
