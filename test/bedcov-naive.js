#!/usr/bin/env k8

Interval = {};

Interval.sort = function(a)
{
	if (typeof a[0] == 'number')
		a.sort(function(x, y) { return x - y });
	else a.sort(function(x, y) { return x[0] != y[0]? x[0] - y[0] : x[1] - y[1] });
}

Interval.merge = function(a, sorted)
{
	if (typeof sorted == 'undefined') sorted = true;
	if (!sorted) Interval.sort(a);
	var k = 0;
	for (var i = 1; i < a.length; ++i) {
		if (a[k][1] >= a[i][0])
			a[k][1] = a[k][1] > a[i][1]? a[k][1] : a[i][1];
		else a[++k] = a[i].slice(0);
	}
	a.length = k + 1;
}

Interval.index_end = function(a, sorted)
{
	if (a.length == 0) return;
	if (typeof sorted == 'undefined') sorted = true;
	if (!sorted) Interval.sort(a);
	a[0].push(0);
	var k = 0, k_en = a[0][1];
	for (var i = 1; i < a.length; ++i) {
		if (k_en <= a[i][0]) {
			for (++k; k < i; ++k)
				if (a[k][1] > a[i][0])
					break;
			k_en = a[k][1];
		}
		a[i].push(k);
	}
}

Interval.find_intv = function(a, x)
{
	var left = -1, right = a.length;
	if (typeof a[0] == 'number') {
		while (right - left > 1) {
			var mid = left + ((right - left) >> 1);
			if (a[mid] > x) right = mid;
			else if (a[mid] < x) left = mid;
			else return mid;
		}
	} else {
		while (right - left > 1) {
			var mid = left + ((right - left) >> 1);
			if (a[mid][0] > x) right = mid;
			else if (a[mid][0] < x) left = mid;
			else return mid;
		}
	}
	return left;
}

Interval.find_ovlp = function(a, st, en)
{
	if (a.length == 0 || st >= en) return [];
	var l = Interval.find_intv(a, st);
	var k = l < 0? 0 : a[l][a[l].length - 1];
	var b = [];
	for (var i = k; i < a.length; ++i) {
		if (a[i][0] >= en) break;
		else if (st < a[i][1])
			b.push(a[i]);
	}
	return b;
}

function main(args)
{
	if (args.length < 2) {
		warn("Usage: bedcov.js <loaded.bed> <streamed.bed>");
		exit(1);
	}
	var file, buf = new Bytes();

	file = new File(args[0]);
	var bed = {};
	while (file.readline(buf) >= 0) {
		var t = buf.toString().split("\t", 3);
		if (bed[t[0]] == null) bed[t[0]] = [];
		bed[t[0]].push([parseInt(t[1]), parseInt(t[2])]);
	}
	for (var ctg in bed) {
		Interval.sort(bed[ctg]);
		Interval.index_end(bed[ctg]);
	}
	file.close();

	file = new File(args[1]);
	while (file.readline(buf) >= 0) {
		var t = buf.toString().split("\t", 3);
		if (bed[t[0]] == null) {
			print(t[0], t[1], t[2], 0, 0);
		} else {
			var st0 = parseInt(t[1]), en0 = parseInt(t[2]);
			var a = Interval.find_ovlp(bed[t[0]], st0, en0);
			var cov_st = 0, cov_en = 0, cov = 0;
			for (var i = 0; i < a.length; ++i) {
				var st1 = a[i][0] > st0? a[i][0] : st0;
				var en1 = a[i][1] < en0? a[i][1] : en0;
				if (st1 > cov_en) {
					cov += cov_en - cov_st;
					cov_st = st1, cov_en = en1;
				} else cov_en = cov_en > en1? cov_en : en1;
			}
			cov += cov_en - cov_st;
			print(t[0], t[1], t[2], a.length, cov);
		}
	}
	file.close();

	buf.destroy();
}

main(arguments);
