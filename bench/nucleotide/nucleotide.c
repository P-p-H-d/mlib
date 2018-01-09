// The Computer Language Benchmarks Game
// http://benchmarksgame.alioth.debian.org/
//
// Original version Contributed by Jeremy Zerfas
// M*LIB port contributed by Patrick Pélissier
// gcc -O2 -march=native nucleotide.c -I../.. -std=c11 -lpthread -DNDEBUG

// This controls the maximum length for each set of oligonucleotide frequencies
// and each oligonucleotide count output by this program.
#define NDEBUG

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <pthread.h>

#include "m-worker.h"
#include "m-dict.h"

#define MAXIMUM_OUTPUT_LENGTH 4096

// Define Open Addressing dictionnary.
// Need to define how to handle out of bound data.
static inline bool oor_equal_p(uint64_t k, unsigned char n)
{
  return k == ~(uint64_t) n;
}
static inline void oor_set(uint64_t *k, unsigned char n)
{
  *k = ~(uint64_t)n;
}
// Custom hash function.
static inline size_t my_hash(uint64_t key)
{
  return ((key) ^ (key)>>7);
}
// Define dictionnary
DICT_OA_DEF2(dict_oligonucleotide,
             uint64_t, M_OPEXTEND(M_DEFAULT_OPLIST, HASH(my_hash), OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)),
             uint32_t, M_DEFAULT_OPLIST)
// Define oplist of dictionary
#define OLIGONUCLEOTIDE_OPLIST DICT_OPLIST(dict_oligonucleotide, M_OPEXTEND(M_DEFAULT_OPLIST, HASH(oor_hash), OOR_EQUAL(oor_equal_p), OOR_SET(oor_set M_IPTR)), M_DEFAULT_OPLIST)

typedef struct {
  uint64_t	key;
  uint32_t	value;
} element;

// Macro to convert a nucleotide character to a code. Note that upper and lower
// case ASCII letters only differ in the fifth bit from the right and we only
// need the three least significant bits to differentiate the letters 'A', 'C',
// 'G', and 'T'. Spaces in this array/string will never be used as long as
// characters other than 'A', 'C', 'G', and 'T' aren't used.
#define CODE_FOR_NUCLEOTIDE(nucleotide) (" \0 \1\3  \2"[nucleotide & 0x7])


// And one more macro to convert the codes back to nucleotide characters.
#define NUCLEOTIDE_FOR_CODE(code) ("ACGT"[code & 0x3])


// Function to use when sorting elements with qsort() later. Elements with
// larger values will come first and in cases of identical values then elements
// with smaller keys will come first.
static int element_Compare(const void *a, const void *b)
{
  const element * const left_Element = a;
  const element * const right_Element = b;
  // Sort based on element values.
  if(left_Element->value < right_Element->value) return 1;
  if(left_Element->value > right_Element->value) return -1;
  
  // If we got here then both items have the same value so then sort based on
  // key.
  return left_Element->key > right_Element->key ? 1 : -1;
}


