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

/* Define a Single Producer Single Consummer snapshot and its functions
   USAGE: SNAPSHOT_SPSC_DEF(name, type[, oplist]) */
#define SNAPSHOT_SPSC_DEF(name, ...)                                    \
  SNAPSHOTI_SPSC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                     \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)() ), \
                         (name, __VA_ARGS__ )))


/* Define a Single Producer Multiple Consummer snapshot and its functions
   USAGE: SNAPSHOT_SPMC_DEF(name, type[, oplist]) */
#define SNAPSHOT_SPMC_DEF(name, ...)                                    \
  SNAPSHOTI_SPMC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                     \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)() ), \
                         (name, __VA_ARGS__ )))


/* Define a Multiple Producer Multiple Consummer snapshot and its functions
   USAGE: SNAPSHOT_MPMC_DEF(name, type[, oplist]) */
#define SNAPSHOT_MPMC_DEF(name, ...)                                    \
  SNAPSHOTI_MPMC_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                     \
                        ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)() ), \
                         (name, __VA_ARGS__ )))


/* Define the oplist of a snapshot (SPSC, SPMC or MPMC).
   USAGE: SNAPSHOT_OPLIST(name[, oplist]) */
#define SNAPSHOT_OPLIST(...)                                            \
  SNAPSHOTI_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                       \
                      ((__VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)() ), \
                       (__VA_ARGS__ )))


/********************************** INTERNAL ************************************/

// deferred evaluation of the input
#define SNAPSHOTI_OPLIST_P1(arg) SNAPSHOTI_OPLIST_P2 arg

