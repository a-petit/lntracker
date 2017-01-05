#include <ctype.h>
#include <stdio.h>
#include "lnscanner.h"

#define FUN_SUCCESS 0
#define FUN_FAILURE -1

struct lnscanner {
  transform tranformation;
  bool filters[FILTER_COUNT];
};

static int (*funfilters[FILTER_COUNT]) (int) = {
  isalnum,
  isalpha,
  iscntrl,
  isdigit,
  ispunct,
  isspace
};

//--- fonctions de lnscanner ---------------------------------------------------

lnscanner *lnscanner_default() {
  lnscanner *opt = malloc(sizeof *opt);
  if (opt == NULL) {
    return NULL;
  }
  opt->tranformation = TRANSFORM_NONE;
  for (int i = 0; i < FILTER_COUNT; ++i) {
    opt->filters[i] = false;
  }
  return opt;
}

void lnscanner_set_transform(lnscanner *opt, transform t) {
  opt->tranformation = t;
}

void lnscanner_activate_filter(lnscanner *opt, filter f) {
  opt->filters[f] = true;
}

bool lnscanner_has_active_filter(const lnscanner *opt) {
  int i = 0;
  while (i < FILTER_COUNT) {
    if (opt->filters[i]) {
      return true;
    }
    ++i;
  }
  return false;
}

void lnscanner_dispose(lnscanner **ptro) {
  if (*ptro == NULL) {
    return;
  }
  free(*ptro);
}

//--- fonctions locales --------------------------------------------------------

#define DEFUN_STR_TRANSFORM(fun, trans)       \
    static void fun(char *s) {                \
      while (*s) {                            \
        *s = (char) trans(*s);                \
        ++s;                                  \
      }                                       \
    }

DEFUN_STR_TRANSFORM(str_toupper, toupper)
DEFUN_STR_TRANSFORM(str_tolower, tolower)

static void str_filter(const lnscanner *opt, char *s) {
  char *p = s;
  while (*s) {
    int i = 0;
    // IB : 0 <= i <= FILTER_COUNT
    //    && les filtres de funfilters associés aux indices allant de 0 à i-1
    //    sont négatifs pour le caractère *s, ou bien déactivés.
    // QC : i
    while (! (opt->filters[i] && (*funfilters[i]) (*s)) && i < FILTER_COUNT) {
      ++i;
    }
    if (i < FILTER_COUNT) {
      *p = (char) *s;
      ++p;
    }
    ++s;
  }
  *p = *s;
}

//--- fonctions de lnscanner ---------------------------------------------------

int lnscanner_getline(const lnscanner *opt, FILE *stream, char *s, size_t n) {
  int c = fgetc(stream);
  if (c == EOF) {
    return EOF;
  }
  while (isspace(c)) {
    c = fgetc(stream);
  }
  char *p = s;
  // IB : s < p < s + m - 1
  //    && (p - s) caractères différents du caractère de fin de ligne ont été
  //    lus sur l'entrée et recopiés dans s
  // QC : (size_t)(p - s)
  while (c != '\n' && (size_t)(p - s) < n) {
    *p = (char) c;
    c = fgetc(stream);
    ++p;
  }
  *p = '\0';

  switch (opt->tranformation) {
    case TRANSFORM_NONE  : break;
    case TRANSFORM_UPPER : str_toupper(s); break;
    case TRANSFORM_LOWER : str_tolower(s); break;
  }

  if (lnscanner_has_active_filter(opt)) {
    str_filter(opt, s);
  }

  return  c;
}
