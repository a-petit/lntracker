#include "lntracker.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "hashtbl.h"
#include "ftrack.h"
#include "lnscanner.h"
#include "dyna.h"

#define FUN_FAILURE 1
#define FUN_SUCCESS 0

#define STRINGLEN_MAX 4095

//--- Structure lntracker ------------------------------------------------------

struct lntracker {
  dyna *filenames;
  dyna *klist;
  dyna *vlist;
  lnscanner *opt;
  sorting sort;
  hashtable *ht;
};

//--- utilitaires ------------------------------------------------

#define ON_VALUE_GOTO(expr, value, label)     \
    if ((expr) == (value)) {                  \
      goto label;                             \
    }

#define LOG_ERR(msg)                                                    \
    fprintf(stderr, "*** Erreur - fichier %s, ligne %d : " msg "\n",    \
        __FILE__, __LINE__);

//--- Raccourcis ---------------------------------------------------------------

#define FILES(t)      ((t) -> filenames)
#define HTABL(t)      ((t) -> ht)
#define HKEYS(t)      ((t) -> klist)
#define HVALS(t)      ((t) -> vlist)
#define SCOPT(t)      ((t) -> opt)

#define FILES_LEN(t)  (dyna_length(FILES(t)))
#define HKEYS_LEN(t)  (dyna_length(HKEYS(t)))
#define HVALS_LEN(t)  (dyna_length(HVALS(t)))

//--- Fonctions locales --------------------------------------------------------

static int lnt_parselines(lntracker *t, FILE *stream, size_t id, bool gen) {
  char buf[STRINGLEN_MAX + 1];
  long int n = 1;
  int c;

  char *s     = NULL;
  dyna *ftl = NULL;
  ftrack *ft  = NULL;

  // IB :
  // QC : nombre d'appels à lnscanner_getline, majoré par LONG_MAX
  while ((c = lnscanner_getline(SCOPT(t), stream, buf, STRINGLEN_MAX)) != EOF) {

    if (c != '\n') {
      fprintf(stderr, "*** Warning: string  %.30s... cut (line %ld)\n", buf, n);
      while (c != '\n') {
        c = fgetc(stream);
      }
    }

    ftl = (dyna *) hashtable_value(HTABL(t), buf);
    if (ftl == NULL && gen) {
      ON_VALUE_GOTO(ftl = dyna_empty(),               NULL, error_phase1);
      ON_VALUE_GOTO(s = malloc(strlen(buf) + 1),      NULL, error_phase1);
      strcpy(s, buf);

      ON_VALUE_GOTO(hashtable_add(HTABL(t), s, ftl),  NULL, error_phase1);
      ON_VALUE_GOTO(dyna_push(HKEYS(t), s),           NULL, error_phase1);
      s = NULL;
      ON_VALUE_GOTO(dyna_push(HVALS(t), ftl),         NULL, error_phase1);
    }
    if (ftl != NULL) {
      ft = NULL;
      if (dyna_length(ftl)) {
        ft = (ftrack *) dyna_lst(ftl);
      }
      if (ft == NULL || ftrack_id(ft) != id) {
        ON_VALUE_GOTO(ft = ftrack_create(id),         NULL, error_phase2);
        ON_VALUE_GOTO(dyna_push(ftl, ft),             NULL, error_phase2);
      }

      ON_VALUE_GOTO(ftrack_addline(ft, n),            NULL, error_phase3);
    }

    ++n;
  }

  return FUN_SUCCESS;

error_phase1:
  LOG_ERR("Création de l'entrée dans la table de hashage.");
  free(s);
  dyna_dispose(&ftl);
  return FUN_FAILURE;

error_phase2:
  LOG_ERR("Création du relevé d'occurences pour le fichier.");
  ftrack_dispose(&ft);
  return FUN_FAILURE;

error_phase3:
  LOG_ERR("Ajout d'une occurence");
  return FUN_FAILURE;
}

static int lnt_open_and_parse_file(lntracker *t, const char *fname,
    size_t id, bool gen) {
  FILE *f = fopen(fname, "r");
  if (f == NULL) {
    LOG_ERR("Impossible d'ouvrir le fichier");
    return FUN_FAILURE;
  }

  int r = FUN_SUCCESS;
  if (lnt_parselines(t, f, id, gen) != 0) {
    r = FUN_FAILURE;
  }

  if (! feof(f)) {
    LOG_ERR("Parcours du fichier incomplet.");
    r = FUN_FAILURE;
  }
  if (fclose(f) != 0) {
    LOG_ERR("Fermeture du fichier.");
    r = FUN_FAILURE;
  }

  return r;
}

#define PRINT_COLUMN_SEPARATOR "\t"
#define PRINT_LINEID_SEPARATOR ","

