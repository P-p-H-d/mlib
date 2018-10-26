#include <stdio.h>

#include "m-tuple.h"
#include "m-array.h"
#include "m-string.h"
#include "m-list.h"
#include "m-buffer.h"

TUPLE_DEF2(symbol, (type, int), (name, string_t))
#define M_OPL_symbol_t() TUPLE_OPLIST(symbol, M_DEFAULT_OPLIST, STRING_OPLIST)

ARRAY_DEF(symbol_list, symbol_t)
#define M_OPL_symbol_list_t() ARRAY_OPLIST(symbol_list, M_OPL_symbol_t())

TUPLE_DEF2(record, (fields, symbol_list_t))
#define M_OPL_record_t() TUPLE_OPLIST(record, M_OPL_symbol_list_t())

TUPLE_DEF2(my_tuple, (name, string_t), (value, int))
#define M_OPL_my_tuple_t() TUPLE_OPLIST(my_tuple)

ARRAY_DEF(my_list_of_tuple, my_tuple_t)
#define M_OPL_my_list_of_tuple_t() ARRAY_OPLIST(my_list_of_tuple)

TUPLE_DEF2(my_tuple_of_tuple, (name, string_t), (inner_tuple, my_tuple_t))
#define M_OPL_my_tuple_of_tuple_t() TUPLE_OPLIST(my_tuple_of_tuple)

LIST_DEF(list_of_tuple_of_tuple, my_tuple_of_tuple_t)
#define M_OPL_list_of_tuple_of_tuple_t() LIST_OPLIST(list_of_tuple_of_tuple, M_OPL_my_tuple_of_tuple_t() )

TUPLE_DEF2(my_triple_tuple, (name, string_t), (inner_tuple, my_tuple_t), (list, list_of_tuple_of_tuple_t) )
#define M_OPL_my_triple_tuple_t() TUPLE_OPLIST(my_triple_tuple, STRING_OPLIST, M_OPL_my_tuple_t(), M_OPL_list_of_tuple_of_tuple_t())

BUFFER_DEF(trbuff, my_triple_tuple_t, 100, BUFFER_QUEUE)
#define M_OPL_trbuff_t() BUFFER_OPLIST(trbuff, M_OPL_my_triple_tuple_t())

TUPLE_DEF2(my_junction, (active, bool), (buffer, trbuff_t))
#define M_OPL_my_junction_t() TUPLE_OPLIST(my_junction, M_DEFAULT_OPLIST, M_OPL_trbuff_t())
           
static void let(void)
{
  M_LET(s, symbol_t)
    M_LET(l, symbol_list_t)
    M_LET(r,record_t)
    M_LET(tup, my_tuple_t)
    M_LET(l2, my_list_of_tuple_t)
    M_LET(tuptup, my_tuple_of_tuple_t)
    M_LET(ltuptup, list_of_tuple_of_tuple_t)
    M_LET(tuptuptup, my_triple_tuple_t)
    M_LET(junc, my_junction_t)
    {
      printf("Init ok\n");
    }
}

int main(void)
{
  let();
}

