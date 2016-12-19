/*
 * lntracker : programme quasi-principal, en charge de l'analyse des lignes
 * répétées et de leur affichage
 */
#ifndef LNTRACKER_H
#define LNTRACKER_H

#include <stdlib.h>
#include "lnscan.h"


typedef enum sorting {
  SORT_PLAIN,
  SORT_CONTENTS,
  SORT_REPEATED
} sorting;

typedef struct lntracker lntracker;

extern lntracker *lntracker_create(size_t (*str_hashfun)(const char *));

extern void lntracker_dispose(lntracker **ptrt);

extern int lntracker_addfile(lntracker *tracker, char *filename);

extern int lntracker_parsefiles(lntracker *tracker);

extern void lntracker_set_sort(lntracker *tracker, sorting s);

extern void lntracker_display(const lntracker *tracker);

extern scanopt *lntracker_getopt(lntracker *tracker);

#endif // LNTRACKER_H
