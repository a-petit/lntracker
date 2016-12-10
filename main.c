#include <stdlib.h>
#include <stdio.h>
#include "lntracker.h"


static size_t str_hashfun(const char *s);


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
  //lntracker_addfile(tracker, "./textes/bobdylan_iwysb.txt");
  //lntracker_addfile(tracker, "./textes/moilhiverjepense.txt");
  //lntracker_addfile(tracker, "./textes/toujoursetjamais.txt");
  //lntracker_addfile(tracker, "./textes/essais.txt");
  //lntracker_addfile(tracker, "./textes/orthographe1990.txt");
  //lntracker_addfile(tracker, "./textes/lebourgeoisgentilhomme.txt");
  lntracker_addfile(tracker, "./textes/surleboutdesdoigts.txt");
  lntracker_addfile(tracker, "./textes/surleboutdesdoigts.txt");

  lntracker_parsefiles(tracker);

  lntracker_display(tracker);
  lntracker_dispose(&tracker);

  return EXIT_SUCCESS;
}

// proposer une meilleure fonction de hashage ?

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}
