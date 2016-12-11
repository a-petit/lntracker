#include <stdint.h>
#include <stdio.h>
#include "vector.h"

#define VEC_NSLOTS_MIN  2
#define VEC_RESIZE_MUL  2

struct vector {
  void **value;
  size_t nslots;
  size_t nentries;
  void *playhead;
};

#define PTRSIZE       (sizeof(void *))

#define VALUE(t, i)   ((t) -> value[i])

//------------------------------------------------------------------------------

static int vector_resize(vector *t, size_t nslots) {
  //printf("vector_resize to %zu\n", nslots);
  void **a = realloc(t->value, nslots * PTRSIZE);
  if (a == NULL) {
    printf("*** realloc error\n");
    return 1;
  }
  t->value = a;
  t->nslots = nslots;
  t->playhead = t->value;
  return 0;
}

//------------------------------------------------------------------------------

vector *vector_empty(void) {
  vector *t = malloc(sizeof *t);
  if (t == NULL) {
    return NULL;
  }
  t->value = malloc(PTRSIZE * VEC_NSLOTS_MIN);
  if (t->value == NULL) {
    free(t);
    return NULL;
  }
  t->nslots = VEC_NSLOTS_MIN;
  t->nentries = 0;
  return t;
}

const void *vector_push(vector *t, const void * x) {
  //printf("vector_push at %zu/%zu\n", t->nentries, t->nslots);
  if (t->nentries >= t->nslots) {
    size_t n =  t->nslots * VEC_RESIZE_MUL;
    if (t->nslots >= SIZE_MAX / VEC_RESIZE_MUL || vector_resize(t, n)) {
      return NULL;
    }
  }

  t->value[t->nentries] = (void *) x;
  ++t->nentries;
  return x;
}

const void *vector_get(const vector *t, size_t i) {
  //printf("*** WARNING : vector_get is now deprecated\n");
  //printf("vector_get at %zu / %zu\n", i, t->nentries);
  if (i >= t->nentries) {
    return NULL;
  }
  return t->value[i];
}

size_t vector_length(const vector *t) {
  //printf("*** WARNING : vector_length is now deprecated\n");
  return t->nentries;
}

const void *vector_rsearch(vector *t, const void *key,
  int (*cmp)(const void *, const void *)) {
  //printf("*** WARNING : vector_rsearch is now deprecated\n");
  size_t k = 0;
  size_t n = t->nentries;
  // IB :
  // QC : k
  while (k < n && cmp(VALUE(t, n - k - 1), key) != 0) {
    ++k;
  }
  return k == n ? NULL : VALUE(t, k);
}



static int vector_hasnext(vector *t) {
  return (size_t)((char *) t->playhead - (char *) t->value + 1) < t->nentries;
}

void vector_reset_iterator(vector *t) {
  t->playhead = t->value[0];
}

const void *vector_iterate(vector *t) {
  if (vector_hasnext(t) == 0) {
    return NULL;
  }
  const void *x = t->playhead;
  t->playhead = (char *) t->playhead + PTRSIZE;
  return x;
}

const void *vector_fst(const vector *t) {
  if (t->nentries == 0) {
    return NULL;
  }
  return t->value[0];
}

const void *vector_lst(const vector *t) {
  if (t->nentries == 0) {
    return NULL;
  }
  return t->value[t->nentries -1];
}

//const void *vector_next(vector *t) {
  //printf("vector_next\n");
  //if (! vector_hasnext(t)) {
    //return NULL;
  //}
  //t->playhead = (char *) t->playhead + PTRSIZE;
  //return t->playhead;
//}

void vector_dispose(vector **ptrt) {
  if (*ptrt == NULL) {
    return;
  }
  free((*ptrt)->value);
  free(*ptrt);
  *ptrt = NULL;
}
