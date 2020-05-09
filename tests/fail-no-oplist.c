#include "m-core.h"

#if !defined(TEST)
# warning Please define TEST variable to select the test to run

#elif TEST == 1

/* Test of the call to no default function */
#define OP (INIT(init))

void f(int x, int y) {
  M_CALL_SWAP(OP, x, y);
}

#elif TEST == 2

void f(int x, int y) {
  M_CALL_SET(M_VALID_OPLIST(DEFAULT_OPLIST), x, y);
}
     
#elif TEST == 3

#include "m-array.h"
ARRAY_DEF(array, int, DEFAULT_OPLIST)

#elif TEST == 4

#include "m-list.h"
LIST_DEF(list, int, DEFAULT_OPLIST)

#elif TEST == 5

#include "m-deque.h"
DEQUE_DEF(deque, int, DEFAULT_OPLIST)

#elif TEST == 6

#include "m-i-list.h"
typedef struct tmp_str_s {
  ILIST_INTERFACE(tmpstr, struct tmp_str_s);
  int n;
} tmp_str_t;
ILIST_DEF(tmpstr, tmp_str_t, DEFAULT_OPLIST)

#elif TEST == 7

#include "m-string.h"
void f(void)
{
  M_LET(x, M_STRING_OPLIST) {
  }
}

#elif TEST == 8

#include "m-string.h"
typedef string_t my_type;
#define M_OPL_my_type() MSTRING_OPLIST
void f(void)
{
  M_LET(x, my_type) {
  }
}

#elif TEST == 9

void f(void)
{
  M_LET(x, () ) {
  }
}

#else
# warning TEST variable is out of range.
#endif

