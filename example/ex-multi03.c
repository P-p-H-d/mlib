#include <stdio.h>

#include "m-array.h"
#include "m-dict.h"
#include "m-string.h"
#include "m-variant.h"

/* This is a JSON like syntax, not exactly JSON format */

/* OUT/IN of boolean */
static inline void boolean_out_str(FILE *f, bool b)
{
  fprintf(f, b ? "TRUE" : "FALSE");
}
static inline bool boolean_in_str(bool *b, FILE *f)
{
  char c = fgetc(f);
  if (c == 'T') {
    c = fgetc(f);
    if (c != 'R') return false;
    c = fgetc(f);
    if (c != 'U') return false;
    c = fgetc(f);
    return c == 'E';
  } else if (c == 'F') {
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

/* OUT/IN of float */
static inline void real_out_str(FILE *f, float r)
{
  fprintf(f, "%f", r);
}
static inline bool real_in_str(float *r, FILE *f)
{
  return fscanf(f, "%f", r) == 1;
}

/* Header definition */

typedef struct json_node_s *json_t;

extern void json_init (json_t *);
extern void json_clear (json_t);
extern void json_init_set (json_t *, json_t);
extern void json_set (json_t *, json_t);
extern void json_out_str(FILE *, json_t);
extern bool json_in_str(json_t *, FILE *);

#define JSON_OPLIST (INIT(json_init M_IPTR), CLEAR(json_clear),         \
                     INIT_SET(json_init_set M_IPTR), SET(json_set M_IPTR),\
                     OUT_STR(json_out_str), IN_STR(json_in_str M_IPTR) )

DICT_DEF2(json, string_t, STRING_OPLIST, json_t, JSON_OPLIST)

ARRAY_DEF(json, json_t, JSON_OPLIST)

VARIANT_DEF2(variant_json,
             (boolean, bool, M_OPEXTEND(M_DEFAULT_OPLIST, OUT_STR(boolean_out_str), IN_STR(boolean_in_str M_IPTR)) ),
             (real, float, M_OPEXTEND (M_DEFAULT_OPLIST, OUT_STR(real_out_str), IN_STR(real_in_str M_IPTR))),
             (array, array_json_t, ARRAY_OPLIST(json, JSON_OPLIST)), 
             (dict, dict_json_t, DICT_OPLIST(json, STRING_OPLIST, JSON_OPLIST)))

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
