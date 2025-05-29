#include <stdio.h>
#include "m-array.h"
#include "m-algo.h"
#include "m-string.h"

ARRAY_DEF(array_int, int)
#define M_OPL_array_int_t() ARRAY_OPLIST(array_int, M_BASIC_OPLIST)
ALGO_DEF(array_int, array_int_t)

static bool callback1(int *out, int in, void *data)
{
  int *cpt = data;
  bool ret = true;
  if (*cpt == -1 && in == 11)
    *cpt = 0;                               // Start when first reach 11
  ret &= (*cpt >= 0);                       // Skip initial numbers unequal 11
  ret &= (in % 2 != 0);                     // Collect odd numbers
  (*cpt) += ret;
  ret &= (*cpt <= 5);                      //  Only take five numbers
  *out = in * in;                           // Square each number
  return ret;
}

static void callback_add_int(int *out, int in)
{
  *out += in;
}

static void demo(void)
{
  M_LET( (t, M_SEQ(0, 30)), vector, array_int_t) {
    printf("t=");  array_int_out_str(stdout, t);
    int b = -1;
    array_int_transform(vector, t, callback1, &b);
    printf("\nvector=");  array_int_out_str(stdout, vector); printf("\n");
    array_int_reduce(&b, vector, callback_add_int);
    printf("Sum=%d\n", b);
  }
}

ARRAY_DEF(array_str, string_t)
#define M_OPL_array_str_t() ARRAY_OPLIST(array_str, M_STRING_OPLIST)
ALGO_DEF(array_str, array_str_t)

static bool callback2(string_t *out, const string_t in, void *data)
{
  char *cp = data, c = *cp;
  if (string_search_char(in, c) == M_STRING_FAILURE)
    return false;
  string_set(*out, in);
  return true;
}

static void demo_split(void)
{
  M_LET( (s, ("This is a sentence in C11.")), string_t)
    M_LET(words, f_words, array_str_t) {
    array_str_split(words, s, ' ');
    array_str_transform(f_words, words, callback2, &(char){'i'});
    printf("f_words="); array_str_out_str(stdout, f_words); printf("\n");
  }
}

int main(void)
{
  demo();
  demo_split();
  return 0;
}
