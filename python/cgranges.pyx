from libc.stdint cimport int32_t, int64_t
from libc.stdlib cimport free

cdef extern from "cgranges.h":

	ctypedef struct cr_intv_t:
		pass

	ctypedef struct cgranges_t:
		pass

	cgranges_t *cr_init()
	void cr_destroy(cgranges_t *cr)
	cr_intv_t *cr_add(cgranges_t *cr, const char *ctg, int32_t st, int32_t en, int32_t label_int)
	void cr_index(cgranges_t *cr)
	int64_t cr_overlap(const cgranges_t *cr, const char *ctg, int32_t st, int32_t en, int64_t **b_, int64_t *m_b_)
	int32_t cr_start(const cgranges_t *cr, int64_t i)
	int32_t cr_end(const cgranges_t *cr, int64_t i)
	int32_t cr_label(const cgranges_t *cr, int64_t i)

cdef class cgranges:
	cdef cgranges_t *cr
	cdef int indexed

	def __cinit__(self):
		self.cr = cr_init()
		indexed = 0

	def __dealloc__(self):
		cr_destroy(self.cr)

	def add(self, ctg, st, en, label=-1):
		if not self.indexed:
			cr_add(self.cr, str.encode(ctg), st, en, label)

	def index(self):
		if not self.indexed:
			cr_index(self.cr)
			self.indexed = 1

	def overlap(self, ctg, st, en):
		cdef int64_t *b = NULL
		cdef int64_t m_b = 0
		cdef int64_t n
		if not self.indexed: return None
		n = cr_overlap(self.cr, str.encode(ctg), st, en, &b, &m_b)
		for i in range(n):
			yield cr_start(self.cr, b[i]), cr_end(self.cr, b[i]), cr_label(self.cr, b[i])
		free(b)

	def coverage(self, ctg, st, en):
		cdef int64_t *b = NULL
		cdef int64_t m_b = 0
		cdef int64_t n
		cdef int32_t cov, cov_st, cov_en, cnt
		if not self.indexed: return None
		cov, cov_st, cov_en = 0, 0, 0
		n = cr_overlap(self.cr, str.encode(ctg), st, en, &b, &m_b)
		for i in range(n):
			st0, en0 = cr_start(self.cr, b[i]), cr_end(self.cr, b[i])
			if st0 < st: st0 = st
			if en0 > en: en0 = en
			if st0 > cov_en:
				cov += cov_en - cov_st
				cov_st, cov_en = st0, en0
			else:
				if cov_en < en0: cov_en = en0
		cov += cov_en - cov_st
		free(b)
		return cov, n
