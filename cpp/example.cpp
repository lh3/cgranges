#include <stdio.h>
#include "IITree.h"

int main(void)
{
	IITree<int, int> tree;
	tree.add(12, 34, 0);
	tree.add(0, 23, 1);
	tree.add(34, 56, 2);
	tree.index();
	std::vector<size_t> a;
	tree.overlap(22, 25, a);
	for (size_t i = 0; i < a.size(); ++i)
		printf("%d\t%d\t%d\n", tree.start(a[i]), tree.end(a[i]), tree.data(a[i]));
	return 0;
}