// Generate frequencies for all oligonucleotides in polynucleotide that are of
// desired_Length_For_Oligonucleotides and then save it to output.
static void
generate_Frequencies_For_Desired_Length_Oligonucleotides(const char polynucleotide[],
                                                         const int polynucleotide_Length,
                                                         const int desired_Length_For_Oligonucleotides,
                                                         char * const output)
{  
  assert (2*desired_Length_For_Oligonucleotides <= 64);

  dict_oligonucleotide_t hash_Table;
  dict_oligonucleotide_init(hash_Table);
  
  uint64_t key=0;
  const uint64_t mask=(1ULL<<(2*desired_Length_For_Oligonucleotides))-1;
  
  // For the first several nucleotides we only need to append them to key in
  // preparation for the insertion of complete oligonucleotides to hash_Table.
  for(int i=0; i<desired_Length_For_Oligonucleotides-1; i++)
    key=(key<<2 & mask) | polynucleotide[i];

  // Add all the complete oligonucleotides of
  // desired_Length_For_Oligonucleotides to hash_Table and update the count
  // for each oligonucleotide.
  for(int i=desired_Length_For_Oligonucleotides-1
        ; i<polynucleotide_Length; i++){
    
    key=(key<<2 & mask) | polynucleotide[i];
    
    uint32_t *p = dict_oligonucleotide_get(hash_Table, key);
    if (p != NULL)
      (*p) ++;
    else {
      dict_oligonucleotide_set_at(hash_Table, key, 1);
    }
  }
  
  // Create an array of elements from hash_Table.
  int elements_Array_Size=dict_oligonucleotide_size(hash_Table), i=0;
  element * elements_Array=malloc(elements_Array_Size*sizeof(element));
  if (!elements_Array) abort();
  for M_EACH(item, hash_Table, OLIGONUCLEOTIDE_OPLIST) {
      elements_Array[i++]=((element){item->key, item->value});
    }
  dict_oligonucleotide_clear(hash_Table);
  
  // Sort elements_Array.
  qsort(elements_Array, elements_Array_Size, sizeof(element), element_Compare);
  
  // Print the frequencies for each oligonucleotide.
  for(int output_Position=0, i=0; i<elements_Array_Size; i++) {
    
    // Convert the key for the oligonucleotide to a string.
    char oligonucleotide[desired_Length_For_Oligonucleotides+1];
    for(int j=desired_Length_For_Oligonucleotides-1; j>-1; j--){
      oligonucleotide[j]=NUCLEOTIDE_FOR_CODE(elements_Array[i].key);
      elements_Array[i].key>>=2;
    }
    oligonucleotide[desired_Length_For_Oligonucleotides]='\0';
    
    // Output the frequency for oligonucleotide to output.
    output_Position+=snprintf(output+output_Position
                              , MAXIMUM_OUTPUT_LENGTH-output_Position, "%s %.3f\n", oligonucleotide
                              , 100.0f*elements_Array[i].value
                              /(polynucleotide_Length-desired_Length_For_Oligonucleotides+1));
  }
  
  free(elements_Array);
}


// Generate a count for the number of times oligonucleotide appears in
// polynucleotide and then save it to output.
static void
generate_Count_For_Oligonucleotide(const char * const polynucleotide,
                                   const int polynucleotide_Length,
                                   const char * const oligonucleotide,
                                   char * const output)
{
  const int oligonucleotide_Length=strlen(oligonucleotide);
  assert (2*oligonucleotide_Length <= 64);
  
  dict_oligonucleotide_t hash_Table;
  dict_oligonucleotide_init(hash_Table);
  
  uint64_t key = 0;
  const uint64_t mask=(1ULL<<(2*oligonucleotide_Length))-1;
  
  // For the first several nucleotides we only need to append them to key in
  // preparation for the insertion of complete oligonucleotides to hash_Table.
  for(int i=0; i<oligonucleotide_Length-1; i++)
    key=(key<<2 & mask) | polynucleotide[i];

  // Add all the complete oligonucleotides of oligonucleotide_Length to
  // hash_Table and update the count for each oligonucleotide.
  for(int i=oligonucleotide_Length-1; i<polynucleotide_Length; i++){
    key=(key<<2 & mask) | polynucleotide[i];
    
    uint32_t *p = dict_oligonucleotide_get(hash_Table, key);
    if (p != NULL)
      (*p) ++;
    else {
      dict_oligonucleotide_set_at(hash_Table, key, 1);
    }
  }
  
  // Generate the key for oligonucleotide.
  key=0;
  for(int i=0; i<oligonucleotide_Length; i++)
    key=(key<<2) | CODE_FOR_NUCLEOTIDE(oligonucleotide[i]);
  
  // Output the count for oligonucleotide to output.
  uint32_t *p = dict_oligonucleotide_get(hash_Table, key);
  uintmax_t count= p== NULL ? 0 : *p;
  snprintf(output, MAXIMUM_OUTPUT_LENGTH, "%ju\t%s", count, oligonucleotide);
  
  dict_oligonucleotide_clear(hash_Table);
}

