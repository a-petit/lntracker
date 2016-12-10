#ifndef HASHTBL_H
#define HASHTBL_H

#include <stdlib.h>

typedef struct hashtable hashtable;

extern hashtable *hashtable_empty(size_t (*hashfun)(const void *),
    int (*compar)(const void *, const void *));
extern const void *hashtable_add(hashtable *ht, const void *key,
    const void *value);
extern const void *hashtable_remove(hashtable *ht, const void *key);
extern const void *hashtable_value(const hashtable *ht, const void *key);
extern void hashtable_dispose(hashtable **ptrht);

#endif  // HASHTBL_H
