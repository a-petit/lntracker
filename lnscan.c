#include <ctype.h>
#include <stdio.h>
#include "lnscan.h"

#define FUN_SUCCESS 0
#define FUN_FAILURE -1

struct scanopt {
  transform t;
  bool filters[FILTER_COUNT];
  //vector filters;
};

//static int funcompar(void *f1, void *f2) {
  //return f1 == f2;
//}

static int (*funfilters[FILTER_COUNT]) (int) = {
  isalnum,
  isalpha,
  iscntrl,
  isdigit,
  ispunct,
  isspace
};

//--- fonctions de scanopt -----------------------------------------------------

scanopt *scanopt_default() {
  scanopt *opt = malloc(sizeof *opt);
  if (opt == NULL) {
    return NULL;
  }
  opt->t = TRANSFORM_NONE;
  for (int i = 0; i < FILTER_COUNT; ++i) {
    opt->filters[i] = false;
  }
  return opt;
}

void scanopt_set_transform(scanopt *opt, transform trans) {
  opt->t = trans;
}

void scanopt_activate_filter(scanopt *opt, filter fltr) {
  //int (*ptrf)(int) = NULL;
  //switch (opt->f) {
    //case FILTER_NONE  : break;
    //case FILTER_ALNUM : ptrf = isalnum; break;
    //case FILTER_ALPHA : ptrf = isalpha; break;
    //case FILTER_CNTRL : ptrf = iscntrl; break;
    //case FILTER_DIGIT : ptrf = isdigit; break;
    //case FILTER_PUNCT : ptrf = ispunct; break;
    //case FILTER_SPACE : ptrf = isspace; break;
  //}
  //if (ptrf && ! vector_search(opt->filters, funcompar, ptrf)) {
    //vector_push(opt->filters, ptrf);
  //}
  opt -> filters[fltr] = true;
}

bool scanopt_has_active_filter(const scanopt *opt) {
  int i = 0;
  while (i < FILTER_COUNT) {
    if (opt->filters[i]) {
      return true;
    }
    ++i;
  }
  return false;
}

void scanopt_dispose(scanopt **ptro) {
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

static void str_filter(const scanopt *opt, char *s) {
  char *p = s;
  while (*s) {
    int i = 0;
    // IB : 0 <= i <= FILTER_COUNT
    //    && les filtres référencés dans funfilters aux indices inférieurs à i
    //    sont soit désactivés, soit négatifs.
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

//--- fonctions de lnscan ------------------------------------------------------

int lnscan_getline(const scanopt *opt, FILE *stream, char *s, size_t m) {
  int c = fgetc(stream);
  if (c == EOF) {
    return FUN_FAILURE;
  }
  char *p = s;
  // IB : s < p < s + m - 1
  //    && (p - s) caractères différents du caractère de fin de ligne ont été
  //    lus sur l'entrée et recopiés dans s
  // QC : (size_t)(p - s)
  while (c != '\n' && (size_t)(p - s) < m - 1) {
    *p = (char) c;
    c = fgetc(stream);
    ++p;
  }
  *p = '\0';

  switch (opt->t) {
    case TRANSFORM_NONE  : break;
    case TRANSFORM_UPPER : str_toupper(s); break;
    case TRANSFORM_LOWER : str_tolower(s); break;
  }

  if (scanopt_has_active_filter(opt)) {
    str_filter(opt, s);
  }

  return  c == '\n' ? FUN_SUCCESS : FUN_FAILURE;
}
