#include "m-prioqueue.h"

PRIOQUEUE_DEF(int_pqueue, int)

static void test1(void)
{
  int x;
  int_pqueue_t p;
  int_pqueue_init(p);
  assert (int_pqueue_empty_p(p));
  assert (int_pqueue_size(p) == 0);
  int_pqueue_push(p, 10);
  assert (!int_pqueue_empty_p(p));
  assert (int_pqueue_size(p) == 1);
  int_pqueue_push(p, 60);
  int_pqueue_push(p, 40);
  int_pqueue_push(p, 5);
  int_pqueue_push(p, 30);
  assert (int_pqueue_size(p) == 5);
  assert (*int_pqueue_front(p) == 5);
  int_pqueue_pop(&x, p);
  assert (int_pqueue_size(p) == 4);
  assert (x == 5);
  int_pqueue_pop(&x, p);
  assert (int_pqueue_size(p) == 3);
  assert (x == 10);
  int_pqueue_pop(&x, p);
  assert (x == 30);
  int_pqueue_pop(&x, p);
  assert (x == 40);
  int_pqueue_pop(&x, p);
  assert (x == 60);
  assert (int_pqueue_empty_p(p));
  assert (int_pqueue_size(p) == 0);
  
  int_pqueue_clear(p);
}

int main(void)
{
  test1();
}
