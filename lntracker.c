#include "lntracker.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hashtbl.h"
#include "ftrack.h"
#include "lnscan.h"
#include "vector.h"

#define FUN_FAILURE 1
#define FUN_SUCCESS 0

// STRINGLEN_MAX : taille maximale des lignes lues.
// Peut être amélioré. Cependant, Si les prefixes de deux lignes de 4095 crctrs
// sont identiques, il est fort à supposer que les lignes sont identiques dans
// leur intégralité.
#define STRINGLEN_MAX (4095 - 1)

typedef struct lntracker {
  vector *filenames;
  hashtable *ht;
  vector *keys;
  scanopt *opt;
} lntracker;


#define FILES(t)      ((t) -> filenames)
#define KEYS(t)       ((t) -> keys)
#define HTBL(t)       ((t) -> ht)

#define FILES_CT(t)   (vector_length(FILES(t)))
#define KEYS_CT(t)    (vector_length(KEYS(t)))

//--- Fonctions locales --------------------------------------------------------

static int lnt_parselines(lntracker *tracker, FILE *stream, size_t fid, bool gen) {
  //printf("--- parselines : %zu, %d\n", fid, gen);
  char buf[STRINGLEN_MAX + 1];
  long int n = 0;

  // IB :
  // QC : nombre d'appels à lnscan_getline
  while (lnscan_getline(tracker->opt, stream, buf, STRINGLEN_MAX + 1) == 0) {
    //printf("--- parselines : %s\n", buf);
    vector *ftl = (vector *) hashtable_value(tracker->ht, buf);
    if (ftl == NULL && gen) {
      ftl = vector_empty();
      if (ftl == NULL) {
        return FUN_FAILURE;
      }

      char *s = malloc(strlen(buf) + 1);
      if (s == NULL) {
        vector_dispose(&ftl);
        return FUN_FAILURE;
      }
      strcpy(s, buf);

      if (vector_push(tracker->keys, s) == NULL) {
        vector_dispose(&ftl);
        free(s);
        return FUN_FAILURE;
      }

      if (hashtable_add(tracker->ht, s, ftl) == NULL) {
        vector_dispose(&ftl);
        free(s);
        return FUN_FAILURE;
      }
    }
    if (ftl != NULL) {
      ftrack *ft = NULL;
      if (vector_length(ftl)) {
        ft = vector_get(ftl, vector_length(ftl) - 1);
      }
      if (ft == NULL || ftrack_id(ft) != fid) {
        ft = ftrack_create(fid);
        if (ft == NULL) {
          return FUN_FAILURE;
        }
        if (vector_push(ftl, ft) == NULL) {
          ftrack_dispose(&ft);
          return FUN_FAILURE;
        }
      }

      if (ftrack_addline(ft, n) == NULL) {
        //ftrack_dispose(ft); // SURTOUT PAS ICI !
        return FUN_FAILURE;
      }
    }

    ++n; // débordement possible .? ou QC  déjà majorée par SIZE_MAX
  }
  return FUN_SUCCESS;
}

static int lnt_parsefile(lntracker *tracker, char *fname, size_t fid, bool gen) {
  //printf("--- parsefile : %s, %zu, %d\n", fname, fid, gen);
  FILE *f = fopen(fname, "r");
  if (f == NULL) {
    return FUN_FAILURE;
  }

  int r = FUN_SUCCESS;
  if (lnt_parselines(tracker, f, fid, gen) != 0) {
    r = FUN_FAILURE;
  }

  if (!feof(f)) {
    r = FUN_FAILURE;
  }
  if (fclose(f) != 0) {
    r = FUN_FAILURE;
  }
  return r;
}

//--- Fonctions de lntracker ---------------------------------------------------

lntracker *lntracker_create(size_t (*str_hashfun)(const char *)) {
  lntracker *tracker = malloc(sizeof *tracker);
  tracker->filenames = vector_empty();
  tracker->ht = hashtable_empty((size_t (*)(const void *)) str_hashfun,
      (int (*)(const void *, const void *)) strcmp);
  tracker->keys = vector_empty();
  tracker->opt = scanopt_default();
  return tracker;
}