/* Validation of the given oplist */
#define SNAPSHOTI_OPLIST_P2(name, oplist)					\
  M_IF_OPLIST(oplist)(SNAPSHOTI_OPLIST_P3, SNAPSHOTI_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define SNAPSHOTI_OPLIST_FAILURE(name, oplist)		\
  ((M_LIB_ERROR(ARGUMENT_OF_SNAPSHOT_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* Define the oplist of a snapshot */
#define SNAPSHOTI_OPLIST_P3(name, oplist)                               \
  (INIT(M_C(name, _init))						\
   ,INIT_SET(M_C(name, _init_set))					\
   ,SET(M_C(name, _set))						\
   ,CLEAR(M_C(name, _clear))						\
   ,TYPE(M_C(name, _t))                                                 \
   ,SUBTYPE(M_C(name_, type_t))						\
   ,OPLIST(oplist)                                                      \
   ,M_IF_METHOD(INIT_MOVE, oplist)(INIT_MOVE(M_C(name, _init_move)),)	\
   ,M_IF_METHOD(MOVE, oplist)(MOVE(M_C(name, _move)),)			\
   )


/******************************** INTERNAL SPSC **********************************/

/* Flag defining the atomic state of a snapshot:
 * - r: Index of the read buffer Range [0..2]
 * - w: Index of the write buffer  Range [0..2]
 * - f: Next index of the write buffer when a shot is taken Range [0..2]
 * - b: Boolean indicating that the read buffer shall be updated
 * all fields packed in an unsigned char type.
 */
#define SNAPSHOTI_SPSC_FLAG(r, w, f, b)					\
  ((unsigned char)( ( (r) << 4) | ((w) << 2) | ((f)) | ((b) << 6)))
#define SNAPSHOTI_SPSC_R(flags)			\
  (((flags) >> 4) & 0x03u)
#define SNAPSHOTI_SPSC_W(flags)			\
  (((flags) >> 2) & 0x03u)
#define SNAPSHOTI_SPSC_F(flags)			\
  (((flags) >> 0) & 0x03u)
#define SNAPSHOTI_SPSC_B(flags)			\
  (((flags) >> 6) & 0x01u)

/* NOTE: Due to atomic_load only accepting non-const pointer,
   we can't have any const in the interface. */
#define SNAPSHOTI_SPSC_FLAGS_CONTRACT(flags)                            \
  assert(SNAPSHOTI_SPSC_R(flags) != SNAPSHOTI_SPSC_W(flags)             \
	 && SNAPSHOTI_SPSC_R(flags) != SNAPSHOTI_SPSC_F(flags)          \
	 && SNAPSHOTI_SPSC_W(flags) != SNAPSHOTI_SPSC_F(flags))
#define SNAPSHOTI_SPSC_CONTRACT(snap)	do {                            \
    assert((snap) != NULL);						\
    unsigned char f = atomic_load (&(snap)->flags);                     \
    SNAPSHOTI_SPSC_FLAGS_CONTRACT(f);                                   \
  } while (0)

// A snapshot is basically an atomic triple buffer (Lock Free)
// between a single producer thread and a single consummer thread.
#define SNAPSHOTI_SPSC_MAX_BUFFER             3

// Defered evaluation of the arguments.
#define SNAPSHOTI_SPSC_DEF_P1(arg)	SNAPSHOTI_SPSC_DEF_P2 arg

/* Validate the oplist before going further */
#define SNAPSHOTI_SPSC_DEF_P2(name, type, oplist)            \
  M_IF_OPLIST(oplist)(SNAPSHOTI_SPSC_DEF_P3, SNAPSHOTI_SPSC_DEF_FAILURE)(name, type, oplist)

/* Stop processing with a compilation failure */
#define SNAPSHOTI_SPSC_DEF_FAILURE(name, type, oplist)                  \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SNAPSHOT_SPSC_DEF): the given argument is not a valid oplist: " #oplist)

#define SNAPSHOTI_SPSC_DEF_P3(name, type, oplist)                       \
                                                                        \
  /* Create an aligned type to avoid false sharing between threads */   \
  typedef struct M_C(name, _aligned_type_s) {                           \
    type         x;							\
    M_CACHELINE_ALIGN(align, type);                                     \
  } M_C(name, _aligned_type_t);                                         \
                                                                        \
  typedef struct M_C(name, _s) {					\
    M_C(name, _aligned_type_t)  data[SNAPSHOTI_SPSC_MAX_BUFFER];        \
    atomic_uchar flags;                                                 \
  } M_C(name, _t)[1];							\
  typedef struct M_C(name, _s) *M_C(name, _ptr);                        \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);               \
                                                                        \
  typedef type M_C(name, _type_t);                                      \
                                                                        \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                      \
                                                                        \
  static inline void M_C(name, _init)(M_C(name, _t) snap)               \
  {									\
    assert(snap != NULL);						\
    for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                \
      M_CALL_INIT(oplist, snap->data[i].x);                             \
    }									\
    atomic_init (&snap->flags, SNAPSHOTI_SPSC_FLAG(0, 1, 2, 0));        \
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
  }									\
                                                                        \
  static inline void M_C(name, _clear)(M_C(name, _t) snap)		\
  {									\
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
    for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                \
      M_CALL_CLEAR(oplist, snap->data[i].x);				\
    }									\
  }									\
                                                                        \
  static inline void M_C(name, _init_set)(M_C(name, _t) snap,		\
					  M_C(name, _t) org)		\
  {									\
    SNAPSHOTI_SPSC_CONTRACT(org);                                       \
    assert(snap != NULL && snap != org);				\
    for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                \
      M_CALL_INIT_SET(oplist, snap->data[i].x, org->data[i].x);		\
    }									\
    atomic_init (&snap->flags, atomic_load(&org->flags));		\
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
  }									\
                                                                        \
  static inline void M_C(name, _set)(M_C(name, _t) snap,		\
				     M_C(name, _t) org)			\
  {									\
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
    SNAPSHOTI_SPSC_CONTRACT(org);                                       \
    for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {                \
      M_CALL_SET(oplist, snap->data[i].x, org->data[i].x);              \
    }									\
    atomic_init (&snap->flags, atomic_load(&org->flags));		\
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
  }									\
                                                                        \
  M_IF_METHOD(INIT_MOVE, oplist)(					\
    static inline void M_C(name, _init_move)(M_C(name, _t) snap,        \
					     M_C(name, _t) org)		\
    {									\
      SNAPSHOTI_SPSC_CONTRACT(org);                                     \
      assert(snap != NULL && snap != org);				\
      for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {              \
	M_CALL_INIT_MOVE(oplist, snap->data[i].x, org->data[i].x);      \
      }									\
      atomic_store (&snap->flags, atomic_load(&org->flags));            \
      atomic_store (&org->flags, SNAPSHOTI_SPSC_FLAG(0,0,0,0) );        \
      SNAPSHOTI_SPSC_CONTRACT(snap);                                    \
    }									\
    ,) /* IF_METHOD (INIT_MOVE) */					\
                                                                        \
   M_IF_METHOD(MOVE, oplist)(                                           \
     static inline void M_C(name, _move)(M_C(name, _t) snap,            \
					 M_C(name, _t) org)		\
     {									\
       SNAPSHOTI_SPSC_CONTRACT(snap);					\
       SNAPSHOTI_SPSC_CONTRACT(org);                                    \
       assert(snap != org);						\
       for(int i = 0; i < SNAPSHOTI_SPSC_MAX_BUFFER; i++) {             \
	 M_CALL_MOVE(oplist, snap->data[i].x, org->data[i].x);		\
       }								\
       atomic_store (&snap->flags, atomic_load(&org->flags));           \
       atomic_store (&org->flags, SNAPSHOTI_SPSC_FLAG(0,0,0,0) );       \
       SNAPSHOTI_SPSC_CONTRACT(snap);					\
     }									\
     ,) /* IF_METHOD (MOVE) */						\
                                                                        \
                                                                        \
  static inline type *M_C(name, _write)(M_C(name, _t) snap)             \
  {									\
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
    unsigned char nextFlags, origFlags = atomic_load (&snap->flags);	\
    /* Atomic CAS operation */                                          \
    do {								\
      /* Swap F and W buffer, setting exchange flag */                  \
      nextFlags = SNAPSHOTI_SPSC_FLAG(SNAPSHOTI_SPSC_R(origFlags),      \
                                      SNAPSHOTI_SPSC_F(origFlags),      \
                                      SNAPSHOTI_SPSC_W(origFlags), 1);  \
      /* exponential backoff is not needed as there can't be more       \
         than 2 threads which try to update the data. */                \
    } while (!atomic_compare_exchange_weak (&snap->flags, &origFlags,	\
					    nextFlags));		\
    /* Return new write buffer for new updating */                      \
    return &snap->data[SNAPSHOTI_SPSC_W(nextFlags)].x;                  \
  }									\
                                                                        \
  static inline type const *M_C(name, _read)(M_C(name, _t) snap)	\
  {									\
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
    unsigned char nextFlags, origFlags = atomic_load (&snap->flags);	\
    /* Atomic CAS operation */                                          \
    do {								\
      /* If no exchange registered, do nothing and keep the same */     \
      if (!SNAPSHOTI_SPSC_B(origFlags)) {                               \
        nextFlags = origFlags;                                          \
	break;								\
      }                                                                 \
      /* Swap R and F buffer, clearing exchange flag */                 \
      nextFlags = SNAPSHOTI_SPSC_FLAG(SNAPSHOTI_SPSC_F(origFlags),      \
                                      SNAPSHOTI_SPSC_W(origFlags),      \
                                      SNAPSHOTI_SPSC_R(origFlags), 0);  \
      /* exponential backoff is not needed as there can't be more       \
         than 2 threads which try to update the data. */                \
    } while (!atomic_compare_exchange_weak (&snap->flags, &origFlags,	\
					    nextFlags));		\
    /* Return current read buffer */                                    \
    return M_CONST_CAST(type, &snap->data[SNAPSHOTI_SPSC_R(nextFlags)].x); \
  }									\
                                                                        \
  static inline bool M_C(name, _updated_p)(M_C(name, _t) snap)          \
  {									\
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
    unsigned char flags = atomic_load (&snap->flags);                   \
    return SNAPSHOTI_SPSC_B(flags);                                     \
  }									\
                                                                        \
  static inline type *M_C(name, _get_write_buffer)(M_C(name, _t) snap)	\
  {									\
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
    unsigned char flags = atomic_load(&snap->flags);			\
    return &snap->data[SNAPSHOTI_SPSC_W(flags)].x;                      \
  }									\
                                                                        \
  static inline type const *M_C(name, _get_read_buffer)(M_C(name, _t) snap) \
  {									\
    SNAPSHOTI_SPSC_CONTRACT(snap);                                      \
    unsigned char flags = atomic_load(&snap->flags);			\
    return  M_CONST_CAST(type, &snap->data[SNAPSHOTI_SPSC_R(flags)].x);	\
  }									\
  


