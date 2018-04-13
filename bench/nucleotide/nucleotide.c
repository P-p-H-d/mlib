// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
//
// Original version Contributed by Jeremy Zerfas
// M*LIB port contributed by Patrick Pélissier
#define NDEBUG
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "m-worker.h"
#include "m-dict.h"
#include "m-bptree.h"

// This controls the maximum length for each set of oligonucleotide frequencies
// and each oligonucleotide count output by this program.
#define MAXIMUM_OUTPUT_LENGTH 4096

// Number of outputs to display
#define MAXMUM_NUMBER_OUTPUT 7

/* Define the needed containers */
/* Define Open Addressing dictionary of uint64_t --> uint32_t. Need to define how to handle out of bound data. */
static inline bool my_oor_equal_p(uint64_t k, unsigned char n)
{
  return k == ~(uint64_t) n;
}
static inline void my_oor_set(uint64_t *k, unsigned char n)
{
  *k = ~(uint64_t)n;
}
static inline size_t my_hash(uint64_t key)
{
  return ((key) ^ (key)>>7);
}
DICT_OA_DEF2(dict_oligonucleotide,
             uint64_t, M_OPEXTEND(M_DEFAULT_OPLIST, HASH(my_hash), OOR_EQUAL(my_oor_equal_p), OOR_SET(my_oor_set M_IPTR)),
             uint32_t, M_DEFAULT_OPLIST)
#define M_OPL_dict_oligonucleotide_t() DICT_OPLIST(dict_oligonucleotide, M_OPEXTEND(M_DEFAULT_OPLIST, HASH(my_hash), OOR_EQUAL(my_oor_equal_p), OOR_SET(my_oor_set M_IPTR)), M_DEFAULT_OPLIST)

// Define a B*TREE-21 of uint32_t --> uint64_t and register its oplist
#define TREE_CMP(a,b)      ((a) > (b) ? -1 : (a) < (b))
BPTREE_DEF2(tree_dict_oligonucleotide, 21, uint32_t, M_OPEXTEND(M_DEFAULT_OPLIST, CMP(TREE_CMP)), uint64_t, M_DEFAULT_OPLIST)
#define M_OPL_tree_dict_oligonucleotide_t() BPTREE_OPLIST2(tree_dict_oligonucleotide, M_DEFAULT_OPLIST, M_DEFAULT_OPLIST)

/* Define the data to analyse (a plain array) */
ARRAY_DEF(polynucleotide, unsigned char)

// Macro to convert a nucleotide character to a code. Note that upper and lower
// case ASCII letters only differ in the fifth bit from the right and we only
// need the three least significant bits to differentiate the letters 'A', 'C',
// 'G', and 'T'. Spaces in this array/string will never be used as long as
// characters other than 'A', 'C', 'G', and 'T' aren't used.
#define CODE_FOR_NUCLEOTIDE(nucleotide) (" \0 \1\3  \2"[nucleotide & 0x7])

// And one more macro to convert the codes back to nucleotide characters.
#define NUCLEOTIDE_FOR_CODE(code) ("ACGT"[code & 0x3])

static void polynucleotide_init_stream(polynucleotide_t p, FILE *stream)
{
  char buffer[MAXIMUM_OUTPUT_LENGTH], c;

  polynucleotide_init(p);
  polynucleotide_reserve(p, MAXIMUM_OUTPUT_LENGTH);
  // Find the start of the third polynucleotide.
  while(fgets(buffer, sizeof(buffer), stream) && memcmp(">THREE", buffer, 6));
  // Start reading and encoding the third polynucleotide.
  while(fgets(buffer, sizeof(buffer), stream) && buffer[0]!='>'){
    for(size_t i = 0; (c = buffer[i]) !='\0'; i++) {
      if (c != '\n') {
          polynucleotide_push_back(p, CODE_FOR_NUCLEOTIDE(c));
      }
    }
  }
}

static void compute_hash(dict_oligonucleotide_t hash_Table, const polynucleotide_t p, const size_t desiredLength)
{
  uint64_t key = 0;
  const uint64_t mask = (1ULL<<(2*desiredLength))-1;
  assert (2*desiredLength < 64 && desiredLength < polynucleotide_size(p));

  // For the first several nucleotides we only need to append them to key in
  // preparation for the insertion of complete oligonucleotides to hash_Table.
  for(size_t i = 0; i < desiredLength - 1; i++)
    key = (key<<2) | *polynucleotide_get(p, i);

  // Add all the complete oligonucleotides of
  // desiredLength to hash_Table, updating their count
  for(size_t i=desiredLength-1 ; i < polynucleotide_size(p); i++) {
    key = (key<<2 & mask) | *polynucleotide_get(p, i);
    uint32_t *p = dict_oligonucleotide_get(hash_Table, key);
    if (p != NULL) {
      (*p) ++;
    } else {
      dict_oligonucleotide_set_at(hash_Table, key, 1);
    }
  }
}

