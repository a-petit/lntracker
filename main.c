#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "hashtbl.h"
#include "ftrack.h"
#include "lnscan.h"
#include "vector.h"

//------------------------------------------------------------------------------

#define FUN_FAILURE 1
#define FUN_SUCCESS 0

#define STR(s)  #s
#define XSTR(s) STR(s)

// STRINGLEN_MAX : taille maximale des lignes lues.
// Peut être amélioré. Cependant, Si les prefixes de deux lignes de 4095 crctrs
// sont identiques, il est fort à supposer que les lignes sont identiques dans
// leur intégralité.
#define STRINGLEN_MAX (4095 - 1)

#define ON_VALUE_GOTO(expr, value, label)     \
    if ((expr) == (value)) {                  \
      goto label;                             \

static size_t str_hashfun(const char *s);

typedef vector ftrack_list;

typedef struct lntracker {
  vector *filenames;
  hashtable *ht;
  vector *keys;
  scanopt *opt;
} lntracker;

static int parselines(lntracker *tracker, FILE *stream, size_t fid, bool gen) {
  //printf("--- parselines : %zu, %d\n", fid, gen);
  char buf[STRINGLEN_MAX + 1];
  long int n = 0;

  // IB :
  // QC : nombre d'appels à lnscan_getline
  while (lnscan_getline(tracker->opt, stream, buf, STRINGLEN_MAX + 1) == 0) {
    //printf("--- parselines : %s\n", buf);
    ftrack_list *ftl = (ftrack_list *) hashtable_value(tracker->ht, buf);
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

      if (vector_add(tracker->keys, s) == NULL) {
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
        if (vector_add(ftl, ft) == NULL) {
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

static int parsefile(lntracker *tracker, char *fname, size_t fid, bool gen) {
  //printf("--- parsefile : %s, %zu, %d\n", fname, fid, gen);
  FILE *f = fopen(fname, "r");
  if (f == NULL) {
    return FUN_FAILURE;
  }

  int r = FUN_SUCCESS;
  if (parselines(tracker, f, fid, gen) != 0) {
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

static lntracker *lntracker_create() {
  lntracker *tracker = malloc(sizeof *tracker);
  tracker->filenames = vector_empty();
  tracker->ht = hashtable_empty((size_t (*)(const void *)) str_hashfun, (int (*)(const void *, const void *)) strcmp);
  tracker->keys = vector_empty();
  tracker->opt = scanopt_default();
  return tracker;
}

static int lntracker_addfile(lntracker *tracker, char *filename) {
  // controler les doublons ?
  return vector_add(tracker->filenames, filename) == NULL ?
      FUN_FAILURE :
      FUN_SUCCESS;
}

int main(void) {

  lntracker *tracker = lntracker_create();

  lntracker_addfile(tracker, "double.txt");
  lntracker_addfile(tracker, "double.txt");

  parsefile(tracker, vector_get(tracker->filenames, 0), 0, true);


  // display

  // créer des apply context. là c'est vraiment torp lourd

  for (size_t i = 0; i < vector_length(tracker->keys); ++i) {
    char * s = (char *) vector_get(tracker->keys, i);
    const vector *t = hashtable_value(tracker->ht, s);
    for (size_t k = 0; k < vector_length(t); ++k) {
      const ftrack *x = (const ftrack *) vector_get(t, k);
      printf("%zu\t", ftrack_id(x));
    }
    putchar('\n');
    for (size_t k = 0; k < vector_length(t); ++k) {
      const ftrack *x = (ftrack *) vector_get(t, k);
      const vector *lines = ftrack_getlines(x);
      for (size_t i = 0; i < vector_length(lines); ++i) {
        long int *n = (long int *) vector_get(lines, i);
        printf("%ld,", *n);
      }
      putchar('\t');
    }
    printf("\t%s\n", s);
  }

  // free

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

  return EXIT_SUCCESS;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}
