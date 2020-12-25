/*
 * M*LIB - SNAPSHOT Module
 *
 * Copyright (c) 2017-2020, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef MSTARLIB_SNAPSHOT
#define MSTARLIB_SNAPSHOT

#include "m-atomic.h"
#include "m-core.h"
#include "m-genint.h"

#ifndef M_NAMING_INIT
#define M_NAMING_INIT _init
#endif

M_BEGIN_PROTECTED_CODE

/* Define a Single Producer Single Consummer snapshot and its functions
   USAGE: SNAPSHOT_SPSC_DEF(name, type[, oplist]) */
#define SNAPSHOT_SPSC_DEF(name, ...)                                          \
  M_BEGIN_PROTECTED_CODE                                                      \
  SNAPSHOTI_SPSC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_C(name, _t) ), \
                         (name, __VA_ARGS__                                       , M_C(name, _t) ))) \
  M_END_PROTECTED_CODE


/* Define a Single Producer Single Consummer snapshot and its functions
   as the given name name_t
   USAGE: SNAPSHOT_SPSC_DEF_AS(name, name_t, type[, oplist]) */
#define SNAPSHOT_SPSC_DEF_AS(name, name_t, ...)                               \
  M_BEGIN_PROTECTED_CODE                                                      \
  SNAPSHOTI_SPSC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t ), \
                         (name, __VA_ARGS__                                       , name_t ))) \
  M_END_PROTECTED_CODE


/* Define a Single Producer Multiple Consummer snapshot and its functions
   USAGE: SNAPSHOT_SPMC_DEF(name, type[, oplist]) */
#define SNAPSHOT_SPMC_DEF(name, ...)                                          \
  M_BEGIN_PROTECTED_CODE                                                      \
  SNAPSHOTI_SPMC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_C(name, _t) ), \
                         (name, __VA_ARGS__                                       , M_C(name, _t) ))) \
  M_END_PROTECTED_CODE


/* Define a Single Producer Multiple Consummer snapshot and its functions
   as the given name name_t
   USAGE: SNAPSHOT_SPMC_DEF_AS(name, type[, oplist]) */
#define SNAPSHOT_SPMC_DEF_AS(name, name_t, ...)                               \
  M_BEGIN_PROTECTED_CODE                                                      \
  SNAPSHOTI_SPMC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t ), \
                         (name, __VA_ARGS__                                       , name_t ))) \
  M_END_PROTECTED_CODE


/* Define a Multiple Producer Multiple Consummer snapshot and its functions
   USAGE: SNAPSHOT_MPMC_DEF(name, type[, oplist]) */
#define SNAPSHOT_MPMC_DEF(name, ...)                                          \
  M_BEGIN_PROTECTED_CODE                                                      \
  SNAPSHOTI_MPMC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), M_C(name, _t) ), \
                         (name, __VA_ARGS__                                       , M_C(name, _t) ))) \
  M_END_PROTECTED_CODE


/* Define a Multiple Producer Multiple Consummer snapshot and its functions
   as the given name name_t
   USAGE: SNAPSHOT_MPMC_DEF_AS(name, name_t, type[, oplist]) */
#define SNAPSHOT_MPMC_DEF_AS(name, name_t, ...)                               \
  M_BEGIN_PROTECTED_CODE                                                      \
  SNAPSHOTI_MPMC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t ), \
                         (name, __VA_ARGS__                                       , name_t ))) \
  M_END_PROTECTED_CODE


/* Define the oplist of a snapshot (SPSC, SPMC or MPMC).
   USAGE: SNAPSHOT_OPLIST(name[, oplist]) */
#define SNAPSHOT_OPLIST(...)                                                  \
  SNAPSHOTI_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                             \
                      ((__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)() ), \
                       (__VA_ARGS__ )))


/********************************** INTERNAL ************************************/

// deferred evaluation of the input
#define SNAPSHOTI_OPLIST_P1(arg) SNAPSHOTI_OPLIST_P2 arg

