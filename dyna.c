#include <stdint.h>
#include <stdio.h>
#include "quick_sort.h"
#include "dyna.h"

#define VEC_NSLOTS_MIN  1
#define VEC_RESIZE_MUL  2

#define PTRSIZE (sizeof(void *))

//--- Structure dyna ---------------------------------------------------------

struct dyna {
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

static int dyna_resize(dyna *t, size_t nslots) {
  void **a = realloc(VALUE(t), nslots * PTRSIZE);
  if (a == NULL) {
    printf("*** realloc error\n");
    return 1;
  }
  VALUE(t) = a;
  NSLOTS(t) = nslots;
  return 0;
}

//--- Fonctions de dyna ------------------------------------------------------

dyna *dyna_empty(void) {
  dyna *t = malloc(sizeof *t);
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

const void *dyna_push(dyna *t, const void * x) {
  if (IS_FILLED(t)) {
    size_t n =  NSLOTS(t) * VEC_RESIZE_MUL;
    if (NSLOTS(t) >= SIZE_MAX / VEC_RESIZE_MUL || dyna_resize(t, n) != 0) {
      return NULL;
    }
  }
  VALUE(t)[NENTRIES(t)] = (void *) x;
  ++NENTRIES(t);
  return x;
}

const void *dyna_get(const dyna *t, size_t i) {
  if (i >= NENTRIES(t)) {
    return NULL;
  }
  return VALUE(t)[i];
}

const void *dyna_fst(const dyna *t) {
  if (IS_EMPTY(t)) {
    return NULL;
  }
  return VALUE(t)[0];
}

const void *dyna_lst(const dyna *t) {
  if (IS_EMPTY(t)) {
    return NULL;
  }
  return VALUE(t)[NENTRIES(t) -1];
}

size_t dyna_length(const dyna *t) {
  return NENTRIES(t);
}

void dyna_qsort(dyna *t, int (*compar)(const void *, const void *)) {
  quick_sort(VALUE(t), NENTRIES(t), PTRSIZE, compar);
}

void dyna_dispose(dyna **ptrt) {
  if (*ptrt == NULL) {
    return;
  }
  free((*ptrt)->value);
  free(*ptrt);
  *ptrt = NULL;
}
