#include <stdio.h>

#include "m-tuple.h"
#include "m-tree.h"
#include "m-worker.h"
#include "m-string.h"
#include "m-atomic.h"

#define M_OPL_atomic_uint() M_OPEXTEND(M_BASIC_OPLIST, HASH(0))

TUPLE_DEF2( dir, (nlines, atomic_uint), (name, string_t))
#define M_OPL_dir_t() TUPLE_OPLIST(dir, M_BASIC_OPLIST, M_STRING_OPLIST)

M_TREE_DEF(tree, dir_t)

static tree_t directories;

static void
scan_directories(const char *dirname)
{

}

static void
consolidate_directories(void)
{

}

static void
print_result(void)
{
  
}

int main(int argc, const char *argv[])
{
  if (argc < 2) {
    fprintf(stderr, "ERROR. Usage is %s <directory>.\n", argv[0]);
    exit(1);
  }

  tree_init(directories);
  scan_directories(argv[1]);
  consolidate_directories();
  print_result();
  tree_clear(directories);
  exit(0);
}
