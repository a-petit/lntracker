/*
 * ftrack : file tracked informations
 *
 *    Cette structure de donnée sert à conserver le relevé des occurences d'une
 *    entrée donnée pour un fichier. Chaque entrée possède une liste de ftrack
 *    qui référencient l'ensemble des occurences au travers divers
 *    fichiers.
 */
#ifndef FTRACK_H
#define FTRACK_H

#include <stdlib.h>
#include "dyna.h"

typedef struct ftrack ftrack;

// ftrack_create : créer une structure ftrack d'identifiant fichier fileid
//    En cas d'erreur, la valeur NULL est renvoyée.
extern ftrack *ftrack_create(size_t fileid);

// ftrack_dispose : libère la structure associée à *ptrt et met ptrt à NULL.
//    Tolère que la valeur associée à ptrt soit NULL.
extern void ftrack_dispose(ftrack **ptrt);

// ftrack_addline : mémorise une occurence supplémentaire dans ft. Renvoie n en
//    cas de succès, sinon NULL.
extern const long int *ftrack_addline(ftrack *ft, long int n);

// ftrack_getlines : renvoie le tableau des occurences (numéros de lignes) de ft
extern const dyna *ftrack_getlines(const ftrack *ft);

// ftrack_id : renvoie l'identifiant fichier de ft
extern size_t ftrack_id(const ftrack *ft);

#endif  // FTRACK_H
