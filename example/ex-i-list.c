#include "m-i-list.h"

typedef struct data_s {
    M_ILIST_INTERFACE(list1, struct data_s);
    char *ptr;
    int  n;
} data_t;

static void
data_init(data_t *p)
{
    M_ILIST_INIT_FIELD(list1, *p);
    p->ptr = malloc(100);
    p->n = 100;
}

static void
data_clear(data_t *p)
{
    free(p->ptr);
    p->n = 0;
}

static void
data_init_set(data_t *p, data_t s)
{
    p->ptr = malloc(s.n);
    p->n = s.n;
    memcpy(p->ptr, s.ptr, s.n);
}

#define DATA_OP (INIT(API_2(data_init)), INIT_SET(API_2(data_init_set)), CLEAR(API_2(data_clear)) )

M_ILIST_DEF(list1, data_t, DATA_OP)

data_t pool[100];

int main(void)
{
    list1_t l;

    list1_init(l);
    data_init(&pool[0]);
    list1_push_back(l, &pool[0]);
    data_init(&pool[1]);
    list1_push_back(l, &pool[1]);

    list1_it_t it;
    list1_it(it, l);
    data_init(&pool[2]);
    list1_insert(l, it, &pool[2]);

    list1_clear(l);

    exit(0);
}
