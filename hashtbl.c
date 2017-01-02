#include <stdint.h>
#include "hashtbl.h"

#define HT_LDFACT_MAX  0.75
#define HT_NSLOTS_MIN  41
#define HT_RESIZE_MUL  2
#define HT_RESIZE_ADD  25

#define AALLOC(base, nelem)                     \
    if ((nelem) > SIZE_MAX / sizeof(*(base))) { \
      base = NULL;                              \
    } else {                                    \
      base = malloc(nelem * sizeof(*(base)));   \
    }

typedef struct clist clist;

struct clist {
  const void *key;
  const void *value;
  size_t hkey;
  clist *next;
};

struct hashtable {
  size_t (*fun)(const void *);
  int (*compar)(const void *, const void *);
  clist **array;
  size_t nslots;
  size_t nentries;
};

hashtable *hashtable_empty(size_t (*hashfun)(const void *),
    int (*compar)(const void *, const void *)) {
  hashtable *ht = malloc(sizeof *ht);
  if (ht == NULL) {
    return NULL;
  }
  ht -> fun = hashfun;
  ht -> compar = compar;
  AALLOC(ht -> array, HT_NSLOTS_MIN);
  if (ht -> array == NULL) {
    free(ht);
    return NULL;
  }
  for (size_t k = 0; k < HT_NSLOTS_MIN; ++k) {
    ht -> array[k] = NULL;
  }
  ht -> nslots = HT_NSLOTS_MIN;
  ht -> nentries = 0;
  return ht;
}

typedef struct {
  size_t hkey;
  clist **pcurr;
} htsearch;

static clist **hashtable_search(const hashtable *ht, const void *key,
    size_t *ptrhkey) {
  size_t hkey = ht -> fun(key);
  clist * const *pp = &(ht -> array[hkey % ht -> nslots]);
  while (*pp != NULL && ht -> compar((*pp) -> key, key) != 0) {
    pp = &((*pp) -> next);
  }
  if (ptrhkey != NULL) {
    *ptrhkey = hkey;
  }
  return (clist **) pp;
}

static int ht_resize(hashtable *ht, size_t nslots) {
  clist **a;
  AALLOC(a, nslots);
  if (a == NULL) {
    return 1;
  }
  for (size_t k = 0; k < nslots; ++k) {
    a[k] = NULL;
  }
  for (size_t k = 0; k < ht -> nslots; ++k) {
    clist *p = ht -> array[k];
    while (p != NULL) {
      clist **ps = &(a[p -> hkey % nslots]);
      clist *t = p;
      p = t -> next;
      t -> next = *ps;
      *ps = t;
    }
  }
  free(ht -> array);
  ht -> array = a;
  ht -> nslots = nslots;
  return 0;
}

const void *hashtable_add(hashtable *ht, const void *key, const void *value) {
  if (value == NULL) {
    return NULL;
  }
  size_t hkey;
  clist **pp = hashtable_search(ht, key, &hkey);
  if (*pp != NULL) {
    (*pp) -> value = value; // set ???????????????????????????????????????????!!
  } else {
    if (ht -> nentries + 1 > HT_LDFACT_MAX * (double) ht -> nslots) {
      if (ht -> nslots >= (SIZE_MAX - HT_RESIZE_ADD) / HT_RESIZE_MUL
          || ht_resize(ht, ht -> nslots * HT_RESIZE_MUL + HT_RESIZE_ADD)) {
        return NULL;
      }
      pp = hashtable_search(ht, key, NULL);
    }
    *pp = malloc(sizeof **pp);
    if (*pp == NULL) {
      return NULL;
    }
    (*pp) -> key   = key;
    (*pp) -> value = value;
    (*pp) -> hkey  = hkey;
    (*pp) -> next  = NULL;
    ht -> nentries += 1;
  }

  return value;
}

const void *hashtable_remove(hashtable *ht, const void *key) {
  clist **pp = hashtable_search(ht, key, NULL);
  if (*pp == NULL) {
    return NULL;
  }
  clist *t = *pp;
  const void *value = t -> value;
  *pp = t -> next;
  free(t);
  ht -> nentries -= 1;
  return value;
}

const void *hashtable_value(const hashtable *ht, const void *key) {
  clist **pp = hashtable_search(ht, key, NULL);
  return *pp == NULL ? NULL : (*pp) -> value;
}

void hashtable_dispose(hashtable **ptrht) {
  if (*ptrht != NULL) {
    for (size_t k = 0; k < (*ptrht) -> nslots; ++k) {
      clist *p = (*ptrht) -> array[k];
      while (p != NULL) {
        clist *t = p;
        p = p -> next;
        free(t);
      }
    }
    free((*ptrht) -> array);
    free(*ptrht);
    *ptrht = NULL;
  }
}
