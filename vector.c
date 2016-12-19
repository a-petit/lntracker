#include <stdint.h>
#include <stdio.h>
#include "quick_sort.h"
#include "vector.h"

#define VEC_NSLOTS_MIN  1
#define VEC_RESIZE_MUL  2

#define PTRSIZE (sizeof(void *))

//--- Structure vector ---------------------------------------------------------

struct vector {
  void **value;
  size_t nslots;
  size_t nentries;
};

#define VALUE(t)      ((t) -> value)
#define NSLOTS(t)     ((t) -> nslots)
#define NENTRIES(t)   ((t) -> nentries)

#define IS_EMPTY(t)   (NENTRIES(t) <= 0)
#define IS_FILLED(t)  (NENTRIES(t) >= NSLOTS(t))

//--- Fonctions locales --------------------------------------------------------

static int vector_resize(vector *t, size_t nslots) {
  void **a = realloc(VALUE(t), nslots * PTRSIZE);
  if (a == NULL) {
    printf("*** realloc error\n");
    return 1;
  }
  VALUE(t) = a;
  NSLOTS(t) = nslots;
  return 0;
}

//------------------------------------------------------------------------------

vector *vector_empty(void) {
  vector *t = malloc(sizeof *t);
  if (t == NULL) {
    return NULL;
  }
  VALUE(t) = malloc(PTRSIZE * VEC_NSLOTS_MIN);
  if (VALUE(t) == NULL) {
    free(t);
    return NULL;
  }
  NSLOTS(t) = VEC_NSLOTS_MIN;
  NENTRIES(t) = 0;
  return t;
}

const void *vector_push(vector *t, const void * x) {
  if (IS_FILLED(t)) {
    size_t n =  NSLOTS(t) * VEC_RESIZE_MUL;
    if (NSLOTS(t) >= SIZE_MAX / VEC_RESIZE_MUL || vector_resize(t, n) != 0) {
      return NULL;
    }
  }
  VALUE(t)[NENTRIES(t)] = (void *) x;
  ++NENTRIES(t);
  return x;
}

const void *vector_get(const vector *t, size_t i) {
  if (i >= NENTRIES(t)) {
    return NULL;
  }
  return VALUE(t)[i];
}

const void *vector_fst(const vector *t) {
  if (IS_EMPTY(t)) {
    return NULL;
  }
  return VALUE(t)[0];
}

const void *vector_lst(const vector *t) {
  if (IS_EMPTY(t)) {
    return NULL;
  }
  return VALUE(t)[NENTRIES(t) -1];
}

size_t vector_length(const vector *t) {
  return NENTRIES(t);
}

void vector_qsort(vector *t, int (*compar)(const void *, const void *)) {
  quick_sort(VALUE(t), NENTRIES(t), PTRSIZE, compar);
}

void vector_dispose(vector **ptrt) {
  if (*ptrt == NULL) {
    return;
  }
  free((*ptrt)->value);
  free(*ptrt);
  *ptrt = NULL;
}
