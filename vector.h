/*
 * Vector
 * Tableau dynamique sur le type void *
 *
 * push, pop, last, first next ?
 * or apply context
 */
#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct vector vector;

// vector_empty : crée un tableau vide sur le type void * initialement vide.
//   renvoie un pointeur sur l'objet qui gère ce tableau en cas de succès,
//   et NULL en cas d'échec
extern vector *vector_empty(void);

// vector_push : insère x à la fin du tableau associé à t.
//   renvoie x en cas de succès, NULL en cas d'échec
extern const void *vector_push(vector *t, const void * x);


// vector_get : DEPRECATED
// vector_get : renvoie l'élément référencé à l'indice i dans le tableau t.
//   renvoie x en cas de succès, NULL en cas d'échec
extern void *vector_get(const vector *t, size_t i);

// vector_length : DEPRECATED
// vector_length : renvoie la longueur du tableau associé à t
extern size_t vector_length(const vector *t);

// vector_search : DEPRECATED
extern const void *vector_rsearch(vector *t, const void *key,
    int (*cmp)(const void *, const void *));


// vector_fst : renvoie le premier élément du tableau s'il existe et place la
//   tête de lecture sur le premier élément. Si l'élément n'existe pas, renvoie
//   la valeur NULL
extern void vector_reset_iterator(vector *t);
extern const void *vector_iterate(vector *t);

extern const void *vector_fst(const vector *t);
extern const void *vector_lst(const vector *t);
//extern const void *vector_next(vector *t);
//extern int vector_hasnext(vector *t);

// vector_dispose : libère les ressources allouées à *ptrt et donne à *ptrt
//   la valeur NULL. tolère que *ptrt vaille NULL
extern void vector_dispose(vector **ptrt);

#endif  // VECTOR_H
