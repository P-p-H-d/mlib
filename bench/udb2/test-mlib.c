/* Makefile:

all:run-test

run-test:test.c ../common.c mlib
	$(CC) -std=gnu99 -O2 -Wall -Imlib $< -o $@

mlib:
	git clone https://github.com/P-p-H-d/mlib.git

clean:
	rm -f run-test* *~
	rm -rf mlib
*/

#include "../common.c"
#include "m-dict.h"

static inline bool oor_equal_p(unsigned int k, unsigned char n)
{
  return k == (unsigned int)n;
}
static inline void oor_set(unsigned int *k, unsigned char n)
{
  *k = (unsigned int)n;
}

DICT_OA_DEF2(dict_oa_uint,
	     unsigned int, M_OPEXTEND(M_DEFAULT_OPLIST, OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)),
	     unsigned int, M_DEFAULT_OPLIST)

void test_int(uint32_t n, uint32_t x0)
{
  uint32_t i, x, z = 0;
  dict_oa_uint_t h;
  dict_oa_uint_init(h);
  for (i = 0, x = x0; i < n; ++i) {
    x = hash32(x);
    unsigned int key = get_key(n, x);
    unsigned int *ptr = dict_oa_uint_get(h, key);
    if (ptr) (*ptr)++;
    else dict_oa_uint_set_at(h, key, 1);
  }
  fprintf(stderr, "# unique keys: %d; checksum: %u\n", (int) dict_oa_uint_size(h), z);
  dict_oa_uint_clear(h);
}