int lntracker_addfile(lntracker *tracker, char *filename) {
  // controler les doublons ?
  return vector_push(tracker->filenames, filename) == NULL ?
      FUN_FAILURE :
      FUN_SUCCESS;
}

int lntracker_parsefiles(lntracker *tracker) {
  size_t n = FILES_CT(tracker);
  for (size_t i = 0; i < n; ++i) {
    char *fname = vector_get(FILES(tracker), i);
    if (lnt_parsefile(tracker, fname, i, i == 0) != FUN_SUCCESS) {
      return FUN_FAILURE;
    }
  }
  return FUN_SUCCESS;
}

#define PRINT_COLUMN_SEPARATOR "\t"
#define PRINT_LINEID_SEPARATOR ","


static void lnt_display_single(lntracker *t) {

  size_t n = KEYS_CT(t);
  for (size_t i = 0; i < n; ++i) {
    char *s = vector_get(KEYS(t), i);
    const vector *ftl = (const vector *) hashtable_value(HTBL(t), s);
    const ftrack *ft = (const ftrack *) vector_fst(ftl);
    const vector *lines = ftrack_getlines(ft);

    size_t m = vector_length(lines);
    for (size_t k = 0; k < m; ++k) {
      long int *n = vector_get(lines, k);
      printf("%ld", *n);
      if (k + 1 < m) {
        printf(PRINT_LINEID_SEPARATOR);
      }
    }
    printf(PRINT_COLUMN_SEPARATOR);
    printf("%s\n", s);
  }
}


static void lnt_display_multiple(lntracker *t) {

  size_t n = KEYS_CT(t);
  for (size_t i = 0; i < n; ++i) {
    char *s = vector_get(KEYS(t), i);
    const vector *ftl = (const vector *) hashtable_value(HTBL(t), s);

    size_t m = vector_length(ftl);
    for (size_t k = 0; k < m; ++k) {
      const ftrack *ft = (const ftrack *) vector_get(ftl, k);
      const vector *lines = ftrack_getlines(ft);
      size_t m = vector_length(lines);
      printf("%zu", m);
      printf(PRINT_COLUMN_SEPARATOR);
    }
    printf("%s\n", s);
  }
}

void lntracker_display(lntracker *tracker) {

  if (FILES_CT(tracker) == 0) {
    printf("*** Warning : no files to display\n");
    return;
  }

  size_t n = FILES_CT(tracker);
  for (size_t i = 0; i < n; ++i) {
    char *fname = vector_get(FILES(tracker), i);
    printf("%s", fname);
    if (i + 1 < n) {
      printf(PRINT_COLUMN_SEPARATOR);
    }
  }
  printf("\n");

  if (n == 1) {
    lnt_display_single(tracker);
  } else {
    lnt_display_multiple(tracker);
  }
  printf("\n");
}

void lntracker_dispose(lntracker **ptrt) {
  if (*ptrt == NULL) {
    return;
  }
  lntracker *tracker = *ptrt;
  for (size_t i = 0; i < vector_length(tracker->keys); ++i) {
    char * s = (char *) vector_get(tracker->keys, i);
    vector *t = (vector *) hashtable_value(tracker->ht, s);
    for (size_t k = 0; k < vector_length(t); ++k) {
      ftrack *x = (ftrack *) vector_get(t, k);
      ftrack_dispose(&x);
    }
    vector_dispose(&t);
  }

  for (size_t i = 0; i < vector_length(tracker->keys); ++i) {
    char * s = (char *) vector_get(tracker->keys, i);
    free(s);
  }

  vector_dispose(&(tracker->filenames));
  vector_dispose(&(tracker->keys));
  hashtable_dispose(&(tracker->ht));

  *ptrt = NULL;
}
