#include <stdlib.h>
#include <stdio.h>
#include "lntracker.h"


static size_t str_hashfun(const char *s);


int main(void) {

  lntracker *tracker = lntracker_create((size_t (*)(const char *)) str_hashfun);

  lntracker_addfile(tracker, "double.txt");
  //lntracker_addfile(tracker, "double.txt");

  lntracker_parsefiles(tracker);

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