int main()
{
  char buffer[MAXIMUM_OUTPUT_LENGTH];
  worker_t worker;
  
  worker_init(worker, 0, 1, NULL);
  
  // Find the start of the third polynucleotide.
  while(fgets(buffer, sizeof(buffer), stdin) && memcmp(">THREE", buffer
                                                       , sizeof(">THREE")-1));
  
  // Start with 1 MB of storage for reading in the polynucleotide and grow
  // geometrically.
  int polynucleotide_Capacity=1048576;
  int polynucleotide_Length=0;
  char * polynucleotide=malloc(polynucleotide_Capacity);
  if (!polynucleotide) abort();
  
  // Start reading and encoding the third polynucleotide.
  while(fgets(buffer, sizeof(buffer), stdin) && buffer[0]!='>'){
    for(int i=0; buffer[i]!='\0'; i++)
      if(buffer[i]!='\n')
        polynucleotide[polynucleotide_Length++]
          =CODE_FOR_NUCLEOTIDE(buffer[i]);
    
    // Make sure we still have enough memory allocated for any potential
    // nucleotides in the next line.
    if(polynucleotide_Capacity-polynucleotide_Length<sizeof(buffer)) {
      polynucleotide=realloc(polynucleotide, polynucleotide_Capacity*=2);
      if (!polynucleotide) abort();
    }
  }
  
  char output_Buffer[7][MAXIMUM_OUTPUT_LENGTH];
  char (*output)[MAXIMUM_OUTPUT_LENGTH] = output_Buffer;
  
  // Do the following functions in parallel.
  worker_sync_t block;
  worker_start(block);
  
  WORKER_SPAWN(worker, block, (polynucleotide, polynucleotide_Length, output), {
      generate_Count_For_Oligonucleotide(polynucleotide,
                                         polynucleotide_Length,
                                         "GGTATTTTAATTTATAGT", output[6]);
    }, ());

  WORKER_SPAWN(worker, block, (polynucleotide, polynucleotide_Length, output), {
      generate_Count_For_Oligonucleotide(polynucleotide,
                                         polynucleotide_Length,
                                         "GGTATTTTAATT", output[5]);
    }, ());
  
  WORKER_SPAWN(worker, block, (polynucleotide, polynucleotide_Length, output), {
      generate_Count_For_Oligonucleotide(polynucleotide,
                                         polynucleotide_Length,
                                         "GGTATT", output[4]);
    }, ());
  
  WORKER_SPAWN(worker, block, (polynucleotide, polynucleotide_Length, output), {
      generate_Count_For_Oligonucleotide(polynucleotide,
                                         polynucleotide_Length,
                                         "GGTA", output[3]);
    }, ());
  
  WORKER_SPAWN(worker, block, (polynucleotide, polynucleotide_Length, output), {
      generate_Count_For_Oligonucleotide(polynucleotide,
                                         polynucleotide_Length,
                                         "GGT", output[2]);  
    }, ());

  WORKER_SPAWN(worker, block, (polynucleotide, polynucleotide_Length, output), {
      generate_Frequencies_For_Desired_Length_Oligonucleotides(polynucleotide,
                                                               polynucleotide_Length,
                                                               2,
                                                               output[1]);
    }, ());

  generate_Frequencies_For_Desired_Length_Oligonucleotides(polynucleotide,
                                                           polynucleotide_Length,
                                                           1,
                                                           output[0]);
  worker_sync(block);
  
  // Output the results to stdout.
  for(int i=0; i<7; i++)
    printf("%s\n", output_Buffer[i]);
  
  free(polynucleotide);
  worker_clear(worker);
  
  return 0;
}
