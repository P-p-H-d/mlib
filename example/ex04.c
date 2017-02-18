#include "m-buffer.h"
#include "m-shared.h"
#include "m-mutex.h"

/**************************************************************/

/* Define a random big data structure */

#define BIGDATA_SIZE 100000
#define BIGDATA_NUM 10

typedef struct bigdata_s {
  char bigdata[BIGDATA_SIZE];
  char *ptr;
} bigdata_t[1];

static void bigdata_init(bigdata_t d)
{
  memset(d->bigdata, 0, BIGDATA_SIZE);
  d->ptr = malloc (BIGDATA_SIZE * 10);
}

static void bigdata_clear(bigdata_t d)
{
  free(d->ptr);
}

static bigdata_t *bigdata_new (void)
{
  bigdata_t *n = malloc (sizeof (bigdata_t));
  bigdata_init(*n);
  return n;
}

static void bigdata_free(bigdata_t *d)
{
  bigdata_clear(*d);
  free(d);
}

/**************************************************************/

/* Perform some works on this big data */

static void perform_acquisition(bigdata_t *p)
{
  // TODO  
}
static void perform_computation2(bigdata_t *p)
{
  // TODO  
}
static void perform_computation3(bigdata_t *p)
{
  // TODO  
}
static void perform_computation4(bigdata_t *p)
{
  // TODO  
}

/**************************************************************/

/* Let's do the thread scheduling */

#define MY_QUEUE_SIZE 10
SHARED_PTR_DEF(bigdata, bigdata_t, (CLEAR(bigdata_clear), FREE(free)))
BUFFER_DEF(bigdata, shared_bigdata_t, MY_QUEUE_SIZE,
           BUFFER_QUEUE|BUFFER_PUSH_INIT_POP_MOVE, SHARED_PTR_OPLIST(bigdata))

/* Thread synchros:
 * Thread1 ---> Thread 2
 *          +-> Thread 3 --> Thread 4
 */
buffer_bigdata_t buf_t1tot2;
buffer_bigdata_t buf_t1tot3;
buffer_bigdata_t buf_t3tot4;
bool             continue_threading;
M_THREAD_T       t1, t2, t3, t4;

static void initbuf(void)
{
  buffer_bigdata_init(buf_t1tot2, MY_QUEUE_SIZE);
  buffer_bigdata_init(buf_t1tot3, MY_QUEUE_SIZE);
  buffer_bigdata_init(buf_t3tot4, MY_QUEUE_SIZE);
}

static void clearbuf(void)
{
  buffer_bigdata_clear(buf_t1tot2);
  buffer_bigdata_clear(buf_t1tot3);
  buffer_bigdata_clear(buf_t3tot4);
}

static void thread1 (void *arg)
{
  while (continue_threading) {
    shared_bigdata_t ptr;
    shared_bigdata_init2 (ptr, bigdata_new());
    perform_acquisition(shared_bigdata_ref(ptr));
    buffer_bigdata_push(buf_t1tot2, ptr);
    buffer_bigdata_push(buf_t1tot3, ptr);
    shared_bigdata_clear(ptr);
  }
}
static void thread2 (void *arg)
{
  while (continue_threading) {
    shared_bigdata_t ptr;
    shared_bigdata_init (ptr);
    buffer_bigdata_pop(&ptr, buf_t1tot2);
    perform_computation2(shared_bigdata_ref(ptr));
    shared_bigdata_clear(ptr);
  }
}
static void thread3 (void *arg)
{
  while (continue_threading) {
    shared_bigdata_t ptr;
    shared_bigdata_init (ptr);
    buffer_bigdata_pop(&ptr, buf_t1tot3);
    perform_computation3(shared_bigdata_ref(ptr));
    buffer_bigdata_push(buf_t3tot4, ptr);
    shared_bigdata_clear(ptr);
  }
}
static void thread4 (void *arg)
{
  while (continue_threading) {
    shared_bigdata_t ptr;
    shared_bigdata_init (ptr);
    buffer_bigdata_pop(&ptr, buf_t3tot4);
    perform_computation4(shared_bigdata_ref(ptr));
    shared_bigdata_clear(ptr);
  }
}


static void init(void)
{
  initbuf();
  continue_threading = true;
  M_THREAD_CREATE(t1, thread1, NULL);
  M_THREAD_CREATE(t2, thread2, NULL);
  M_THREAD_CREATE(t3, thread3, NULL);
  M_THREAD_CREATE(t3, thread4, NULL);
}

static void clear(void)
{
  continue_threading = false;
  M_THREAD_JOIN(t1);
  M_THREAD_JOIN(t2);
  M_THREAD_JOIN(t3);
  M_THREAD_JOIN(t4);
  clearbuf();
}

int main(void)
{
  init();
  // 
  clear();
}
