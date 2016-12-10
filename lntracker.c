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

#define ON_VALUE_GOTO(expr, value, label)     \
    if ((expr) == (value)) {                  \
      goto label;                             \
    }


typedef struct lntracker {
  vector *filenames;
  vector *keys;
  scanopt *opt;
  hashtable *ht;
} lntracker;


#define FILES(t)      ((t) -> filenames)
#define HKEYS(t)       ((t) -> keys)
#define HTBLE(t)       ((t) -> ht)
#define SCOPT(t)    ((t) -> opt)

#define FILES_CT(t)   (vector_length(FILES(t)))
#define KEYS_CT(t)    (vector_length(HKEYS(t)))

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

#define PRINT_COLUMN_SEPARATOR "\t"
#define PRINT_LINEID_SEPARATOR ","

static void lnt_display_single(lntracker *t) {

  size_t n = KEYS_CT(t);
  for (size_t i = 0; i < n; ++i) {
    char *s = vector_get(HKEYS(t), i);
    const vector *ftl = (const vector *) hashtable_value(HTBLE(t), s);
    const ftrack *ft = (const ftrack *) vector_fst(ftl);
    const vector *lines = ftrack_getlines(ft);

    size_t m = vector_length(lines);
    if (m > 1) {
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
}

static void lnt_display_multiple(lntracker *t) {

  size_t n = KEYS_CT(t);
  for (size_t i = 0; i < n; ++i) {
    char *s = vector_get(HKEYS(t), i);
    const vector *ftl = (const vector *) hashtable_value(HTBLE(t), s);

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

//--- Fonctions de lntracker ---------------------------------------------------

lntracker *lntracker_create(size_t (*str_hashfun)(const char *)) {
  lntracker *t = malloc(sizeof *t);
  if (t == NULL) {
    return NULL;
  }
  FILES(t) = NULL;
  HKEYS(t) = NULL;
  SCOPT(t) = NULL;
  HTBLE(t) = NULL;

  ON_VALUE_GOTO(FILES(t) = vector_empty(),    NULL, error);
  ON_VALUE_GOTO(HKEYS(t) = vector_empty(),    NULL, error);
  ON_VALUE_GOTO(SCOPT(t) = scanopt_default(), NULL, error);
  ON_VALUE_GOTO(HTBLE(t) = hashtable_empty(
      (size_t (*)(const void *)) str_hashfun,
      (int (*)(const void *, const void *)) strcmp),
      NULL,
      error
      );
  /*
  FILES(t)    = vector_empty();
  ON_VALUE_GOTO(FILES(t), NULL, error);

  HKEYS(t)     = vector_empty();
  ON_VALUE_GOTO(HKEYS(t), NULL, error);

  SCOPT(t)  = scanopt_default();
  ON_VALUE_GOTO(SCOPT(t), NULL, error);

  HTBLE(t)     = hashtable_empty((size_t (*)(const void *)) str_hashfun,
      (int (*)(const void *, const void *)) strcmp);
  ON_VALUE_GOTO(FILE(t), NULL, error);
  */

  goto endfun;

error:
  fprintf(stderr, "*** Error: ask a wizard to enlarge me\n");
  lntracker_dispose(&t);

endfun:
  return t;
}

int lntracker_addfile(lntracker *tracker, char *filename) {
  // ? controler les doublons ?
  if (vector_push(tracker->filenames, filename) == NULL) {
    return FUN_FAILURE;
  }
  return FUN_SUCCESS;
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
  putchar('\n');

  if (n == 1) {
    lnt_display_single(tracker);
  } else {
    lnt_display_multiple(tracker);
  }
  putchar('\n');
}

void lntracker_dispose(lntracker **ptrt) {
  if (*ptrt == NULL) {
    return;
  }

  if (HKEYS(*ptrt)) {
    size_t n = KEYS_CT(*ptrt);
    for (size_t i = 0; i < n; ++i) {
      char * s = (char *) vector_get(HKEYS(*ptrt), i);
      vector *ftl = (vector *) hashtable_value(HTBLE(*ptrt), s);

      size_t m = vector_length(ftl);
      for (size_t k = 0; k < m; ++k) {
        ftrack *ft = (ftrack *) vector_get(ftl, k);
        ftrack_dispose(&ft);
      }
      vector_dispose(&ftl);
      free(s);
    }
  }

  vector_dispose(&FILES(*ptrt));
  vector_dispose(&HKEYS(*ptrt));
  hashtable_dispose(&HTBLE(*ptrt));
  scanopt_dispose(&SCOPT(*ptrt));
  free(*ptrt);
  *ptrt = NULL;
}