/******************************** INTERNAL SPMC **********************************/

#define SNAPSHOTI_SPMC_INT_FLAG(w, n) ( ((w) << 1) | (n) )
#define SNAPSHOTI_SPMC_INT_FLAG_W(f)  ((f) >> 1)
#define SNAPSHOTI_SPMC_INT_FLAG_N(f)  ((f) & 1)

// 2 more buffer than the number of readers are needed
#define SNAPSHOTI_SPMC_EXTRA_BUFFER 2

#define SNAPSHOTI_SPMC_MAX_READER (GENINT_MAX_ALLOC-SNAPSHOTI_SPMC_EXTRA_BUFFER)

/* structure to handle SPMC snapshot but return an unique index in the buffer array.
   - lastNext: last published written index + next flag (format SNAPSHOTI_SPMC_INT_FLAG)
   - currentWrite: the index being currently written.
   - n : number of readers 
   - cptTab: ref counter array
   - freeList: a pool of free integers.
*/
typedef struct snapshot_mrsw_int_s {
  atomic_uint lastNext;
  unsigned int currentWrite;
  size_t n;
  atomic_uint *cptTab;
  genint_t freeList;
} snapshot_mrsw_int_t[1];

// can't check currentWrite due to potential data race on it
#define SNAPSHOTI_SPMC_INT_CONTRACT(s) do {                             \
    assert (s != NULL);                                                 \
    assert (s->n > 0 && s->n <= SNAPSHOTI_SPMC_MAX_READER);             \
    assert ((size_t)SNAPSHOTI_SPMC_INT_FLAG_W(atomic_load(&s->lastNext)) \
            <= s->n + SNAPSHOTI_SPMC_EXTRA_BUFFER);                     \
    assert (s->cptTab != NULL);                                         \
  } while (0)

