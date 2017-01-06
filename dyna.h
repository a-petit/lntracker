/*
 * DYN-A : "DYNAMIC ARRAY"
 * Tableau dynamique sur le type générique const void *
 */
#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>

typedef struct dyna dyna;

// dyna_empty : crée un tableau vide sur le type void * initialement vide.
//    renvoie un pointeur sur l'objet qui gère ce tableau en cas de succès,
//    et NULL en cas d'échec
extern dyna *dyna_empty(void);

// dyna_push : insère x à la fin du tableau associé à t.
//    renvoie x en cas de succès, NULL en cas d'échec
extern const void *dyna_push(dyna *t, const void * x);

// dyna_get : renvoie l'élément référencé à l'indice i dans le tableau t
//    renvoie x en cas de succès, NULL si l'indice n'est pas valide
extern const void *dyna_get(const dyna *t, size_t i);

// dyna_lst : renvoie le premier élément du tableau t,
//    ou NULL si le tableau est vide
extern const void *dyna_fst(const dyna *t);

// dyna_lst : renvoie le dernier élément du tableau t,
//    ou NULL si le tableau est vide
extern const void *dyna_lst(const dyna *t);

// dyna_length : renvoie la longueur du tableau associé à t
extern size_t dyna_length(const dyna *t);

// dyna_qsort : applique le tri rapide sur les éléments du tableau dynamique t
//    selon l'ordre définit par la fonction compar
extern void dyna_qsort(dyna *t,
    int (*compar)(const void *, const void *));

// dyna_dispose : libère les ressources allouées à *ptrt et donne à *ptrt
//    la valeur NULL. tolère que *ptrt vaille NULL
extern void dyna_dispose(dyna **ptrt);

#endif  // VECTOR_H
