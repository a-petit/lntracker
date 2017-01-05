#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "lntracker.h"
#include "lnscanner.h"

static size_t str_hashfun(const char *s);

static void help(char **argv);

//--- Traitement des options ---------------------------------------------------

#define GETFUN_FAILURE  0
#define GETFUN_SUCCESS  1
#define GETFUN_CUTSHORT 2

#define FILE_FLAG_SHRT "-"
#define FILE_FLAG_LONG "--"

// getoptions : procède au traitement des options passées en ligne de commande.
//   Renvoie 1 en cas de succès, 2 dans le cas ou une option demande
//   l'interruption normale du programme, et 0 en cas d'erreur.
static int getoptions(int argc, char **argv, lntracker *tracker);

// getfiles : procède à la récupération des fichier passés en ligne de commande.
//    Les fichiers sont séquentiellement ajoutés à la liste des fichiers à
//    traiter par tracker. Renvoie 0 si un ajout n'a pas pu s'effectuer, sinon,
//    si tout c'est déroulé correctement, renvoie 0.
static int getfiles(int argc, char **argv, lntracker *tracker);

//--- MACRO

#define ON_VALUE_EXITERR(x, value, tracker, msg)      \
    if ((x) == (value)) {                             \
      fprintf(stderr, "*** Erreur : " msg "\n");      \
      lntracker_dispose(&tracker);                    \
      return EXIT_FAILURE;                            \
    }

#define ON_NOT_VALUE_EXITERR(x, value, tracker, msg)      \
    if ((x) != (value)) {                             \
      fprintf(stderr, "*** Erreur : " msg "\n");      \
      lntracker_dispose(&tracker);                    \
      return EXIT_FAILURE;                            \
    }

//--- Programme principal ------------------------------------------------------

#include <time.h>
int main(int argc, char **argv) {
  printf("%ld\n", time(NULL));
  srand((unsigned int) time(NULL));

  lntracker *tracker = lntracker_create((size_t (*)(const char *)) str_hashfun);
  ON_VALUE_EXITERR(tracker, NULL, tracker, "allocation d'un tracker.");

  int r;

  r = getoptions(argc, argv, tracker);
  ON_VALUE_EXITERR(r, GETFUN_FAILURE, tracker,
      "Try './original --help' for more information.");

  if (r == GETFUN_CUTSHORT) {
    lntracker_dispose(&tracker);
    return EXIT_SUCCESS;
  }

  r = getfiles(argc, argv, tracker);
  ON_VALUE_EXITERR(r, GETFUN_FAILURE, tracker, "ajout des fichiers.");

  r = lntracker_compute(tracker);
  ON_NOT_VALUE_EXITERR(r, 0, tracker, "traitement des fichiers.");

  lntracker_display(tracker);
  lntracker_dispose(&tracker);

  return EXIT_SUCCESS;
}

//--- Fonctions locales --------------------------------------------------------

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

int getoptions(int argc, char **argv, lntracker *tracker) {
  int c;

  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"help",    no_argument,        NULL,  'h'},
      {"case",    required_argument,  NULL,  'c'},
      {"filter",  required_argument,  NULL,  'f'},
      {"sort",    required_argument,  NULL,  'S'},
      {0,         0,                  NULL,  0}
    };

    c = getopt_long(argc, argv, "hc:f:S:", long_options, &option_index);
    if (c == -1) {
      break;
    }

    switch (c) {
      case 'h':
        help(argv);
        return GETFUN_CUTSHORT;

      case 'c':
        if (strcasecmp(optarg, "upper") == 0) {
          lnscanner_set_transform(lntracker_getopt(tracker), TRANSFORM_UPPER);
        } else if (strcasecmp(optarg, "lower") == 0) {
          lnscanner_set_transform(lntracker_getopt(tracker), TRANSFORM_LOWER);
        } else {
          return GETFUN_FAILURE;
        }
        break;

      case 'f':
        while (*optarg != '\0') {
          lnscanner *opt = (lnscanner *) lntracker_getopt(tracker);
          switch (*optarg) {
            case 'a': lnscanner_activate_filter(opt, FILTER_ALPHA); break;
            case 'c': lnscanner_activate_filter(opt, FILTER_CNTRL); break;
            case 'd': lnscanner_activate_filter(opt, FILTER_DIGIT); break;
            case 'n': lnscanner_activate_filter(opt, FILTER_ALNUM); break;
            case 'p': lnscanner_activate_filter(opt, FILTER_PUNCT); break;
            case 's': lnscanner_activate_filter(opt, FILTER_SPACE); break;
            default: return GETFUN_FAILURE;
          }
          ++optarg;
        }
        break;

      case 'S':
        if (strcasecmp(optarg, "plain") == 0) {
          lntracker_set_sort(tracker, SORT_PLAIN);
        } else if (strcasecmp(optarg, "contents") == 0) {
          lntracker_set_sort(tracker, SORT_CONTENTS);
        } else {
          return GETFUN_FAILURE;
        }
        break;

      case '?':
        return GETFUN_FAILURE;
        break;

      default:
        printf("?? getopt returned character code 0%o ??\n", c);
        return GETFUN_FAILURE;
    }
  }

  return GETFUN_SUCCESS;
}

int getfiles(int argc, char **argv, lntracker *tracker) {
  if (optind < argc) {
    while (optind < argc) {
      // --- [!]
      if (strcmp(argv[optind], FILE_FLAG_SHRT) == 0) {
        ++optind;
        continue;
      }
      if (lntracker_addfile(tracker, argv[optind])  != 0) {
        return GETFUN_FAILURE;
      }
      ++optind;
    }
  }

  return GETFUN_SUCCESS;
}

#define HELP_MSG                                                               "\
If only one FILE is specified, print, for each non empty line occurring at least\n\
two times in FILE, the numbers increasing sequence of lines whose contents are  \n\
the same and the content of these lines. If more than two FILEs are specified,  \n\
print, for each FILE, the counts of each non empty line whose content occurs at \n\
least one time in all FILEs and the content of the line. If no FILE is          \n\
specified, read the standard input.                                             \n\
Mandatory arguments to long options are mandatory for short options too.        \n\
                                                                                \n\
  -c, --case=WORD   transform lines before treating them. Available values for  \n\
                    WORD are lower, upper, or none (no transformation). Default \n\
                    is none                                                     \n\
                                                                                \n\
  -f, --filter=STRING    keep of lines only specified characters. Parameter     \n\
                    STRING is a concatenation of letters among the following: a \n\
                    (alphabetic), c (control), d (digit), n (alphanumerical,    \n\
                    i.e. alphabetic or digit), p (punctuation), s (space), or * \n\
                    (all). Default is *                                         \n\
                                                                                \n\
  -s, --sort=WORD   sort the result. Available values for WORD are contents     \n\
                    (lexicographically by lines) or plain (in the lines' order  \n\
                    in the first of the specified FILEs). Default is plain      \n\
                                                                                \n\
      --help        display this help and exit                                  \n\
                                                                                \n\
      --man         display this help following man-style and exit              \n\
                                                                                \n\
      --version     output version information and exit                         \n\
"
void help(char **argv) {
  char *prog = argv[0];
  while (ispunct(*prog)) {
    ++prog;
  }
  printf("Usage: %s [OPTION]... [FILE]...\n\n", prog);
  printf(HELP_MSG);
}