// Generate frequencies for all oligonucleotides in polynucleotide that are of
// desiredLength and then save it to output.
static void compute_freq(const polynucleotide_t p, const size_t desiredLength, char output[])
{  
  dict_oligonucleotide_t hash_Table;
  dict_oligonucleotide_init(hash_Table);
  compute_hash(hash_Table, p, desiredLength);

  // Order key is the count frequency (inverse value & key)
  tree_dict_oligonucleotide_t tree;
  tree_dict_oligonucleotide_init(tree);
  for M_EACH(item, hash_Table, dict_oligonucleotide_t) {
    tree_dict_oligonucleotide_set_at (tree, item->value, item->key);
  }

  // Create Output
  char oligonucleotide[64];
  const double invSum = 100.0 / (double) (polynucleotide_size(p) - desiredLength + 1);
  int output_Position = 0;
  for M_EACH(item, tree, tree_dict_oligonucleotide_t) {
    // Convert the key for the oligonucleotide to a string.
    uint64_t freq  = *item->value_ptr;
    uint32_t count = *item->key_ptr;
    for(size_t j = desiredLength-1; j < desiredLength; j--){
      oligonucleotide[j]=NUCLEOTIDE_FOR_CODE(freq);
      freq>>=2;
    }
    oligonucleotide[desiredLength]='\0';
    // Output the frequency for oligonucleotide to output.
    output_Position+=snprintf(output+output_Position, MAXIMUM_OUTPUT_LENGTH-output_Position,
                              "%s %.3f\n", oligonucleotide, invSum * count);
  }
  
  dict_oligonucleotide_clear(hash_Table);
  tree_dict_oligonucleotide_init(tree);
}

// Generate a count for the number of times oligonucleotide appears in
// polynucleotide and then save it to output.
static void compute_count(polynucleotide_t p, const char oligonucleotide[], char output[])
{
  const size_t oligonucleotide_Length = strlen(oligonucleotide);
  dict_oligonucleotide_t hash_Table;
  dict_oligonucleotide_init(hash_Table);
  compute_hash(hash_Table, p, oligonucleotide_Length);
  
  // Generate the key for oligonucleotide.
  uint64_t key=0;
  for(size_t i=0; i<oligonucleotide_Length; i++)
    key = (key<<2) | CODE_FOR_NUCLEOTIDE(oligonucleotide[i]);
  
  // Output the count for oligonucleotide to output.
  uint32_t *val = dict_oligonucleotide_get(hash_Table, key);
  uintmax_t count= val== NULL ? 0 : *val;
  snprintf(output, MAXIMUM_OUTPUT_LENGTH, "%ju\t%s", count, oligonucleotide);
  
  dict_oligonucleotide_clear(hash_Table);
}

int main()
{
  // Initialize the different worker threads by auto-detecting the number of cores of the system
  worker_t worker;
  worker_init(worker, 0, 1, NULL);

  // Read the input
  polynucleotide_t p, *ptr = &p;
  polynucleotide_init_stream(p, stdin);
  
  // Prepare outputs
  char outputBuffer[MAXMUM_NUMBER_OUTPUT][MAXIMUM_OUTPUT_LENGTH];
  char (*output)[MAXIMUM_OUTPUT_LENGTH] = outputBuffer;
  
  // Do the following functions in parallel.
  worker_sync_t block;   // Define a synchronization block
  worker_start(block);
  
  WORKER_SPAWN(worker, block, (ptr, output), {
      compute_count(*ptr, "GGTATTTTAATTTATAGT", output[6]);
    }, (/*no output*/));
  WORKER_SPAWN(worker, block, (ptr, output), {
      compute_count(*ptr, "GGTATTTTAATT", output[5]);
    }, (/*no output*/));
  WORKER_SPAWN(worker, block, (ptr, output), {
      compute_count(*ptr, "GGTATT", output[4]);
    }, (/*no output*/));
  WORKER_SPAWN(worker, block, (ptr, output), {
      compute_count(*ptr, "GGTA", output[3]);
    }, (/*no output*/));
  WORKER_SPAWN(worker, block, (ptr, output), {
      compute_count(*ptr, "GGT", output[2]);  
    }, (/*no output*/));
  WORKER_SPAWN(worker, block, (ptr, output), {
      compute_freq(*ptr, 2, output[1]);
    }, (/*no output*/));
  compute_freq(p, 1, output[0]);

  worker_sync(block);
  
  // Output the results to stdout.
  for(int i=0; i<MAXMUM_NUMBER_OUTPUT; i++)
    printf("%s\n", outputBuffer[i]);
  
  polynucleotide_clear(p);
  worker_clear(worker);
  
  return 0;
}
