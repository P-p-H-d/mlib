#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "m-tuple.h"
#include "m-tree.h"
#include "m-worker.h"
#include "m-string.h"
#include "m-atomic.h"


/* Define the comparison function for atomic_uint type
   to sort the atomic unsigned int with the biggest value first. */
static inline int atomic_uint_cmp(const atomic_uint *a, const atomic_uint *b)
{
  unsigned ai = atomic_load(a), bi = atomic_load(b);
  return ai > bi ? -1 : ai < bi;
}

/* Define the oplist for atomic_uint and register it globally.
   This oplist extends the basic oplist by:
   * disabling HASH for atomic as the default hash method won't work.
   * providing the methods for CMP, INIT_SET and SET operators (with their API adaptator) */
#define M_OPL_atomic_uint() M_OPEXTEND(M_BASIC_OPLIST, HASH(0), CMP(API_6(atomic_uint_cmp)), INIT_SET(API_2(atomic_store)), SET(API_2(atomic_store)))


/* Define a directory as a tree where each node contains:
   * the number of lines of the files in this directory,
   * and the name of the directory/
   Since we want to sort with the number of lines only, we extend the oplist of a string by disabling the compare method.
   So first, let's define the node as a tuple of such elements:
*/
M_TUPLE_DEF2( dir, (nlines, atomic_uint), (name, m_string_t, M_OPEXTEND(M_STRING_OPLIST, CMP(0))))
/* Register globally the oplist of this tuple */
#define M_OPL_dir_t() M_TUPLE_OPLIST(dir, M_OPL_atomic_uint(), M_STRING_OPLIST)

/* Define a hierarchical tree of directory.
   Note that it will use the globally registered oplist associated to dir_t to use it */
M_TREE_DEF(tree, dir_t)


/* Maximum number of characters read by file */
#define MAX_READ_BUFFER 8192

/* Maximum number of directory to scan */
#define MAX_DIRECTORY 100000


/* Define the pool of worker threads */
m_worker_t workers;

/* Global synchronous block for all threads */
m_worker_sync_t workers_block;

/* Add a specialization of the spawn function for the scan_file function.
   This functions takes as input an iterator of a tree (treated as a POD data),
   and a file name */
M_WORKER_SPAWN_DEF2(scan_file, (parent, tree_it_t, M_POD_OPLIST), (filename, m_string_t) )


/* Add either the root node or a child to parent in function of the existence of parent */
static tree_it_t
add_directory(tree_it_t parent, const m_string_t dirname)
{
  if (tree_end_p(parent)) {
    return tree_emplace_root( tree_tree(parent), 0, dirname);
  } else {
    return tree_emplace_child(parent, 0, dirname);
  }
}

/* count the number of end of line character in the buffer */
static unsigned
count_eol(size_t num, const char buffer[num])
{
  unsigned count = 0;
  for(size_t i = 0; i < num; i++) {
    count += buffer[i] == '\n';
  }
  return count;
}

/* Test if a filename suffix ends with a C++ or C suffix */
static bool
is_a_c_file(const m_string_t filename)
{
  return (m_string_end_with_str_p(filename, ".c")
    || m_string_end_with_str_p(filename, ".h")
    || m_string_end_with_str_p(filename, ".cpp")
    || m_string_end_with_str_p(filename, ".hpp"));
}

/* Scan the file and count its number of lines */
static void
scan_file(tree_it_t parent, m_string_t filename)
{
  FILE *f = fopen(m_string_get_cstr(filename), "rt");
  if (f == NULL) {
    fprintf(stderr, "ERROR: Cannot open %s as a text file.\n", m_string_get_cstr(filename) );
    exit(1);
  }

  /* Read the FILE per bulk of MAX_READ_BUFFER */
  char buffer[MAX_READ_BUFFER];
  unsigned count = 0;
  size_t num;
  do {
    num = fread (buffer, 1, MAX_READ_BUFFER, f);
    count += count_eol(num, buffer);
  } while (num == MAX_READ_BUFFER);
  fclose(f);

  /* Adding the number of lines to the parent directory */
  dir_t *d = tree_ref(parent);
  /* atomic add because the scan of files is done in parallel */
  atomic_fetch_add( &(*d)->nlines, count );
}