/* Validation of the given oplist */
#define SNAPSHOTI_OPLIST_P2(name, oplist)                                     \
  M_IF_OPLIST(oplist)(SNAPSHOTI_OPLIST_P3, SNAPSHOTI_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define SNAPSHOTI_OPLIST_FAILURE(name, oplist)                                \
  ((M_LIB_ERROR(ARGUMENT_OF_SNAPSHOT_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* Define the oplist of a snapshot */
#define SNAPSHOTI_OPLIST_P3(name, oplist)                                     \
  (INIT(M_F(name, M_NAMING_INIT)),						                        \
   INIT_SET(M_F(name, M_NAMING_INIT_SET)),					                  \
   SET(M_F(name, M_NAMING_SET)),						                          \
   CLEAR(M_F(name, M_NAMING_CLEAR)),						                      \
   ,TYPE(M_T(name, ct))                                                      \
   ,SUBTYPE(M_T3(name,_subtype, ct))                                           \
   OPLIST(oplist),                                                    \
   M_IF_METHOD(INIT_MOVE, oplist)(INIT_MOVE(M_F3(name, M_NAMING_INIT, move)),),	\
   M_IF_METHOD(MOVE, oplist)(MOVE(M_F(name, move)),)			            \
   )


/******************************** INTERNAL SPSC **********************************/

/* Flag defining the atomic state of a snapshot:
 * - r: Index of the read buffer Range [0..2]
 * - w: Index of the write buffer  Range [0..2]
 * - f: Next index of the write buffer when a shot is taken Range [0..2]
 * - b: Boolean indicating that the read buffer shall be updated
 * all fields packed in an unsigned char type.
 */
#define SNAPSHOTI_SPSC_FLAG(r, w, f, b)                                       \
  ((unsigned char)( ( (r) << 4) | ((w) << 2) | ((f)) | ((b) << 6)))
#define SNAPSHOTI_SPSC_R(flags)                                               \
  (((unsigned int) (flags) >> 4) & 0x03u)
#define SNAPSHOTI_SPSC_W(flags)                                               \
  (((unsigned int) (flags) >> 2) & 0x03u)
#define SNAPSHOTI_SPSC_F(flags)                                               \
  (((unsigned int) (flags) >> 0) & 0x03u)
#define SNAPSHOTI_SPSC_B(flags)                                               \
  (((unsigned int) (flags) >> 6) & 0x01u)

/* NOTE: Due to atomic_load only accepting non-const pointer,
   we can't have any const in the interface. */
#define SNAPSHOTI_SPSC_FLAGS_CONTRACT(flags)                                  \
  M_ASSERT(SNAPSHOTI_SPSC_R(flags) != SNAPSHOTI_SPSC_W(flags)                 \
         && SNAPSHOTI_SPSC_R(flags) != SNAPSHOTI_SPSC_F(flags)                \
         && SNAPSHOTI_SPSC_W(flags) != SNAPSHOTI_SPSC_F(flags))

#define SNAPSHOTI_SPSC_CONTRACT(snap)        do {                             \
    M_ASSERT((snap) != NULL);                                                 \
    unsigned char f = atomic_load (&(snap)->flags);                           \
    SNAPSHOTI_SPSC_FLAGS_CONTRACT(f);                                         \
  } while (0)

// A snapshot is basically an atomic triple buffer (Lock Free)
// between a single producer thread and a single consummer thread.
#define SNAPSHOTI_SPSC_MAX_BUFFER             3

// Defered evaluation of the arguments.
#define SNAPSHOTI_SPSC_DEF_P1(arg)        SNAPSHOTI_SPSC_DEF_P2 arg

/* Validate the oplist before going further */
#define SNAPSHOTI_SPSC_DEF_P2(name, type, oplist, snapshot_t)                 \
  M_IF_OPLIST(oplist)(SNAPSHOTI_SPSC_DEF_P3, SNAPSHOTI_SPSC_DEF_FAILURE)(name, type, oplist, snapshot_t)

/* Stop processing with a compilation failure */
#define SNAPSHOTI_SPSC_DEF_FAILURE(name, type, oplist, snapshot_t)            \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SNAPSHOT_SPSC_DEF): the given argument is not a valid oplist: " #oplist)

/* Expand the type and the functions of a SPSC snapshot */
#define SNAPSHOTI_SPSC_DEF_P3(name, type, oplist, snapshot_t)                 \
                                                                              \
  /* Create an aligned type to avoid false sharing between threads */         \
  typedef struct M_C(name, _aligned_type_s) {                                 \
    type         x;                                                           \
    M_CACHELINE_ALIGN(align, type);                                           \
  } M_C(name, _aligned_type_ct);                                              \
                                                                              \
  typedef struct M_C(name, _s) {                                              \
    M_C(name, _aligned_type_ct)  data[SNAPSHOTI_SPSC_MAX_BUFFER];             \
    atomic_uchar flags;                                                       \
  } snapshot_t[1];                                                            \
  typedef struct M_T(name, s) *M_T(name, ptr);                                \
  typedef const struct M_T(name, s) *M_T(name, srcptr);                       \
                                                                              \
  /* Define internal types for oplist */                                      \
  typedef snapshot_t M_T(name, ct);                                           \
  typedef type       M_T3(name, subtype, ct);                                   \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_INIT)(snapshot_t snap)                                           \
  {                                                                           \
    M_ASSERT(snap != NULL);                                                   \
    for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                      \
      M_CALL_INIT(oplist, snap->data[i].x);                                   \
    }                                                                         \
    atomic_init (&snap->flags, SNAPSHOTI_SPSC_FLAG(0, 1, 2, 0));              \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_CLEAR)(snapshot_t snap)                                          \
  {                                                                           \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
    for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                      \
      M_CALL_CLEAR(oplist, snap->data[i].x);                                  \
    }                                                                         \
  }                                                                           \
                                                                              \
  /* const is missing for org due to use of atomic_load of org */             \
  static inline void                                                          \
  M_F(name, M_NAMING_INIT_SET)(snapshot_t snap, snapshot_t org)                       \
  {                                                                           \
    SNAPSHOTI_SPSC_CONTRACT(org);                                             \
    M_ASSERT(snap != NULL && snap != org);                                    \
    for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                      \
      M_CALL_INIT_SET(oplist, snap->data[i].x, org->data[i].x);               \
    }                                                                         \
    atomic_init (&snap->flags, atomic_load(&org->flags));                     \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
  }                                                                           \
                                                                              \
  /* const is missing for org due to use of atomic_load of org */             \
  static inline void                                                          \
  M_F(name, M_NAMING_SET)(snapshot_t snap, snapshot_t org)                    \
  {                                                                           \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
    SNAPSHOTI_SPSC_CONTRACT(org);                                             \
    for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                      \
      M_CALL_SET(oplist, snap->data[i].x, org->data[i].x);                    \
    }                                                                         \
    atomic_init (&snap->flags, atomic_load(&org->flags));                     \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
  }                                                                           \
                                                                              \
  M_IF_METHOD(INIT_MOVE, oplist)(                                             \
    static inline void                                                        \
    M_F3(name, M_NAMING_INIT, move)(snapshot_t snap, snapshot_t org)          \
    {                                                                         \
      SNAPSHOTI_SPSC_CONTRACT(org);                                           \
      M_ASSERT(snap != NULL && snap != org);                                  \
      for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                    \
        M_CALL_INIT_MOVE(oplist, snap->data[i].x, org->data[i].x);            \
      }                                                                       \
      atomic_store (&snap->flags, atomic_load(&org->flags));                  \
      atomic_store (&org->flags, SNAPSHOTI_SPSC_FLAG(0,0,0,0) );              \
      SNAPSHOTI_SPSC_CONTRACT(snap);                                          \
    }                                                                         \
  ,) /* IF_METHOD (INIT_MOVE) */                                              \
                                                                              \
  M_IF_METHOD(MOVE, oplist)(                                                  \
    static inline void                                                        \
    M_F(name, move)(snapshot_t snap,                                         \
                                        snapshot_t org)                       \
    {                                                                         \
      SNAPSHOTI_SPSC_CONTRACT(snap);                                          \
      SNAPSHOTI_SPSC_CONTRACT(org);                                           \
      M_ASSERT(snap != org);                                                  \
      for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                    \
        M_CALL_MOVE(oplist, snap->data[i].x, org->data[i].x);                 \
      }                                                                       \
      atomic_store (&snap->flags, atomic_load(&org->flags));                  \
      atomic_store (&org->flags, SNAPSHOTI_SPSC_FLAG(0,0,0,0) );              \
      SNAPSHOTI_SPSC_CONTRACT(snap);                                          \
    }                                                                         \
  ,) /* IF_METHOD (MOVE) */                                                   \
                                                                              \
  static inline type *                                                        \
  M_F(name, write)(snapshot_t snap)                                          \
  {                                                                           \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
    unsigned char nextFlags, origFlags = atomic_load (&snap->flags);          \
    /* Atomic CAS operation */                                                \
    do {                                                                      \
      /* Swap F and W buffer, setting exchange flag */                        \
      nextFlags = SNAPSHOTI_SPSC_FLAG(SNAPSHOTI_SPSC_R(origFlags),            \
                                      SNAPSHOTI_SPSC_F(origFlags),            \
                                      SNAPSHOTI_SPSC_W(origFlags), 1);        \
      /* exponential backoff is not needed as there can't be more             \
         than 2 threads which try to update the data. */                      \
    } while (!atomic_compare_exchange_weak (&snap->flags, &origFlags,         \
                                            nextFlags));                      \
    /* Return new write buffer for new updating */                            \
    return &snap->data[SNAPSHOTI_SPSC_W(nextFlags)].x;                        \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_F(name, read)(snapshot_t snap)                                           \
  {                                                                           \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
    unsigned char nextFlags, origFlags = atomic_load (&snap->flags);          \
    /* Atomic CAS operation */                                                \
    do {                                                                      \
      /* If no exchange registered, do nothing and keep the same */           \
      if (!SNAPSHOTI_SPSC_B(origFlags)) {                                     \
        nextFlags = origFlags;                                                \
        break;                                                                \
      }                                                                       \
      /* Swap R and F buffer, clearing exchange flag */                       \
      nextFlags = SNAPSHOTI_SPSC_FLAG(SNAPSHOTI_SPSC_F(origFlags),            \
                                      SNAPSHOTI_SPSC_W(origFlags),            \
                                      SNAPSHOTI_SPSC_R(origFlags), 0);        \
      /* exponential backoff is not needed as there can't be more             \
         than 2 threads which try to update the data. */                      \
    } while (!atomic_compare_exchange_weak (&snap->flags, &origFlags,         \
                                            nextFlags));                      \
    /* Return current read buffer */                                          \
    return M_CONST_CAST(type, &snap->data[SNAPSHOTI_SPSC_R(nextFlags)].x);    \
  }                                                                           \
                                                                              \
  /* Non const due to use of atomic_load */                                   \
  static inline bool                                                          \
  M_P(name, updated)(snapshot_t snap)                                      \
  {                                                                           \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
    unsigned char flags = atomic_load (&snap->flags);                         \
    return SNAPSHOTI_SPSC_B(flags);                                           \
  }                                                                           \
                                                                              \
  /* Non const due to use of atomic_load */                                   \
  static inline type *                                                        \
  M_F3(name, get, write_buffer)(snapshot_t snap)                               \
  {                                                                           \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
    unsigned char flags = atomic_load(&snap->flags);                          \
    return &snap->data[SNAPSHOTI_SPSC_W(flags)].x;                            \
  }                                                                           \
                                                                              \
  /* Non const due to use of atomic_load */                                   \
  static inline type const *                                                  \
  M_F3(name, get, read_buffer)(snapshot_t snap)                                \
  {                                                                           \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                            \
    unsigned char flags = atomic_load(&snap->flags);                          \
    return  M_CONST_CAST(type, &snap->data[SNAPSHOTI_SPSC_R(flags)].x);       \
  }                                                                           \
  


/******************************** INTERNAL SPMC **********************************/

#define SNAPSHOTI_SPMC_INT_FLAG(w, n) ( ((w) << 1) | (n) )
#define SNAPSHOTI_SPMC_INT_FLAG_W(f)  ((f) >> 1)
#define SNAPSHOTI_SPMC_INT_FLAG_N(f)  ((f) & 1)

// 2 more buffer than the number of readers are needed
#define SNAPSHOTI_SPMC_EXTRA_BUFFER 2

#define SNAPSHOTI_SPMC_MAX_READER (GENINT_MAX_ALLOC-SNAPSHOTI_SPMC_EXTRA_BUFFER)

/* Internal structure to handle SPMC snapshot but return an unique index in the buffer array.
   - lastNext: last published written index + next flag (format SNAPSHOTI_SPMC_INT_FLAG)
   - currentWrite: the index being currently written.
   - n_reader : number of readers 
   - cptTab: ref counter array to keep track of how many readers use the corresponding buffer.
   - freeList: a pool of free integers.
*/
typedef struct snapshot_mrsw_int_s {
  atomic_uint  lastNext;
  unsigned int currentWrite;
  size_t       n_reader;
  atomic_uint *cptTab;
  genint_t     freeList;
} snapshot_mrsw_int_ct[1];

// can't check currentWrite due to potential data race on it
#define SNAPSHOTI_SPMC_INT_CONTRACT(s) do {                                   \
    M_ASSERT (s != NULL);                                                     \
    M_ASSERT (s->n_reader > 0 && s->n_reader <= SNAPSHOTI_SPMC_MAX_READER);   \
    M_ASSERT ((size_t)SNAPSHOTI_SPMC_INT_FLAG_W(atomic_load(&s->lastNext))    \
            <= s->n_reader + SNAPSHOTI_SPMC_EXTRA_BUFFER);                    \
    M_ASSERT (s->cptTab != NULL);                                             \
  } while (0)

