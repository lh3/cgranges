#include <stdio.h>
#include "cgranges.h"
#include "ksort.h"
#include "khash.h"
#include "IntervalTree.h"

/*********************
 * Convenient macros *
 *********************/

#define CALLOC(type, len) ((type*)calloc((len), sizeof(type)))
#define REALLOC(ptr, len) ((ptr) = (__typeof__(ptr))realloc((ptr), (len) * sizeof(*(ptr))))

#define EXPAND(a, m) do { \
		(m) = (m)? (m) + ((m)>>1) : 16; \
		REALLOC((a), (m)); \
	} while (0)

/***********************
 * Various definitions *
 ***********************/

#define cr_intv_key(r) ((r).x)
KRADIX_SORT_INIT(cr_intv, cr_intv_t, cr_intv_key, 8)

KHASH_MAP_INIT_STR(str, int32_t)
typedef khash_t(str) strhash_t;

typedef IntervalTree<int32_t, int32_t> ITree;

/********************
 * Basic operations *
 ********************/

cgranges_t *cr_init(void)
{
	cgranges_t *cr;
	cr = CALLOC(cgranges_t, 1);
	cr->hc = kh_init(str);
	return cr;
}

void cr_destroy(cgranges_t *cr)
{
	int32_t i;
	if (cr == 0) return;
	for (i = 0; i < cr->n_ctg; ++i) {
		free(cr->ctg[i].name);
		delete (ITree*)cr->ctg[i].ptr;
	}
	free(cr->ctg);
	kh_destroy(str, (strhash_t*)cr->hc);
	free(cr);
}

int32_t cr_add_ctg(cgranges_t *cr, const char *ctg, int32_t len)
{
	int absent;
	khint_t k;
	strhash_t *h = (strhash_t*)cr->hc;
	k = kh_put(str, h, ctg, &absent);
	if (absent) {
		cr_ctg_t *p;
		if (cr->n_ctg == cr->m_ctg)
			EXPAND(cr->ctg, cr->m_ctg);
		kh_val(h, k) = cr->n_ctg;
		p = &cr->ctg[cr->n_ctg++];
		p->name = strdup(ctg);
		kh_key(h, k) = p->name;
		p->len = len;
	}
	if (len > cr->ctg[kh_val(h, k)].len)
		cr->ctg[kh_val(h, k)].len = len;
	return kh_val(h, k);
}

int32_t cr_get_ctg(const cgranges_t *cr, const char *ctg)
{
	khint_t k;
	strhash_t *h = (strhash_t*)cr->hc;
	k = kh_get(str, h, ctg);
	return k == kh_end(h)? -1 : kh_val(h, k);
}

cr_intv_t *cr_add_intv(cgranges_t *cr, const char *ctg, int32_t st, int32_t en, int32_t label_int)
{
	cr_intv_t *p;
	int32_t k;
	if (st > en) return 0;
	k = cr_add_ctg(cr, ctg, 0);
	if (cr->n_r == cr->m_r)
		EXPAND(cr->r, cr->m_r);
	p = &cr->r[cr->n_r++];
	p->x = (uint64_t)k << 32 | st;
	p->y = en;
	p->label = label_int;
	if (cr->ctg[k].len < en)
		cr->ctg[k].len = en;
	return p;
}

void cr_sort(cgranges_t *cr)
{
	if (cr->n_ctg == 0 || cr->n_r == 0) return;
	radix_sort_cr_intv(cr->r, cr->r + cr->n_r);
}

int32_t cr_is_sorted(const cgranges_t *cr)
{
	uint64_t i;
	for (i = 1; i < cr->n_r; ++i)
		if (cr->r[i-1].x > cr->r[i].x)
			break;
	return (i == cr->n_r);
}

/************
 * Indexing *
 ************/

void cr_index_prepare(cgranges_t *cr)
{
	int64_t i, st;
	if (!cr_is_sorted(cr)) cr_sort(cr);
	for (st = 0, i = 1; i <= cr->n_r; ++i) {
		if (i == cr->n_r || cr->r[i].x>>32 != cr->r[st].x>>32) {
			int32_t ctg = cr->r[st].x>>32;
			cr->ctg[ctg].off = st;
			cr->ctg[ctg].n = i - st;
			st = i;
		}
	}
	for (i = 0; i < cr->n_r; ++i) {
		cr_intv_t *r = &cr->r[i];
		r->x = r->x<<32 | r->y;
		r->y = 0;
	}
}

void cr_index(cgranges_t *cr)
{
	ITree::interval_vector a;
	cr_index_prepare(cr);
	for (int32_t i = 0; i < cr->n_ctg; ++i) {
		cr_intv_t *r = &cr->r[cr->ctg[i].off];
		int64_t n = cr->ctg[i].n;
		a.clear();
		for (int64_t j = 0; j < n; ++j) {
			a.push_back(ITree::interval(cr_st(&r[j]) + 1, cr_en(&r[j]), j));
		}
		auto p = new ITree(std::move(a));
		cr->ctg[i].ptr = (void*)p;
	}
}

int64_t cr_overlap_int(const cgranges_t *cr, int32_t ctg_id, int32_t st, int32_t en, int64_t **b_, int64_t *m_b_)
{
	int64_t *b = *b_, m_b = *m_b_;
	if (ctg_id < 0 || ctg_id >= cr->n_ctg) return 0;
	ITree *tree = (ITree*)cr->ctg[ctg_id].ptr;
	auto a = tree->findOverlapping(st + 1, en);
	if (a.size() > m_b) {
		m_b = a.size();
		kroundup32(m_b);
		REALLOC(b, m_b);
	}
	for (size_t i = 0; i < a.size(); ++i)
		b[i] = cr->ctg[ctg_id].off + a[i].value;
	std::sort(b, b + a.size());
	*b_ = b, *m_b_ = m_b;
	return a.size();
}

int64_t cr_overlap(const cgranges_t *cr, const char *ctg, int32_t st, int32_t en, int64_t **b_, int64_t *m_b_)
{
	return cr_overlap_int(cr, cr_get_ctg(cr, ctg), st, en, b_, m_b_);
}
