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

long int *ftrack_addline(ftrack *ft, long int n) {
  long int *x = malloc(sizeof *x);
  if (x == NULL) {
    return NULL;
  }
  *x = n;
  vector_push(ft->lines, x);
  return x;
}

size_t ftrack_id(const ftrack *ft) {
  return ft->id;
}

const vector *ftrack_getlines(const ftrack *ft) {
  return (const vector *) ft->lines;
}

int ftrack_compar_id(const ftrack *ft, size_t fileid) {
  return ftrack_id(ft) == fileid ? 0 : 1;
}

void ftrack_dispose(ftrack **ptrt) {
  if (*ptrt == NULL) {
    return;
  }

#if 1

  long int *n;
  vector_reset_iterator((*ptrt)->lines);
  while ((n = (long int *) vector_iterate((*ptrt)->lines))) {
    free(n);
  }

#elif 1

  long int *n = (long int *) vector_fst((*ptrt)->lines);
  while (n) {
    free(n);
    n = (long int *) vector_next((*ptrt)->lines);
  }

#elif 1

  for (size_t i = 0; i < vector_length((*ptrt)->lines); ++i) {
    long int *n = (long int *) vector_get((*ptrt)->lines, i);
    free(n);
  }

#endif

  vector_dispose(&(*ptrt)->lines);
  free(*ptrt);
  *ptrt = NULL;
}

