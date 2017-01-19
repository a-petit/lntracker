#include <ctype.h>
#include <stdio.h>
#include <limits.h>
#include "lnscanner.h"

#define FUN_SUCCESS 0
#define FUN_FAILURE -1

#define CHAR_DISABLED 0
#define TABLE_LENGTH (UCHAR_MAX + 1)

struct lnscanner {
  transform tranformation;
  bool filters[FILTER_COUNT];
  int table[TABLE_LENGTH];
};

static int (*funfilters[FILTER_COUNT]) (int) = {
  isalnum,
  isalpha,
  iscntrl,
  isdigit,
  ispunct,
  isspace
};

static int identity(int c) {
  return c;
}

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
  lnscanner_build_table(opt);
  return opt;
}

void lnscanner_dispose(lnscanner **ptro) {
  if (*ptro == NULL) {
    return;
  }
  free(*ptro);
}

void lnscanner_set_transform(lnscanner *opt, transform t) {
  opt->tranformation = t;
}

void lnscanner_set_filter(lnscanner *opt, filter f, bool b) {
  opt->filters[f] = b;
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

void lnscanner_build_table(lnscanner *opt) {
  int k = 0;

  bool haf = lnscanner_has_active_filter(opt);

  int (*trans)(int);
  switch (opt->tranformation) {
    case TRANSFORM_NONE  : trans = identity;  break;
    case TRANSFORM_UPPER : trans = toupper;   break;
    case TRANSFORM_LOWER : trans = tolower;   break;
  }

  while (k < TABLE_LENGTH) {
    int i = 0;
    if (haf) {
      // IB : 0 <= i <= FILTER_COUNT
      //   && les filtres de funfilters associés aux indices allant de 0 à i-1
      //      sont négatifs pour le caractère *s ou bien désactivés.
      // QC : i
      while (! (opt->filters[i] && (*funfilters[i]) (k)) && i < FILTER_COUNT) {
        ++i;
      }
    }
    if (i < FILTER_COUNT) {
      opt->table[k] = trans(k);
    } else {
      opt->table[k] = CHAR_DISABLED;
    }

    ++k;
  }
}

int lnscanner_getline(const lnscanner *opt, FILE *stream, char *s, size_t n) {
  int c = fgetc(stream);
  if (c == EOF) {
    return EOF;
  }

  char *p = s;
  // IB : s < p < s + m - 1
  //    && (p - s) caractères différents du caractère de fin de ligne ont été
  //    lus sur l'entrée et recopiés dans s
  // QC : (size_t)(p - s)
  while (c != '\n' && (size_t)(p - s) < n) {
    c = *(opt->table + c);
    if (c != CHAR_DISABLED) {
      *p = (char) c;
      ++p;
    }
    c = fgetc(stream);
  }
  *p = '\0';

  return c;
}
