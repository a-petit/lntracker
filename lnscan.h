/*
 * lnscan
 */
#ifndef LN_SCAN_H
#define LN_SCAN_H

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

typedef struct lnscanopt lnscanopt;

extern lnscanopt *lnscanopt_default();
extern void lnscanopt_set_transform(lnscanopt *opt, transform trans);
extern void lnscanopt_activate_filter(lnscanopt *opt, filter fltr);
extern bool lnscanopt_has_active_filter(const lnscanopt *opt);
extern void lnscanopt_dispose(lnscanopt **ptro);

//--- scanner functions

extern int lnscan_getline(const lnscanopt *opt, FILE *stream, char *s, size_t m);

#endif  // LN_SCAN_H
