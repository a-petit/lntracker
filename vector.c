#include <stdint.h>
#include <stdio.h>
#include "vector.h"

#define VEC_NSLOTS_MIN  2
#define VEC_RESIZE_MUL  2

struct vector {
  void **value;
  size_t nslots;
  size_t nentries;
};

#define VALUE(t, i)   ((t) -> value[i])

static int vector_resize(vector *t, size_t nslots) {
  printf("vector_resize to %zu\n", nslots);
  void **a = realloc(t->value, nslots * sizeof(*(t->value)));
  if (a == NULL) {
    printf("*** realloc error\n");
    return 1;
  }
  t->value = a;
  t->nslots = nslots;
  return 0;
}

// vector_empty : crée un tableau vide sur le type void * initialement vide.
//   renvoie un pointeur sur l'objet qui gère ce tableau en cas de succès,
//   et NULL en cas d'échec
vector *vector_empty(void) {
  vector *t = malloc(sizeof *t);
  if (t == NULL) {
    return NULL;
  }
  t->value = malloc(sizeof *(t->value) * VEC_NSLOTS_MIN);
  if (t->value == NULL) {
    free(t);
    return NULL;
  }
  t->nslots = VEC_NSLOTS_MIN;
  t->nentries = 0;
  return t;
}

// vector_add : insère x en queue du tableau associé à t.
//   renvoie x en cas de succès, NULL en cas d'échec
const void *vector_add(vector *t, const void * x) {
  //printf("vector_add at %zu\n", t->nentries);
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

// vector_get : renvoie l'élément référencé à l'indice i dans le tableau t.
//   renvoie x en cas de succès, NULL en cas d'échec
void *vector_get(const vector *t, size_t i) {
  //printf("vector_get at %zu / %zu\n", i, t->nentries);
  if (i >= t->nentries) {
    return NULL;
  }
  return t->value[i];
}

// vector_length : renvoie la longueur du tableau associé à t
size_t vector_length(const vector *t) {
  return t->nentries;
}


const void *vector_rsearch(vector *t, const void *key,
  int (*cmp)(const void *, const void *)) {
  size_t k = 0;
  size_t n = t->nentries;
  // IB :
  // QC : k
  while (k < n && cmp(VALUE(t, n - k - 1), key) != 0) {
    ++k;
  }
  return k == n ? NULL : VALUE(t, k);
}

// vector_dispose : libère les ressources allouées à *ptrt et donne à *ptrt
//   la valeur NULL. tolère que *ptrt vaille NULL
void vector_dispose(vector **ptrt) {
  if (*ptrt == NULL) {
    return;
  }
  free((*ptrt)->value);
  free(*ptrt);
  *ptrt = NULL;
}
