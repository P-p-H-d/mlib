#include <time.h>
#include <stdio.h>

/* Register a dynamic seed to M*LIB hash function. 
   This shall be done before any M*LIB header inclusion. */
unsigned long long rand_seed;
#define M_HASH_SEED rand_seed

#include "m-array.h"
#include "m-dict.h"
#include "m-string.h"
#include "m-variant.h"

/* This example will show how to create JSON-like files.
   This is a JSON **like** syntax, not exactly JSON format */

/* First let's handle the OUT/IN functions of boolean */
static inline void boolean_out_str(FILE *f, bool b)
{
  fprintf(f, b ? "TRUE" : "FALSE");
}

static inline bool boolean_in_str(bool *b, FILE *f)
{
  char c = fgetc(f);
  if (c == 'T') {
    *b = true;
    c = fgetc(f);
    if (c != 'R') return false;
    c = fgetc(f);
    if (c != 'U') return false;
    c = fgetc(f);
    return c == 'E';
  } else if (c == 'F') {
    *b = false;
    c = fgetc(f);
    if (c != 'A') return false;
    c = fgetc(f);
    if (c != 'L') return false;
    c = fgetc(f);
    if (c != 'S') return false;
    c = fgetc(f);
    return c == 'E';
  } else {
    return false;
  }
}

/* Then the OUT/IN functions of float */
static inline void real_out_str(FILE *f, float r)
{
  fprintf(f, "%f", (double) r);
}

static inline bool real_in_str(float *r, FILE *f)
{
  return fscanf(f, "%f", r) == 1;
}


/* Let's define the header & its interface */

typedef struct json_node_s *json_t;

extern void json_new(json_t *p);
extern void json_init_new(json_t *p);
extern void json_init (json_t *);
extern void json_clear (json_t);
extern void json_init_set (json_t *, json_t);
extern void json_set (json_t *, json_t);
extern void json_out_str(FILE *, json_t);
extern bool json_in_str(json_t *, FILE *);

#define JSON_OPLIST                                                     \
  (INIT(json_init M_IPTR), CLEAR(json_clear),                           \
   INIT_SET(json_init_set M_IPTR), SET(json_set M_IPTR),                \
   OUT_STR(json_out_str), IN_STR(json_in_str M_IPTR) )

DICT_DEF2(dict_json, string_t, STRING_OPLIST, json_t, JSON_OPLIST)

ARRAY_DEF(array_json, json_t, JSON_OPLIST)

VARIANT_DEF2(variant_json,
             (boolean, bool, M_OPEXTEND(M_DEFAULT_OPLIST, OUT_STR(boolean_out_str), IN_STR(boolean_in_str M_IPTR)) ),
             (real, float, M_OPEXTEND (M_DEFAULT_OPLIST, OUT_STR(real_out_str), IN_STR(real_in_str M_IPTR))),
             (array, array_json_t, ARRAY_OPLIST(array_json, JSON_OPLIST)), 
             (dict, dict_json_t, DICT_OPLIST(dict_json, STRING_OPLIST, JSON_OPLIST)))

struct json_node_s {
  variant_json_t json;
};


/* Body definition */

void json_new(json_t *p)
{
  *p = malloc (sizeof (struct json_node_s));
  if (*p == NULL) abort();
}

void json_init(json_t *p)
{
  *p = NULL;
}

void json_init_new(json_t *p)
{
  json_new(p);
  variant_json_init((*p)->json);
}

void json_clear (json_t p)
{
  if (p != NULL) {
    variant_json_clear (p->json);
    free(p);
  }
}

void json_init_set(json_t *p, json_t o)
{
  if (o == NULL) {
    *p = NULL;
  } else {
    json_new(p);
    variant_json_init_set((*p)->json, o->json);
  }
}

void json_set(json_t *p, json_t o)
{
  json_clear(*p);
  json_init_set(p, o);
}

void json_out_str(FILE *f, json_t o)
{
  if (o != NULL) {
    variant_json_out_str(f, o->json);
  }
}

bool json_in_str(json_t *p, FILE *f)
{
  if (*p == NULL) {
    json_new(p);
    variant_json_init((*p)->json);
  }
  return variant_json_in_str((*p)->json, f);
}

/* test */

static json_t generate(void)
{
  json_t p, k;
  dict_json_t d;
  array_json_t a;
  string_t s;

  /* This is a low level generation for example */
  json_init_new(&p);
  json_init_new(&k);
  array_json_init(a);
  dict_json_init(d);
  string_init (s);

  /* Let's generate the following structure :
     { channel: TRUE, filter: 2.3, tab: [2., 3.] } */
  variant_json_set_boolean(k->json, true);
  dict_json_set_at(d, STRING_CTE("channel"), k);
  variant_json_set_real(k->json, 2.3);
  dict_json_set_at(d, STRING_CTE("filter"), k);
  variant_json_set_real(k->json, 2.);
  array_json_push_back (a, k);
  variant_json_set_real(k->json, 3.);
  array_json_push_back (a, k);
  variant_json_set_array (k->json, a);
  dict_json_set_at(d, STRING_CTE("tab"), k);
  variant_json_set_dict(p->json, d);

  json_clear(k);
  string_clear(s);
  dict_json_clear(d);
  array_json_clear(a);

  return p;
}

int main(void)
{
  /* Compute a new random seed for the hash computation */
  srand(time(NULL));
  rand_seed = ((rand() * RAND_MAX + rand()) * RAND_MAX + rand()) * RAND_MAX + rand();
  
  /* Generate data */
  json_t p = generate();
  json_out_str(stdout, p);
  /* Typical Output:
    @dict@{"filter":@real@2.300000@,"tab":@array@[@real@2.000000@,@real@3.000000@]@,"channel":@boolean@TRUE@}@
    If you want a true JSON format, you'll have to overload the variant IN/OUT function so that they
    can detect the type of argument and, as such, don't need to output it in the format */
  printf("\n");
  json_clear(p);
  return 0;
}