/* Initialize snapshot_mrsw_int_ct for n readers (constructor) */
static inline void
M_F3(snapshot, mrsw_int, M_NAMING_INIT)(snapshot_mrsw_int_ct s, size_t n)
{
  M_ASSERT (s != NULL);
  M_ASSERT (n >= 1 && n <= SNAPSHOTI_SPMC_MAX_READER);
  s->n_reader = n;
  n += SNAPSHOTI_SPMC_EXTRA_BUFFER;

  // Initialize the counters to zero (no reader use it)
  atomic_uint *ptr = M_MEMORY_REALLOC (atomic_uint, NULL, n);
  if (M_UNLIKELY (ptr == NULL)) {
    M_MEMORY_FULL(sizeof (atomic_uint) * n);
    return;
  }
  s->cptTab = ptr;
  for(size_t i = 0; i < n; i++)
    atomic_init(&s->cptTab[i], 0U);
  M_F(genint, M_NAMING_INIT)(s->freeList, (unsigned int) n);

  // Get a free buffer and set it as available for readers
  unsigned int w = genint_pop(s->freeList);
  M_ASSERT (w != GENINT_ERROR);
  atomic_store(&s->cptTab[w], 1U);
  atomic_init(&s->lastNext, SNAPSHOTI_SPMC_INT_FLAG(w, true));

  // Get working buffer
  s->currentWrite = genint_pop(s->freeList);
  M_ASSERT (s->currentWrite != GENINT_ERROR);
  atomic_store(&s->cptTab[s->currentWrite], 1U);
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
}

