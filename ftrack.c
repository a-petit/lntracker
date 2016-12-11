#include "ftrack.h"

//--- structure ftrack ---------------------------------------------------------

typedef struct ftrack {
  size_t id;
  vector *lines;
} ftrack;

//--- fonctions de ftrack ------------------------------------------------------

ftrack *ftrack_create(size_t fileid) {
  ftrack * ft = malloc(sizeof *ft);
  if (ft == NULL) {
    return NULL;
  }
  ft->lines = vector_empty();
  if (ft->lines == NULL) {
    free(ft);
    return NULL;
  }
  ft->id = fileid;
  return ft;
}

const long int *ftrack_addline(ftrack *ft, long int n) {
  long int *x = malloc(sizeof *x);
  if (x == NULL) {
    return NULL;
  }
  *x = n;
  vector_push(ft->lines, x);
  return x;
}

const vector *ftrack_getlines(const ftrack *ft) {
  return (const vector *) ft->lines;
}

size_t ftrack_id(const ftrack *ft) {
  return ft->id;
}

void ftrack_dispose(ftrack **ptrt) {
  if (*ptrt == NULL) {
    return;
  }

  size_t k = 0;
  size_t m = vector_length((*ptrt)->lines);
  while (k < m) {
    long int *n = (long int *) vector_get((*ptrt)->lines, k);
    free(n);
    ++k;
  }

  vector_dispose(&(*ptrt)->lines);
  free(*ptrt);
  *ptrt = NULL;
}

