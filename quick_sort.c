#include "quick_sort.h"

#include <stdio.h>

void memswap(void * a, void * b, size_t size) {
  if (size == 0u) {
    return;
  }
  char * e1 = (char *) a;
  char * e2 = (char *) b;

  char aux = *e1;
  *e1 = *e2;
  *e2 = aux;

  memswap(e1 + 1, e2 + 1, size - 1u);
}

void * partition(void * base, size_t j, size_t k, size_t size,
    int (*compar)(const void *, const void *))
{
  char * x = (char *) base + k * size;
  char * p = (char *) base + j * size;
  char * q = p;
  // IB : x = (char *) base + k * size
  //    && j <= p <= q <= k
  //    && base[j...p-1] < x <= base[p...q-1]
  //    && base[j...p-1] est une recomposition de sa valeur initiale
  // QC : q
  while(q < x) {
    if (compar(q, x) < 0) {
      memswap(p, q, size);
      p += size;
    }
    q += size;
  }
  memswap(p, x, size);
  return (void *) p;
}

void quick_sort(void *base, size_t nmemb, size_t size,
    int (*compar)(const void *, const void *)) {
  if (nmemb <= 1) {
    return;
  }

  char * p = (char *) partition(base, 0, nmemb-1, size, compar);
  size_t k = (size_t) (p - (char *) base) / size;
  char * b = (char *) base;

  if (k < size - k) {
    quick_sort(b, k, size, compar);
    b = b + (k + 1) * size;
    k = nmemb - (k + 1);
  } else {
    quick_sort(b + (k + 1) * size, nmemb - (k + 1), size, compar);
  }
  quick_sort(b, k, size, compar);
}
