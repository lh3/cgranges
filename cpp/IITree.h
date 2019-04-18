#pragma once

#include <vector>
#include <algorithm>

template<typename S, typename T>
class IITree {
	struct StackCell {
		size_t x;
		int k, w;
		StackCell() {};
		StackCell(int k_, size_t x_, int w_) : x(x_), k(k_), w(w_) {};
	};
	struct Interval {
		S st, en, max;
		T data;
		Interval(const S &s, const S &e, const T &d) : st(s), en(e), max(e), data(d) {};
	};
	struct IntervalLess {
		bool operator()(const Interval &a, const Interval &b) const { return a.st < b.st; }
	};
	std::vector<Interval> a;
	int max_level;
	int index_core(std::vector<Interval> &a) {
		size_t i, last_i;
		S last;
		int k;
		if (a.size() == 0) return -1;
		for (i = 0; i < a.size(); i += 2) last_i = i, last = a[i].max = a[i].en;
		for (k = 1; 1LL<<k <= a.size(); ++k) {
			size_t x = 1LL<<(k-1), i0 = (x<<1) - 1, step = x<<2;
			for (i = i0; i < a.size(); i += step) {
				S el = a[i - x].max;
				S er = i + x < a.size()? a[i + x].max : last;
				S e = a[i].en;
				e = e > el? e : el;
				e = e > er? e : er;
				a[i].max = e;
			}
			last_i = last_i>>k&1? last_i - x : last_i + x;
			if (last_i < a.size() && a[last_i].max > last)
				last = a[last_i].max;
		}
		return k - 1;
	}
public:
	void add(const S &s, const S &e, const T &d) { a.push_back(Interval(s, e, d)); }
	void index(void) {
		std::sort(a.begin(), a.end(), IntervalLess());
		max_level = index_core(a);
	}
	void overlap(const S &st, const S &en, std::vector<size_t> &out) const {
		int t = 0;
		StackCell stack[64];
		out.clear();
		stack[t++] = StackCell(max_level, (1LL<<max_level) - 1, 0);
		while (t) {
			StackCell z = stack[--t];
			if (z.k <= 2) {
				size_t i, i0 = z.x >> z.k << z.k, i1 = i0 + (1LL<<(z.k+1)) - 1;
				if (i1 >= a.size()) i1 = a.size();
				for (i = i0; i < i1; ++i)
					if (a[i].st < en && st < a[i].en)
						out.push_back(i);
			} else if (z.w == 0) { // if left child not processed
				size_t y = z.x - (1LL<<(z.k-1));
				stack[t++] = StackCell(z.k, z.x, 1);
				if (y >= a.size() || a[y].max > st)
					stack[t++] = StackCell(z.k - 1, y, 0);
			} else if (z.x < a.size() && a[z.x].st < en) {
				if (st < a[z.x].en) out.push_back(z.x);
				stack[t++] = StackCell(z.k - 1, z.x + (1LL<<(z.k-1)), 0);
			}
		}
	}
	const Interval &operator[](size_t i) const { return a[i]; }
};
