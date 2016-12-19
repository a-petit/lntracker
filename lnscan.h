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

typedef struct scanopt scanopt;

extern scanopt *scanopt_default();
extern void scanopt_set_transform(scanopt *opt, transform trans);
extern void scanopt_activate_filter(scanopt *opt, filter fltr);
extern bool scanopt_has_active_filter(const scanopt *opt);
extern void scanopt_dispose(scanopt **ptro);

//--- scanner funcions

extern int lnscan_getline(const scanopt *opt, FILE *stream, char *s, size_t m);

#endif  // LN_SCAN_H
