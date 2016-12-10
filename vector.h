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

// vector_add : insère x en queue du tableau associé à t.
//   renvoie x en cas de succès, NULL en cas d'échec
extern const void *vector_add(vector *t, const void * x);

// vector_get : renvoie l'élément référencé à l'indice i dans le tableau t.
//   renvoie x en cas de succès, NULL en cas d'échec
extern void *vector_get(const vector *t, size_t i);

// vector_length : renvoie la longueur du tableau associé à t
extern size_t vector_length(const vector *t);

// vector_search : TODO
extern const void *vector_rsearch(vector *t, const void *key,
    int (*cmp)(const void *, const void *));

// vector_dispose : libère les ressources allouées à *ptrt et donne à *ptrt
//   la valeur NULL. tolère que *ptrt vaille NULL
extern void vector_dispose(vector **ptrt);

#endif  // VECTOR_H
