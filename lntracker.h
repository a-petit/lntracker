/*
 * lntracker : programme quasi-principal, en charge de l'analyse des lignes
 * répétées et de leur affichage
 */
#ifndef LNTRACKER_H
#define LNTRACKER_H

#include <stdlib.h>

typedef struct lntracker lntracker;

extern lntracker *lntracker_create(size_t (*str_hashfun)(const char *));

extern int lntracker_addfile(lntracker *tracker, char *filename);

extern int lntracker_parsefiles(lntracker *tracker);

extern void lntracker_display(lntracker *tracker);

extern void lntracker_dispose(lntracker **ptrt);

#endif // LNTRACKER_H
