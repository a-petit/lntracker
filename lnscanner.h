/*
 * lnscanner
 */
#ifndef LNSCANNER_H
#define LNSCANNER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum transform {
  TRANSFORM_NONE,
  TRANSFORM_UPPER,
  TRANSFORM_LOWER
} transform;

typedef enum filter {
  FILTER_ALNUM,
  FILTER_ALPHA,
  FILTER_CNTRL,
  FILTER_DIGIT,
  FILTER_PUNCT,
  FILTER_SPACE,
  FILTER_COUNT
} filter;

//--- scanner options

typedef struct lnscanner lnscanner;

extern lnscanner *lnscanner_default();
extern void lnscanner_set_transform(lnscanner *opt, transform trans);
extern void lnscanner_activate_filter(lnscanner *opt, filter fltr);
extern bool lnscanner_has_active_filter(const lnscanner *opt);
extern void lnscanner_dispose(lnscanner **ptro);

//--- scanner functions

extern int lnscanner_getline(const lnscanner *opt, FILE *f, char *s, size_t m);

#endif  // LNSCANNER_H
