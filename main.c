#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "hashtbl.h"
#include "ftrack.h"
#include "lnscan.h"
#include "vector.h"

//------------------------------------------------------------------------------

#define FUN_FAILURE 0
#define FUN_SUCCESS -1

#define STR(s)  #s
#define XSTR(s) STR(s)

// STRINGLEN_MAX : taille maximale des lignes lues.
// Peut être amélioré. Cependant, Si les prefixes de deux lignes de 4095 crctrs
// sont identiques, il est fort à supposer que les lignes sont identiques dans
// leur intégralité.
#define STRINGLEN_MAX (4095 - 1)

static size_t str_hashfun(const char *s);

size_t filecount;
char **filenames;
hashtable *ht;
vector *slist;

static int parseFile(char *name, size_t fileid) {
  FILE *f = fopen(name, "r");
  if (f == NULL) {
    return FUN_FAILURE;
  }

  scanopt *opt = lnscan_opt(TRANSFORM_LOWER, FILTER_ALNUM);

  char buf[STRINGLEN_MAX + 1];
  char *s = NULL;
  long int n = 0;
  while (lnscan_getline(opt, f, buf, STRINGLEN_MAX + 1) == 0) {

    vector *t = (vector *) hashtable_value(ht, buf);
    if (t != NULL){
      // ajouter une nouvelle occurences
      ftrack *x = (ftrack *) vector_rsearch(t, 0,
          (int (*)(const void *, const void *))ftrack_compar_id);
      //ftrack *x = (ftrack *) vector_get(t, vector_length(t) - 1);
      ftrack_addline(x, n);
    } else {
      // allouer une chaine de caractères pour la clé
      s = malloc(strlen(buf) + 1);
      if (s == NULL) {
        return FUN_FAILURE;
      }
      strcpy(s, buf);

      // créer un tableau de ftrack
      vector *v = vector_empty();
      if (v == NULL) {
        return FUN_FAILURE;
      }

      {
        // créer une structure ftrack (numéro de fichier, tableau de lignes vides)
        ftrack *x = ftrack_create(fileid);
        if (x == NULL) {
          return FUN_FAILURE;
        }

        if (ftrack_addline(x, n) == NULL) {
          return FUN_FAILURE;
        }

        vector_add(v, x);
      }

      hashtable_add(ht, s, v);
      vector_add(slist, s);
    }

    //fprintf(stdout, "--- string read: '%s'\n", buf);

    ++n;
  }

  free(opt);

  int r = FUN_SUCCESS;
  if (!feof(f)) {
    r = FUN_FAILURE;
  }
  if (fclose(f) != 0) {
    r = FUN_FAILURE;
  }
  return r;
}


int main(void) {

  ht = hashtable_empty((size_t (*)(const void *)) str_hashfun, (int (*)(const void *, const void *)) strcmp);
  if (ht == NULL) {
    return EXIT_FAILURE;
  }

  slist = vector_empty();
  if (slist == NULL) {
    return EXIT_FAILURE;
  }

  filecount = 2;
  filenames = malloc(sizeof(*filenames) * filecount);
  filenames[0] = "double.txt";
  filenames[0] = "input.txt";

  //{"double.txt", "input.txt"};
  parseFile("double.txt", 0);
  parseFile("double.txt", 1);

  // display

  for (size_t i = 0; i < vector_length(slist); ++i) {
    char * s = (char *) vector_get(slist, i);
    const vector *t = hashtable_value(ht, s);
    for (size_t k = 0; k < vector_length(t); ++k) {
      ftrack * x = (ftrack *) vector_get(t, k);
      printf("%zu\t", ftrack_id(x));
    }
    putchar('\n');
    for (size_t k = 0; k < vector_length(t); ++k) {
      ftrack * x = (ftrack *) vector_get(t, k);
      const vector *lines = ftrack_getlines(x);
      for (size_t i = 0; i < vector_length(lines); ++i) {
        long int *n = (long int *) vector_get(lines, i);
        printf("%ld,", *n);
      }
      putchar('\t');
    }
    printf("\t%s\n", s);
  }

  // free

  for (size_t i = 0; i < vector_length(slist); ++i) {
    char * s = (char *) vector_get(slist, i);
    vector *t = (vector *) hashtable_value(ht, s);
    for (size_t k = 0; k < vector_length(t); ++k) {
      ftrack *x = (ftrack *) vector_get(t, k);
      ftrack_dispose(&x);
    }
    vector_dispose(&t);
  }

  for (size_t i = 0; i < vector_length(slist); ++i) {
    char * s = (char *) vector_get(slist, i);
    free(s);
  }
  vector_dispose(&slist);

  free(filenames);
  hashtable_dispose(&ht);

  return EXIT_SUCCESS;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}