/* Clear snapshot_mrsw_int_ct (destructor) */
static inline void
M_F3(snapshot, mrsw_int, M_NAMING_CLEAR)(M_T3(snapshot, mrsw_int, ct) s)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  M_MEMORY_FREE(s->cptTab);
  M_F(genint, M_NAMING_CLEAR)(s->freeList);
  s->cptTab = NULL;
  s->n_reader = 0;
}

/* Return the current index that is written in the buffer */
static inline unsigned int
snapshot_mrsw_int_get_write_idx(snapshot_mrsw_int_ct s)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return s->currentWrite;
}

/* Return the number of readers */
static inline unsigned int
snapshot_mrsw_int_size(snapshot_mrsw_int_ct s)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return (unsigned int) s->n_reader;
}

/* Give the current index that is written to the readers,
   and return new available index for writter */
static inline unsigned int
snapshot_mrsw_int_write_idx(snapshot_mrsw_int_ct s, unsigned int idx)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);

  // Provide the finalized written buffer to the readers.
  unsigned int newNext, previous = atomic_load(&s->lastNext);
  do {
    newNext = SNAPSHOTI_SPMC_INT_FLAG(idx, true);
  } while (!atomic_compare_exchange_weak(&s->lastNext, &previous, newNext));

  if (SNAPSHOTI_SPMC_INT_FLAG_N(previous)) {
    // Reuse previous buffer as it was not used by any reader
    idx = SNAPSHOTI_SPMC_INT_FLAG_W(previous);
    // Some other read threads may already have try to reserve this index
    // So atomic_load(&s->cptTab[idx]) can be greater than 1.
    // However they will fail to ack it in lastNext,
    // so they will remove their reservation later
  } else {
    // Remove the writer thread counter from the count of the previous buffer
    idx = SNAPSHOTI_SPMC_INT_FLAG_W(previous);
    unsigned int c = atomic_fetch_sub(&s->cptTab[idx], 1U);
    M_ASSERT (c != 0 && c <= s->n_reader + 1);
    // Get a new buffer.
    if (c != 1) {
      // If someone else keeps a ref on the buffer, we can't reuse it
      // get another free one.
      idx = genint_pop(s->freeList);
      M_ASSERT(idx != GENINT_ERROR);
    } else {
      // No other thread keep track of this buffer.
      // Reuse it.
    }
    M_ASSERT (idx < s->n_reader + SNAPSHOTI_SPMC_EXTRA_BUFFER);
    M_ASSERT (atomic_load(&s->cptTab[idx]) == 0);
    atomic_store(&s->cptTab[idx], 1U);
  }
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return idx;
}