static inline void snapshot_mrsw_int_init(snapshot_mrsw_int_t s, size_t n)
{
  assert (s != NULL);
  assert (n >= 1 && n <= SNAPSHOTI_SPMC_MAX_READER);
  s->n = n;
  n += SNAPSHOTI_SPMC_EXTRA_BUFFER;
  atomic_uint *ptr = M_MEMORY_REALLOC (atomic_uint, NULL, n);
  if (M_UNLIKELY (ptr == NULL)) {
    M_MEMORY_FULL(sizeof (atomic_uint) * n);
    return;
  }
  s->cptTab = ptr;
  for(size_t i = 0; i < n; i++)
    atomic_init(&s->cptTab[i], 0U);
  genint_init (s->freeList, n);
  // Get a free buffer and set it as available for readers
  unsigned int w = genint_pop(s->freeList);
  assert (w != GENINT_ERROR);
  atomic_store(&s->cptTab[w], 1U);
  atomic_init(&s->lastNext, SNAPSHOTI_SPMC_INT_FLAG(w, true));
  // Get working buffer
  s->currentWrite = genint_pop(s->freeList);
  assert (s->currentWrite != GENINT_ERROR);
  atomic_store(&s->cptTab[s->currentWrite], 1U);
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
}

static inline void snapshot_mrsw_int_clear(snapshot_mrsw_int_t s)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  M_MEMORY_FREE (s->cptTab);
  genint_clear(s->freeList);
  s->cptTab = NULL;
  s->n = 0;
}

static inline unsigned int snapshot_mrsw_int_get_write_idx(snapshot_mrsw_int_t s)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return s->currentWrite;
}

static inline unsigned int snapshot_mrsw_int_size(snapshot_mrsw_int_t s)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return s->n;
}

