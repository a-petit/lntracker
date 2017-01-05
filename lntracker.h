/*
 * lntracker : programme quasi-principal, en charge de l'analyse des lignes
 * répétées et de leur affichage.
 */
#ifndef LNTRACKER_H
#define LNTRACKER_H

#include <stdlib.h>
#include "lnscanner.h"


typedef enum sorting {
  SORT_PLAIN,
  SORT_CONTENTS
} sorting;

typedef struct lntracker lntracker;

extern lntracker *lntracker_create(size_t (*str_hashfun)(const char *));

// lntracker_addfile : ajoute le fichier filename à la liste des fichiers à
//    traiter par tracker. Renvoie une valeur nulle en cas de succès, une valeur
//    non nulle sinon.
extern int lntracker_addfile(lntracker *tracker, char *filename);

// lntracker_compute : procède au traitement des fichiers précédement ajoutés
//    à tracker (à l'aide de la fonction lntracker_addfile). Si aucun fichier
//    n'a été renseigné alors le programme considère l'entrée standard comme
//    source. Renvoie une valeur nulle en cas de succès, une valeur non nulle
//    en cas d'erreur.
extern int lntracker_compute(lntracker *tracker);

// lntracker_set_sort : définit la façon dont les résultats sont triés suite au
//    traitement
extern void lntracker_set_sort(lntracker *tracker, sorting s);

// lntracker_getopt : renvoie un pointeur vers les options de lecture de tracker
extern lnscanner *lntracker_getopt(lntracker *tracker);

// lntracker_display : affiche
extern void lntracker_display(const lntracker *tracker);

extern void lntracker_dispose(lntracker **ptrt);

#endif // LNTRACKER_H