static void lnt_display_single(const lntracker *t) {

  size_t n = HKEYS_LEN(t);
  for (size_t i = 0; i < n; ++i) {
    const char *s = dyna_get(HKEYS(t), i);
    const dyna *ftl = (const dyna *) hashtable_value(HTABL(t), s);
    const ftrack *ft = (const ftrack *) dyna_fst(ftl);
    const dyna *lines = ftrack_getlines(ft);

    size_t m = dyna_length(lines);
    if (m > 1) {
      for (size_t k = 0; k < m; ++k) {
        const long int *n = dyna_get(lines, k);
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

static void lnt_display_multiple(const lntracker *t) {

  size_t n = HKEYS_LEN(t);
  for (size_t i = 0; i < n; ++i) {
    const char *s = dyna_get(HKEYS(t), i);
    const dyna *ftl = (const dyna *) hashtable_value(HTABL(t), s);

    size_t lastfileID = 0;
    size_t m = dyna_length(ftl);
    if (m > 1) {
      for (size_t k = 0; k < m; ++k) {
        const ftrack *ft = (const ftrack *) dyna_get(ftl, k);

        while (lastfileID < ftrack_id(ft)) {
          printf(PRINT_COLUMN_SEPARATOR);
          ++lastfileID;
        }

        const dyna *lines = ftrack_getlines(ft);
        printf("%zu", dyna_length(lines));
      }

      while (lastfileID < FILES_LEN(t)) {
        printf(PRINT_COLUMN_SEPARATOR);
        ++lastfileID;
      }
      printf("%s\n", s);
    }
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
  HVALS(t) = NULL;
  SCOPT(t) = NULL;
  HTABL(t) = NULL;

  ON_VALUE_GOTO(FILES(t) = dyna_empty(),      NULL, error);
  ON_VALUE_GOTO(HKEYS(t) = dyna_empty(),      NULL, error);
  ON_VALUE_GOTO(HVALS(t) = dyna_empty(),      NULL, error);
  ON_VALUE_GOTO(SCOPT(t) = lnscanner_default(), NULL, error);
  ON_VALUE_GOTO(HTABL(t) = hashtable_empty(
      (size_t (*)(const void *)) str_hashfun,
      (int (*)(const void *, const void *)) strcmp),
      NULL, error);

  t->sort = SORT_PLAIN;

  goto endfun;

error:
  LOG_ERR("Création du tracker, espace insuffisant.");
  lntracker_dispose(&t);

endfun:
  return t;
}

int lntracker_addfile(lntracker *t, char *filename) {
  if (dyna_push(t->filenames, filename) == NULL) {
    return FUN_FAILURE;
  }
  return FUN_SUCCESS;
}

static int str_compar(const char **s1, const char **s2) {
  return strcmp(*s1, *s2);
}

int lntracker_compute(lntracker *t) {
  size_t n = FILES_LEN(t);
  if (n == 0) {
    lnt_parselines(t, stdin, 0, true);
  } else {
    for (size_t i = 0; i < n; ++i) {
      const char *fname = dyna_get(FILES(t), i);
      if (lnt_open_and_parse_file(t, fname, i, i == 0) != FUN_SUCCESS) {
        return FUN_FAILURE;
      }
    }
  }
  if (t->sort == SORT_CONTENTS) {
    dyna_qsort(HKEYS(t), (int (*)(const void *, const void *)) str_compar);
  }
  return FUN_SUCCESS;
}

void lntracker_set_sort(lntracker *tracker, sorting s) {
  tracker->sort = s;
}

lnscanner *lntracker_getopt(lntracker *tracker) {
  return tracker->opt;
}

void lntracker_display(const lntracker *t) {

  size_t n = FILES_LEN(t);
  for (size_t i = 0; i < n; ++i) {
    const char *fname = dyna_get(FILES(t), i);
    printf("%s", fname);
    if (i + 1 < n) {
      printf(PRINT_COLUMN_SEPARATOR);
    }
  }
  putchar('\n');

  if (n > 1) {
    lnt_display_multiple(t);
  } else {
    lnt_display_single(t);
  }
  putchar('\n');
}

void lntracker_dispose(lntracker **ptrt) {
  if (*ptrt == NULL) {
    return;
  }

  if (HKEYS(*ptrt)) {
    size_t n = HKEYS_LEN(*ptrt);
    for (size_t i = 0; i < n; ++i) {
      char * s = (char *) dyna_get(HKEYS(*ptrt), i);
      free(s);
    }
  }

  if (HVALS(*ptrt)) {
    size_t n = HVALS_LEN(*ptrt);
    for (size_t i = 0; i < n; ++i) {
      dyna *ftl = (dyna *) dyna_get(HVALS(*ptrt), i);

      size_t m = dyna_length(ftl);
      for (size_t k = 0; k < m; ++k) {
        ftrack *ft = (ftrack *) dyna_get(ftl, k);
        ftrack_dispose(&ft);
      }
      dyna_dispose(&ftl);
    }
  }

  dyna_dispose(&FILES(*ptrt));
  dyna_dispose(&HKEYS(*ptrt));
  dyna_dispose(&HVALS(*ptrt));
  hashtable_dispose(&HTABL(*ptrt));
  lnscanner_dispose(&SCOPT(*ptrt));
  free(*ptrt);
  *ptrt = NULL;
}
