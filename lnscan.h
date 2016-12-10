/*
 * lnscan
 */
#ifndef LN_SCAN_H
#define LN_SCAN_H

#include <stdlib.h>

typedef enum transform {
  TRANSFORM_NONE,
  TRANSFORM_UPPER,
  TRANSFORM_LOWER
} transform;

typedef enum filter {
  FILTER_NONE,
  FILTER_ALNUM,
  FILTER_ALPHA,
  FILTER_CNTRL,
  FILTER_DIGIT,
  FILTER_PUNCT,
  FILTER_SPACE,
} filter;

typedef struct scanopt scanopt;

extern scanopt *scanopt_default();
extern scanopt *scanopt_transform(transform trans);
extern scanopt *scanopt_filter(filter trans);

extern int lnscan_getline(const scanopt *opt, FILE *stream, char *s, size_t m);

#endif  // LN_SCAN_H