/* Perform a swap of the current write buffer and return a new one */
static inline unsigned int
snapshot_mrsw_int_write(snapshot_mrsw_int_ct s)
{
  s->currentWrite = snapshot_mrsw_int_write_idx(s, s->currentWrite);
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return s->currentWrite;
}

/* Start writing to the write buffer and return its index */
static inline unsigned int
snapshot_mrsw_int_write_start(snapshot_mrsw_int_ct s)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  // Get a new buffer.
  unsigned int idx = genint_pop(s->freeList);
  M_ASSERT (idx != GENINT_ERROR);
  M_ASSERT (idx < s->n_reader + SNAPSHOTI_SPMC_EXTRA_BUFFER);
  M_ASSERT (atomic_load(&s->cptTab[idx]) == 0);
  atomic_store(&s->cptTab[idx], 1U);
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return idx;
}

/* End writing to the given write buffer */
static inline void
snapshot_mrsw_int_write_end(snapshot_mrsw_int_ct s, unsigned int idx)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);

  // Provide this write bufer to the readers
  unsigned int newNext, previous = atomic_load(&s->lastNext);
  do {
    newNext = SNAPSHOTI_SPMC_INT_FLAG(idx, true);
  } while (!atomic_compare_exchange_weak(&s->lastNext, &previous, newNext));

  // Free the previous write buffer
  idx = SNAPSHOTI_SPMC_INT_FLAG_W(previous);
  unsigned int c = atomic_fetch_sub(&s->cptTab[idx], 1U);
  M_ASSERT (c != 0 && c <= s->n_reader + 1);
  if (c == 1) {
    genint_push(s->freeList, idx);
  }
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
}

