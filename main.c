#include <stdlib.h>
#include <stdio.h>
#include "lntracker.h"


// proposer une meilleure fonction de hashage ?
static size_t str_hashfun(const char *s);

#define ON_VALUE_GOTO(expr, value, label)     \
    if ((expr) == (value)) {                  \
      goto label;                             \
    }

int main(void) {

  lntracker *tracker = lntracker_create((size_t (*)(const char *)) str_hashfun);
  if (tracker == NULL) {
    printf("***error : fail to create a line tracker\n");
    return EXIT_FAILURE;
  }

  //lntracker_addfile(tracker, "double.txt");
  //lntracker_addfile(tracker, "double.txt");
  //lntracker_addfile(tracker, "./textes/bescherelle2006.txt");
  //lntracker_addfile(tracker, "./textes/lesmiserables.txt");
  //lntracker_addfile(tracker, "./textes/thebeatles_iwy.txt");
  lntracker_addfile(tracker, "./textes/bobdylan_iwysb.txt");
  //lntracker_addfile(tracker, "./textes/moilhiverjepense.txt");
  //lntracker_addfile(tracker, "./textes/toujoursetjamais.txt");
  //lntracker_addfile(tracker, "./textes/essais.txt");
  //lntracker_addfile(tracker, "./textes/orthographe1990.txt");
  //lntracker_addfile(tracker, "./textes/lebourgeoisgentilhomme.txt");

  //lntracker_addfile(tracker, "./textes/surleboutdesdoigts.txt");
  //lntracker_addfile(tracker, "./textes/surleboutdesdoigts.txt");

  /*
  if (lntracker_addfile(tracker, "./textes/surleboutdesdoigts.txt") != 0) {
    printf("*** error: add file");
    lntracker_dispose(&tracker);
    return EXIT_FAILURE;
  }*/

  if (lntracker_parsefiles(tracker) != 0) {
    printf("*** error: parse files");
    lntracker_dispose(&tracker);
    return EXIT_FAILURE;
  }

  lntracker_display(tracker);
  lntracker_dispose(&tracker);

  return EXIT_SUCCESS;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}