/* Scan the directories recursively for all files */
static void
scan_directories(tree_it_t parent, const m_string_t dirname)
{
  tree_it_t it = add_directory(parent, dirname);

  m_string_t filename;
  m_string_init(filename);
  DIR *dir = opendir(m_string_get_cstr(dirname));
  if (dir == NULL) {
    fprintf(stderr, "ERROR: Cannot open %s as a directory.\n", m_string_get_cstr(dirname) );
    exit(1);
  }

  // Scan all entries in the directory
  struct dirent *entry = readdir(dir);
  while (entry) {
    // Ignore hidden entries (including '.' and '..')
    if (entry->d_name[0] != '.') {
      // Construct the path to the filename by concatenating dirname, "/" and d_name
      m_string_sets(filename, dirname, "/", entry->d_name);
      // Get the property of this filename
      struct stat buf;
      int err = stat(m_string_get_cstr(filename), &buf);
      if (err != 0) {
        fprintf(stderr, "ERROR: Cannot stat %s\n", m_string_get_cstr(filename));
        exit(2);
      }
      // Further scanning in function of the kind of entry
      if (S_ISDIR(buf.st_mode)) {
        scan_directories(it, filename);
      } else if (is_a_c_file(filename)) {
        // Spawn a scan of file to available workers or run it ourselves
        m_worker_spawn_scan_file(workers_block, scan_file, it, filename);
      }
    }
    // Next entry in the directory
    entry = readdir(dir);
  }
  
  closedir(dir);
  m_string_clear(filename);
}

/* Consolidate all lines number to the parent directory */
static void
consolidate_directories(tree_t directories)
{
  // Scan all entries, first the children, then the parent
  for(tree_it_t it = tree_it_post(directories) ; !tree_end_p(it) ; tree_next_post(&it) ) {
    dir_t *parent = tree_up_ref(it);
    dir_t *myself = tree_ref(it);
    if (parent != NULL) {
      // Add the child number of lines to the parent
      atomic_fetch_add_explicit(&(*parent)->nlines, (*myself)->nlines, memory_order_relaxed);
    }
    // Sort the data of the children by the number of lines
    tree_sort_child(it);
  }
}

/* Print the consolidated results on stdout */
static void
print_result(tree_t directories)
{
  // Scan all entries, first the parent, then the children
  for(tree_it_t it = tree_it(directories) ; !tree_end_p(it) ; tree_next(&it) ) {
    int i, depth = tree_depth(it);
    for(i = 0; i < depth; i++) printf("+");
    for(     ; i < 8; i++) printf(" ");
    const dir_t *d = tree_cref(it);
    printf("%6u %s\n", (unsigned) (*d)->nlines, m_string_get_cstr( (*d)->name ) );
  }
}

int main(int argc, const char *argv[])
{
  tree_t directories;
  m_string_t str;

  printf("Count the number of C/C++ lines of code\n");
  if (argc < 2) {
    fprintf(stderr, "ERROR. Usage is '%s <directory>'.\n", argv[0]);
    exit(1);
  }

  /* Initialize the worker threads */
  m_worker_init(workers);
  
  /* Init the tree, reserve for at least the number of max directory
    and prevent any further increase of allocation:
    this is to ensure that other threads can deref a dir safely */
  tree_init(directories);
  tree_reserve(directories, MAX_DIRECTORY);
  tree_lock(directories, true);
  m_string_init_set_cstr(str, argv[1]);
  tree_it_t it = tree_it_end(directories);

  /* Main processing */
  m_worker_start(workers_block, workers);
  scan_directories(it, str);
  worker_sync(workers_block);
  consolidate_directories(directories);
  print_result(directories);

  /* Clear everything & quit */
  m_string_clear(str);
  tree_clear(directories);
  m_worker_clear(workers);

  exit(0);
}