/* Start reading the latest written buffer and return the index to it */
static inline unsigned int
snapshot_mrsw_int_read_start(snapshot_mrsw_int_ct s)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  unsigned int idx, previous;
 reload:
  // Load the last published index + Next flag
  previous = atomic_load(&s->lastNext);
  while (true) {
    // Get the last published index
    idx = SNAPSHOTI_SPMC_INT_FLAG_W(previous);
    // Load the number of threads using this index
    unsigned int c = atomic_load(&s->cptTab[idx]);
    M_ASSERT (c <= s->n_reader + 1);
    // Reserve the index if it still being reserved by someone else
    if (M_UNLIKELY (c == 0
                    || !atomic_compare_exchange_strong(&s->cptTab[idx], &c, c+1)))
      goto reload;
    // Try to ack it
    unsigned int newNext = SNAPSHOTI_SPMC_INT_FLAG(idx, false);
  reforce:
    if (M_LIKELY (atomic_compare_exchange_strong(&s->lastNext, &previous, newNext)))
      break;
    // We have been preempted by another thread
    if (idx == SNAPSHOTI_SPMC_INT_FLAG_W(previous)) {
      // This is still ok if the index has not changed
      // We can get previous to true again if the writer has recycled the index,
      // while we reserved it, and the reader get prempted until its CAS.
      if (M_UNLIKELY (SNAPSHOTI_SPMC_INT_FLAG_N(previous) == true)) goto reforce;
      break;
    }
    // Free the reserved index as we failed it to ack it
    c = atomic_fetch_sub(&s->cptTab[idx], 1U);
    M_ASSERT (c != 0 && c <= s->n_reader + 1);
    if (c == 1) {
      genint_push(s->freeList, idx);
    }
  }
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return idx;
}

/* End the reading the given buffer */
static inline void
snapshot_mrsw_int_read_end(snapshot_mrsw_int_ct s, unsigned int idx)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  M_ASSERT (idx < s->n_reader + SNAPSHOTI_SPMC_EXTRA_BUFFER);
  // Decrement reference counter of the buffer
  unsigned int c = atomic_fetch_sub(&s->cptTab[idx], 1U);
  M_ASSERT (c != 0 && c <= s->n_reader + 1);
  if (c == 1) {
    // Buffer no longer used by any reader thread.
    // Push back index in free list
    genint_push(s->freeList, idx);
  }
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
}

/* Contract of a SPMC snapshot.
   Nothing notable as it can be accessed concurrently */
#define SNAPSHOTI_SPMC_CONTRACT(snap) do {                                    \
    M_ASSERT (snap != NULL);                                                  \
    M_ASSERT (snap->data != NULL);                                            \
  } while (0)


// Defered evaluation
#define SNAPSHOTI_SPMC_DEF_P1(arg)        SNAPSHOTI_SPMC_DEF_P2 arg

/* Validate the oplist before going further */
#define SNAPSHOTI_SPMC_DEF_P2(name, type, oplist, snapshot_t)                 \
  M_IF_OPLIST(oplist)(SNAPSHOTI_SPMC_DEF_P3, SNAPSHOTI_SPMC_DEF_FAILURE)(name, type, oplist, snapshot_t)

