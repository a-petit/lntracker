/*
 * lnscanner : module dédié à la lecture et au traitement des lignes d'un flot
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

// lnscanner_default : renvoie un lnscanner (structure d'options) nécessaire à
//    l'appel de lnscanner_getline. Par défaut, l'option de transformation est
//    réglée sur TRANSFORM_NONE et tous les filtres sont désactivés
extern lnscanner *lnscanner_default();

// lnscanner_dispose : libère la structure de donnée associée à ptro
extern void lnscanner_dispose(lnscanner **ptro);

// lnscanner_set_transform : définit l'option de transformation parmis celles
//    proposés par l'énumération transform. Par défaut, l'option est réglée sur
//    la valeur TRANSFORM_NONE
extern void lnscanner_set_transform(lnscanner *opt, transform trans);

// lnscanner_set_filter : active / désactive un filtre spécifique. Par défaut,
//    tous les filtres sont désactivé. Plusieurs filtres peuvent être activés
//    simultanément. Si aucun filtre n'est activé alors tous les caractères sont
//    conservés. Sinon, seuls les caractères qui appartiennent au moins à l'un
//    des ensembles filtrés sont conservés.
extern void lnscanner_set_filter(lnscanner *opt, filter fltr, bool active);

// lnscanner_has_active_filter : renvoie true si et seulement si au moins un
//    filtre est activé.
extern bool lnscanner_has_active_filter(const lnscanner *opt);

// lnscanner_initialize_table : construit l'image de la table ASCII par
//    l'application des fonctions de filtrage et de transformation définies par
//    opt. La table est alors mémorisée dans la structure opt et sera effective
//    dans les appels à la fonction lnscanner_getline.
extern void lnscanner_build_table(lnscanner *opt);

// lnscanner_getline : Lit une ligne sur le flot f et mémorise le contenu de
//    cette ligne ligne dans le buffer s. Les filtres et transformations définis
//    dans opt sont ensuite appliqués au buffer. Si la taille de la ligne est
//    supérieure à la taille du buffer, c'est à dire supérieure à n, la lecture
//    s'arrête au n-ième caractère. La fonction renvoie le dernier caractère lu.
//    Attention, la chaine renvoyée peut être une châine vide.
extern int lnscanner_getline(const lnscanner *opt, FILE *f, char *s, size_t n);

#endif  // LNSCANNER_H