static inline unsigned int snapshot_mrsw_int_write_idx(snapshot_mrsw_int_t s, unsigned int idx)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  unsigned int newNext, previous = atomic_load(&s->lastNext);
  do {
    newNext = SNAPSHOTI_SPMC_INT_FLAG(idx, true);
  } while (!atomic_compare_exchange_weak(&s->lastNext, &previous, newNext));
  if (SNAPSHOTI_SPMC_INT_FLAG_N(previous)) {
    // Reuse previous buffer as it was not used by any reader
    idx = SNAPSHOTI_SPMC_INT_FLAG_W(previous);
    // Some other read threads may already have try to reserve this index
    // So atomic_load(&s->cptTab[idx]) can be greater than 1.
    // However they will fail to ack it in lastNext, and remove their reservation later
  } else {
    // Free the write index
    idx = SNAPSHOTI_SPMC_INT_FLAG_W(previous);
    unsigned int c = atomic_fetch_sub(&s->cptTab[idx], 1U);
    assert (c != 0 && c <= s->n + 1);
    // Get a new buffer.
    if (c != 1) {
      // If someone else keeps a ref on the buffer, we can't reuse it
      // get another free one.
      idx = genint_pop(s->freeList);
      assert(idx != GENINT_ERROR);
    }
    assert (idx < s->n + SNAPSHOTI_SPMC_EXTRA_BUFFER);
    assert (atomic_load(&s->cptTab[idx]) == 0);
    atomic_store(&s->cptTab[idx], 1U);
  }
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return idx;
}

static inline unsigned int snapshot_mrsw_int_write(snapshot_mrsw_int_t s)
{
  s->currentWrite = snapshot_mrsw_int_write_idx(s, s->currentWrite);
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return s->currentWrite;
}

static inline unsigned int snapshot_mrsw_int_write_start(snapshot_mrsw_int_t s)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  // Get a new buffer.
  unsigned int idx = genint_pop(s->freeList);
  assert (idx != GENINT_ERROR);
  assert (idx < s->n + SNAPSHOTI_SPMC_EXTRA_BUFFER);
  assert (atomic_load(&s->cptTab[idx]) == 0);
  atomic_store(&s->cptTab[idx], 1U);
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return idx;
}

static inline void snapshot_mrsw_int_write_end(snapshot_mrsw_int_t s, unsigned int idx)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  unsigned int newNext, previous = atomic_load(&s->lastNext);
  do {
    newNext = SNAPSHOTI_SPMC_INT_FLAG(idx, true);
  } while (!atomic_compare_exchange_weak(&s->lastNext, &previous, newNext));

  // Free the previous write buffer
  idx = SNAPSHOTI_SPMC_INT_FLAG_W(previous);
  unsigned int c = atomic_fetch_sub(&s->cptTab[idx], 1U);
  assert (c != 0 && c <= s->n + 1);
  if (c == 1) {
    genint_push(s->freeList, idx);
  }
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
}

static inline unsigned int snapshot_mrsw_int_read_start(snapshot_mrsw_int_t s)
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
    assert (c <= s->n + 1);
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
    assert (c != 0 && c <= s->n+1);
    if (c == 1) {
      genint_push(s->freeList, idx);
    }
  }
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  return idx;
}

static inline void snapshot_mrsw_int_read_end(snapshot_mrsw_int_t s, unsigned int idx)
{
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
  assert (idx < s->n + SNAPSHOTI_SPMC_EXTRA_BUFFER);
  // Decrement reference counter of the buffer
  unsigned int c = atomic_fetch_sub(&s->cptTab[idx], 1U);
  assert (c != 0 && c <= s->n+1);
  if (c == 1) {
    // Buffer no longer used by any reader thread.
    // Push back index in free list
    genint_push(s->freeList, idx);
  }
  SNAPSHOTI_SPMC_INT_CONTRACT(s);
}

#define SNAPSHOTI_SPMC_CONTRACT(snap) do {                              \
    assert (snap != NULL);                                              \
    assert (snap->data != NULL);                                        \
  } while (0)


// Defered evaluation
#define SNAPSHOTI_SPMC_DEF_P1(arg)	SNAPSHOTI_SPMC_DEF_P2 arg