/* Stop processing with a compilation failure */
#define SNAPSHOTI_SPMC_DEF_FAILURE(name, type, oplist, snapshot_t)            \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SNAPSHOT_SPMC_DEF): the given argument is not a valid oplist: " #oplist)

/* Expand the type and the functions of a SPMC snapshot */
#define SNAPSHOTI_SPMC_DEF_P3(name, type, oplist, snapshot_t)                 \
                                                                              \
  /* Create an aligned type to avoid false sharing between threads */         \
  typedef struct M_C(name, _aligned_type_s) {                                 \
    type         x;                                                           \
    M_CACHELINE_ALIGN(align, type);                                           \
  } M_C(name, _aligned_type_ct);                                              \
                                                                              \
  typedef struct M_C(name, _s) {                                              \
    M_C(name, _aligned_type_ct)  *data;                                       \
    snapshot_mrsw_int_ct          core;                                       \
  } snapshot_t[1];                                                            \
                                                                              \
  /* Define internal types for oplist */                                      \
  typedef snapshot_t M_C(name, _ct);                                          \
  typedef type       M_C(name, _subtype_ct);                                  \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_INIT)(snapshot_t snap, size_t nReader)                   \
  {                                                                           \
    M_ASSERT (snap != NULL);                                                  \
    M_ASSERT (nReader > 0 && nReader <= SNAPSHOTI_SPMC_MAX_READER);           \
    snap->data = M_CALL_REALLOC(oplist, M_C(name, _aligned_type_ct),          \
                                NULL, nReader+SNAPSHOTI_SPMC_EXTRA_BUFFER);   \
    if (M_UNLIKELY (snap->data == NULL)) {                                    \
      M_MEMORY_FULL(sizeof(M_C(name, _aligned_type_ct)) *                     \
                    (nReader+SNAPSHOTI_SPMC_EXTRA_BUFFER));                   \
      return;                                                                 \
    }                                                                         \
    for(size_t i = 0; i < nReader + SNAPSHOTI_SPMC_EXTRA_BUFFER; i++) {       \
      M_CALL_INIT(oplist, snap->data[i].x);                                   \
    }                                                                         \
    snapshot_mrsw_int_init(snap->core, nReader);                              \
    SNAPSHOTI_SPMC_CONTRACT(snap);                                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_CLEAR)(snapshot_t snap)                                          \
  {                                                                           \
    SNAPSHOTI_SPMC_CONTRACT(snap);                                            \
    size_t nReader = snapshot_mrsw_int_size(snap->core);                      \
    for(size_t i = 0; i < nReader + SNAPSHOTI_SPMC_EXTRA_BUFFER; i++) {       \
      M_CALL_CLEAR(oplist, snap->data[i].x);                                  \
    }                                                                         \
    M_CALL_FREE(oplist, snap->data);                                          \
    M_F3(snapshot, mrsw_int, clear)(snap->core);                                      \
  }                                                                           \
                                                                              \
  static inline type *                                                        \
  M_F(name, write)(snapshot_t snap)                                          \
  {                                                                           \
    SNAPSHOTI_SPMC_CONTRACT(snap);                                            \
    const unsigned int idx = snapshot_mrsw_int_write(snap->core);             \
    return &snap->data[idx].x;                                                \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_F(name, read_start)(snapshot_t snap)                                     \
  {                                                                           \
    SNAPSHOTI_SPMC_CONTRACT(snap);                                            \
    const unsigned int idx = snapshot_mrsw_int_read_start(snap->core);        \
    return M_CONST_CAST(type, &snap->data[idx].x);                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, read_end)(snapshot_t snap, type const *old)                      \
  {                                                                           \
    SNAPSHOTI_SPMC_CONTRACT(snap);                                            \
    M_ASSERT (old != NULL);                                                   \
    const M_C(name, _aligned_type_ct) *oldx;                                  \
    oldx = M_CTYPE_FROM_FIELD(M_C(name, _aligned_type_ct), old, type, x);     \
    M_ASSERT (oldx >= snap->data);                                            \
    M_ASSERT (oldx < snap->data + snap->core->n_reader + SNAPSHOTI_SPMC_EXTRA_BUFFER); \
    M_ASSERT(snap->core->n_reader +SNAPSHOTI_SPMC_EXTRA_BUFFER < UINT_MAX);   \
    const unsigned int idx = (unsigned int) (oldx - snap->data);              \
    snapshot_mrsw_int_read_end(snap->core, idx);                              \
  }                                                                           \
                                                                              \
  static inline type *                                                        \
  M_F3(name, get, write_buffer)(snapshot_t snap)                               \
  {                                                                           \
    SNAPSHOTI_SPMC_CONTRACT(snap);                                            \
    const unsigned int idx = snapshot_mrsw_int_get_write_idx(snap->core);     \
    return &snap->data[idx].x;                                                \
  }                                                                           \
                                                                              \

//FIXME: Evaluate the needs for the methods _set_, _init_set.



/******************************** INTERNAL MPMC **********************************/

// MPMC is built upon SPMC

// Defered evaluation
#define SNAPSHOTI_MPMC_DEF_P1(arg)        SNAPSHOTI_MPMC_DEF_P2 arg

/* Validate the oplist before going further */
#define SNAPSHOTI_MPMC_DEF_P2(name, type, oplist, snapshot_t)                 \
  M_IF_OPLIST(oplist)(SNAPSHOTI_MPMC_DEF_P3, SNAPSHOTI_MPMC_DEF_FAILURE)(name, type, oplist, snapshot_t)

/* Stop processing with a compilation failure */
#define SNAPSHOTI_MPMC_DEF_FAILURE(name, type, oplist, snapshot_t)            \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SNAPSHOT_MPMC_DEF): the given argument is not a valid oplist: " #oplist)

/* Expand the type and the functions of a MPMC snapshot */
#define SNAPSHOTI_MPMC_DEF_P3(name, type, oplist, snapshot_t)                 \
                                                                              \
  SNAPSHOTI_SPMC_DEF_P1((M_C(name, _mrsw), type, oplist, M_C(name, _mrsw_pct))) \
                                                                              \
  typedef struct M_C(name, _s) {                                              \
    M_C(name, _mrsw_pct)  core;                                               \
  } snapshot_t[1];                                                            \
                                                                              \
  /* Define internal types for oplist */                                      \
  typedef snapshot_t M_C(name, _ct);                                          \
  typedef type       M_C(name, _subtype_ct);                                  \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_INIT)(snapshot_t snap, size_t nReader, size_t nWriter)           \
                               size_t nWriter)           \
  {                                                                           \
    M_F3(name, mrsw, M_NAMING_INIT)(snap->core, nReader + nWriter -1 );                 \
    unsigned int idx = snap->core->core->currentWrite;                        \
    snap->core->core->currentWrite = GENINT_ERROR;                            \
    snapshot_mrsw_int_write_end(snap->core->core, idx);                       \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F(name, M_NAMING_CLEAR)(snapshot_t snap)                                          \
  {                                                                           \
    M_F3(name, mrsw, M_NAMING_CLEAR)(snap->core);                        \
  }                                                                           \
                                                                              \
  static inline type *                                                        \
  M_F3(name, write, start)(snapshot_t snap)                                    \
  {                                                                           \
    SNAPSHOTI_SPMC_CONTRACT(snap->core);                                      \
    const unsigned int idx = snapshot_mrsw_int_write_start(snap->core->core); \
    return &snap->core->data[idx].x;                                          \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F3(name, write, end)(snapshot_t snap, type *old)                           \
  {                                                                           \
    SNAPSHOTI_SPMC_CONTRACT(snap->core);                                      \
    const M_T3(name, mrsw_aligned_type, ct) *oldx;                             \
    oldx = M_CTYPE_FROM_FIELD(M_C(name, _mrsw_aligned_type_ct), old, type, x); \
    M_ASSERT (oldx >= snap->core->data);                                      \
    M_ASSERT (oldx < snap->core->data + snap->core->core->n_reader + SNAPSHOTI_SPMC_EXTRA_BUFFER); \
    M_ASSERT(snap->core->core->n_reader + SNAPSHOTI_SPMC_EXTRA_BUFFER < UINT_MAX); \
    const unsigned int idx = (unsigned int) (oldx - snap->core->data);        \
    snapshot_mrsw_int_write_end(snap->core->core, idx);                       \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_F3(name, read, start)(snapshot_t snap)                                     \
  {                                                                           \
    return M_F3(name, mrsw, read_start)(snap->core);                     \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_F3(name, read, end)(snapshot_t snap, type const *old)                      \
  {                                                                           \
    M_F3(name, mrsw, read_end)(snap->core, old);                         \
  }


//TODO: _set_, _init_set

M_END_PROTECTED_CODE

#endif // MSTARLIB_SNAPSHOT
