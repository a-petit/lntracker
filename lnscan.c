#include <ctype.h>
#include <stdio.h>
#include "lnscan.h"

#define FUN_SUCCESS 0
#define FUN_FAILURE -1

struct scanopt {
  transform t;
  filter f;
};

//--- fonctions de scanopt -----------------------------------------------------

extern scanopt *scanopt_default() {
  scanopt *opt = malloc(sizeof *opt);
  if (opt == NULL) {
    return NULL;
  }
  opt->t = TRANSFORM_NONE;
  opt->f = FILTER_NONE;
  return opt;
}

extern void scanopt_settransform(scanopt *opt, transform trans) {
  opt->t = trans;
}

extern void scanopt_setfilter(scanopt *opt, filter fltr) {
  opt->f = fltr;
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

#define DEFUN_STR_FILTER(fun, pattern)        \
    static void fun(char *s) {                \
      char *p = s;                            \
      while (*s) {                            \
        if (pattern(*s)) {                    \
          *p = (char) *s;                     \
          ++p;                                \
        }                                     \
        ++s;                                  \
      }                                       \
      *p = *s;                                \
    }

DEFUN_STR_FILTER(str_alnum, isalnum)
DEFUN_STR_FILTER(str_alpha, isalpha)
DEFUN_STR_FILTER(str_cntrl, iscntrl)
DEFUN_STR_FILTER(str_digit, isdigit)
DEFUN_STR_FILTER(str_punct, ispunct)
DEFUN_STR_FILTER(str_space, isspace)

//--- fonctions de lnscan ------------------------------------------------------

int lnscan_getline(const scanopt *opt, FILE *stream, char *s, size_t m) {
  int c = fgetc(stream);
  if (c == EOF) {
    return FUN_FAILURE;
  }
  char *p = s;
  // IB : s < p < s + m - 1
  //      && (p - s) caractères différents du caractère de fin de ligne ont été
  //         lus sur l'entrée et recopiés dans s
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

  switch (opt->f) {
    case FILTER_NONE  : break;
    case FILTER_ALNUM : str_alnum(s); break;
    case FILTER_ALPHA : str_alpha(s); break;
    case FILTER_CNTRL : str_cntrl(s); break;
    case FILTER_DIGIT : str_digit(s); break;
    case FILTER_PUNCT : str_punct(s); break;
    case FILTER_SPACE : str_space(s); break;
  }

  return  c == '\n' ? FUN_SUCCESS : FUN_FAILURE;
}