/* Validate the oplist before going further */
#define SNAPSHOTI_SPMC_DEF_P2(name, type, oplist)            \
  M_IF_OPLIST(oplist)(SNAPSHOTI_SPMC_DEF_P3, SNAPSHOTI_SPMC_DEF_FAILURE)(name, type, oplist)

/* Stop processing with a compilation failure */
#define SNAPSHOTI_SPMC_DEF_FAILURE(name, type, oplist)                  \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SNAPSHOT_SPMC_DEF): the given argument is not a valid oplist: " #oplist)

#define SNAPSHOTI_SPMC_DEF_P3(name, type, oplist)                       \
                                                                        \
  /* Create an aligned type to avoid false sharing between threads */   \
  typedef struct M_C(name, _aligned_type_s) {                           \
    type         x;							\
    M_CACHELINE_ALIGN(align, type);                                     \
  } M_C(name, _aligned_type_t);                                         \
                                                                        \
  typedef struct M_C(name, _s) {					\
    M_C(name, _aligned_type_t)  *data;                                  \
    snapshot_mrsw_int_t          core;                                  \
  } M_C(name, _t)[1];							\
                                                                        \
  typedef type M_C(name, _type_t);                                      \
                                                                        \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                      \
                                                                        \
  static inline void M_C(name, _init)(M_C(name, _t) snap, size_t nReader) \
  {									\
    assert (snap != NULL);						\
    assert (nReader > 0 && nReader <= SNAPSHOTI_SPMC_MAX_READER);       \
    snap->data = M_CALL_REALLOC(oplist, M_C(name, _aligned_type_t),     \
                                NULL, nReader+SNAPSHOTI_SPMC_EXTRA_BUFFER); \
    if (M_UNLIKELY (snap->data == NULL)) {                              \
      M_MEMORY_FULL(sizeof(M_C(name, _aligned_type_t)) *                \
                    (nReader+SNAPSHOTI_SPMC_EXTRA_BUFFER));             \
      return;                                                           \
    }                                                                   \
    for(size_t i = 0; i < nReader + SNAPSHOTI_SPMC_EXTRA_BUFFER; i++) { \
      M_CALL_INIT(oplist, snap->data[i].x);                             \
    }									\
    snapshot_mrsw_int_init(snap->core, nReader);                        \
    SNAPSHOTI_SPMC_CONTRACT(snap);                                      \
  }									\
                                                                        \
  static inline void M_C(name, _clear)(M_C(name, _t) snap)		\
  {									\
    SNAPSHOTI_SPMC_CONTRACT(snap);                                      \
    size_t nReader = snapshot_mrsw_int_size(snap->core);                \
    for(size_t i = 0; i < nReader + SNAPSHOTI_SPMC_EXTRA_BUFFER; i++) { \
      M_CALL_CLEAR(oplist, snap->data[i].x);				\
    }									\
    M_CALL_FREE(oplist, snap->data);                                    \
    snapshot_mrsw_int_clear(snap->core);                                \
  }									\
                                                                        \
  static inline type *M_C(name, _write)(M_C(name, _t) snap)             \
  {									\
    SNAPSHOTI_SPMC_CONTRACT(snap);                                      \
    const unsigned int idx = snapshot_mrsw_int_write(snap->core);       \
    return &snap->data[idx].x;                                          \
  }									\
                                                                        \
  static inline type const *M_C(name, _read_start)(M_C(name, _t) snap)	\
  {									\
    SNAPSHOTI_SPMC_CONTRACT(snap);                                      \
    const unsigned int idx = snapshot_mrsw_int_read_start(snap->core);  \
    return M_CONST_CAST(type, &snap->data[idx].x);                      \
  }									\
                                                                        \
  static inline void M_C(name, _read_end)(M_C(name, _t) snap, type const *old) \
  {									\
    SNAPSHOTI_SPMC_CONTRACT(snap);                                      \
    assert (old != NULL);                                               \
    const M_C(name, _aligned_type_t) *oldx;                             \
    oldx = M_CTYPE_FROM_FIELD(M_C(name, _aligned_type_t), old, type, x); \
    assert (oldx >= snap->data);                                        \
    assert (oldx < snap->data + snap->core->n+SNAPSHOTI_SPMC_EXTRA_BUFFER); \
    const unsigned int idx = oldx - snap->data;                         \
    snapshot_mrsw_int_read_end(snap->core, idx);                        \
  }									\
                                                                        \
  static inline type *M_C(name, _get_write_buffer)(M_C(name, _t) snap)	\
  {									\
    SNAPSHOTI_SPMC_CONTRACT(snap);                                      \
    const unsigned int idx = snapshot_mrsw_int_get_write_idx(snap->core); \
    return &snap->data[idx].x;                                          \
  }									\
                                                                        \

