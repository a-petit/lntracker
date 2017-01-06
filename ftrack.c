#include "ftrack.h"

//--- structure ftrack ---------------------------------------------------------

typedef struct ftrack {
  size_t id;
  dyna *lines;
} ftrack;

//--- fonctions de ftrack ------------------------------------------------------

ftrack *ftrack_create(size_t fileid) {
  ftrack * ft = malloc(sizeof *ft);
  if (ft == NULL) {
    return NULL;
  }
  ft->lines = dyna_empty();
  if (ft->lines == NULL) {
    free(ft);
    return NULL;
  }
  ft->id = fileid;
  return ft;
}

void ftrack_dispose(ftrack **ptrt) {
  if (*ptrt == NULL) {
    return;
  }

  size_t k = 0;
  size_t m = dyna_length((*ptrt)->lines);
  while (k < m) {
    long int *n = (long int *) dyna_get((*ptrt)->lines, k);
    free(n);
    ++k;
  }

  dyna_dispose(&(*ptrt)->lines);
  free(*ptrt);
  *ptrt = NULL;
}

const long int *ftrack_addline(ftrack *ft, long int n) {
  long int *x = malloc(sizeof *x);
  if (x == NULL) {
    return NULL;
  }
  *x = n;
  if (dyna_push(ft->lines, x) != x) {
    free(x);
    return NULL;
  }
  return x;
}

const dyna *ftrack_getlines(const ftrack *ft) {
  return (const dyna *) ft->lines;
}

size_t ftrack_id(const ftrack *ft) {
  return ft->id;
}

