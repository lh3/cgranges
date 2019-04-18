.. code:: python

	import cgranges as cr
	g = cr.cgranges();
	g.add("chr1", 10, 20, 0)
	g.add("chr1", 15, 25, 1)
	g.index()
	for st, en, label in g.overlap("chr1", 12, 16):
		print(st, en, label)