//FIXME: Evaluate the needs for the methods _set_, _init_set.



/******************************** INTERNAL MPMC **********************************/

// MPMC is built upon SPMC

// Defered evaluation
#define SNAPSHOTI_MPMC_DEF_P1(arg)	SNAPSHOTI_MPMC_DEF_P2 arg

/* Validate the oplist before going further */
#define SNAPSHOTI_MPMC_DEF_P2(name, type, oplist)            \
  M_IF_OPLIST(oplist)(SNAPSHOTI_MPMC_DEF_P3, SNAPSHOTI_MPMC_DEF_FAILURE)(name, type, oplist)

/* Stop processing with a compilation failure */
#define SNAPSHOTI_MPMC_DEF_FAILURE(name, type, oplist)                  \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(SNAPSHOT_MPMC_DEF): the given argument is not a valid oplist: " #oplist)

#define SNAPSHOTI_MPMC_DEF_P3(name, type, oplist)                       \
                                                                        \
  SNAPSHOTI_SPMC_DEF_P1((M_C(name, _mrsw), type, oplist))               \
                                                                        \
  typedef struct M_C(name, _s) {					\
    M_C(name, _mrsw_t)  core;                                           \
  } M_C(name, _t)[1];							\
                                                                        \
  typedef type M_C(name, _type_t);                                      \
                                                                        \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                      \
                                                                        \
  static inline void M_C(name, _init)(M_C(name, _t) snap, size_t nReader, size_t nWriter) \
  {									\
    M_C(name, _mrsw_init)(snap->core, nReader + nWriter -1 );           \
    unsigned int idx = snap->core->core->currentWrite;                  \
    snap->core->core->currentWrite = GENINT_ERROR;                      \
    snapshot_mrsw_int_write_end(snap->core->core, idx);                 \
  }									\
                                                                        \
  static inline void M_C(name, _clear)(M_C(name, _t) snap)		\
  {									\
    M_C(name, _mrsw_clear)(snap->core);                                 \
  }									\
                                                                        \
  static inline type *M_C(name, _write_start)(M_C(name, _t) snap)       \
  {									\
    SNAPSHOTI_SPMC_CONTRACT(snap->core);                                \
    const unsigned int idx = snapshot_mrsw_int_write_start(snap->core->core); \
    return &snap->core->data[idx].x;                                    \
  }									\
                                                                        \
  static inline void M_C(name, _write_end)(M_C(name, _t) snap, type *old) \
  {									\
    SNAPSHOTI_SPMC_CONTRACT(snap->core);                                \
    const M_C(name, _mrsw_aligned_type_t) *oldx;                        \
    oldx = M_CTYPE_FROM_FIELD(M_C(name, _mrsw_aligned_type_t), old, type, x); \
    assert (oldx >= snap->core->data);                                  \
    assert (oldx < snap->core->data + snap->core->core->n + SNAPSHOTI_SPMC_EXTRA_BUFFER); \
    const unsigned int idx = oldx - snap->core->data;                   \
    snapshot_mrsw_int_write_end(snap->core->core, idx);                 \
  }									\
                                                                        \
  static inline type const *M_C(name, _read_start)(M_C(name, _t) snap)	\
  {									\
    return M_C(name, _mrsw_read_start)(snap->core);                     \
  }									\
                                                                        \
  static inline void M_C(name, _read_end)(M_C(name, _t) snap, type const *old) \
  {									\
    M_C(name, _mrsw_read_end)(snap->core, old);                         \
  }									\
                                                                        \

//TODO: _set_, _init_set

#endif
