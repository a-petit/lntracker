#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "lntracker.h"
#include "lnscanner.h"

static size_t str_hashfun(const char *s);

#define HELP_MSG                                                                \
"Usage: original [OPTION]... [FILE]...                                          \n\
                                                                                \n\
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

#define MAN_MSG                                                                "\
NAME                                                                            \n\
        original - track text lines                                             \n\
                                                                                \n\
SYNOPSIS                                                                        \n\
        original [OPTION]... [FILE]...                                          \n\
                                                                                \n\
DESCRIPTION                                                                     \n\
        If only one FILE is specified, print, for each non empty line occurring \n\
        at least  two times  in FILE,  the numbers increasing sequence of lines \n\
        whose contents  are the  same and  the content  of these lines. If more \n\
        than two  FILEs are specified, print, for each FILE, the counts of each \n\
        non empty  line whose content occurs at least one time in all FILEs and \n\
        the content  of the  line. If  no FILE  is specified, read the standard \n\
        input.                                                                  \n\
                                                                                \n\
        Mandatory arguments  to long  options are  mandatory for  short options \n\
        too.                                                                    \n\
                                                                                \n\
        -c, --case=WORD                                                         \n\
                transform lines before treating them. Available values for WORD \n\
                are lower, upper, or none (no transformation). Default is none  \n\
                                                                                \n\
        -f, --filter=STRING                                                     \n\
                keep of  lines only specified characters. Parameter STRING is a \n\
                concatenation of letters among the following: a (alphabetic), c \n\
                (control),  d (digit),  n (alphanumerical,  i.e. alphabetic  or \n\
                digit), p (punctuation), s (space), or * (all). Default is *    \n\
                                                                                \n\
        -s, --sort=WORD                                                         \n\
                sort  the  result.  Available  values  for  WORD  are  contents \n\
                (lexicographically by  lines) or  plain (in the lines' order in \n\
                the first of the specified FILEs). Default is plain             \n\
                                                                                \n\
        --help  display this help and exit                                      \n\
                                                                                \n\
        --man   display this help following man-style and exit                  \n\
                                                                                \n\
        --version                                                               \n\
                output version information and exit                             \n\
                                                                                \n\
AUTHOR                                                                          \n\
        Written by Ballet Psychique 2016/12/16                                  \n\
                                                                                \n\
COPIRATE                                                                        \n\
        This is  free software:  you are  free to  redistribute it. There is NO \n\
        WARRANTY.                                                               \n\
"
// replace '        ' -> \t
// replace (\\t)+     -> ''

//--- Traitement des options ---------------------------------------------------

#define GETFUN_FAILURE  0
#define GETFUN_SUCCESS  1
#define GETFUN_CUTSHORT 2

#define FILE_FLAG_SHRT "-"
#define FILE_FLAG_LONG "--"

// getoptions : procède au traitement des options passées en ligne de commande.
//   Renvoie 1 en cas de succès, 2 dans le cas ou une option demande
//   l'interruption normale du programme, et 0 en cas d'erreur.
int getoptions(int argc, char **argv, lntracker *tracker);

// getfiles : procède à la récupération des fichier passés en ligne de commande.
//    Les fichiers sont séquentiellement ajoutés à la liste des fichiers à
//    traiter par tracker. Renvoie 0 si un ajout n'a pas pu s'effectuer, sinon,
//    si tout c'est déroulé correctement, renvoie 0.
int getfiles(int argc, char **argv, lntracker *tracker);

//--- Programme principal ------------------------------------------------------

int main(int argc, char **argv) {

  lntracker *tracker = lntracker_create((size_t (*)(const char *)) str_hashfun);
  if (tracker == NULL) {
    printf("***error: fail to create a line tracker\n");
    return EXIT_FAILURE;
  }

  int r = getoptions(argc, argv, tracker);
  if (r == GETFUN_FAILURE) {
    printf("Try './original --help' for more information.\n");                  //
    lntracker_dispose(&tracker);                                                //
    return EXIT_FAILURE;
  } else if (r == GETFUN_CUTSHORT) {
    lntracker_dispose(&tracker);
    return EXIT_SUCCESS;
  }

  r = getfiles(argc, argv, tracker);
  if (r == GETFUN_FAILURE) {
    printf("*** error: adding file\n");                                         //
    lntracker_dispose(&tracker);                                                //
    return EXIT_FAILURE;
  }


  if (lntracker_parsefiles(tracker) != 0) {
    printf("*** error: parsing files\n");                                       //
    lntracker_dispose(&tracker);                                                //
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

char *getprogname(char **argv) {
  char *s = argv[0];
  while (ispunct(*s)) {
    ++s;
  }
  return s;
}

int getoptions(int argc, char **argv, lntracker *tracker) {
  int c;

  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"help",    no_argument,        NULL,  'h'},
      {"man",     no_argument,        NULL,  'm'},
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
        printf(HELP_MSG);
        return GETFUN_CUTSHORT;

      case 'm':
        man(argv);
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
      // --- FIXME
      if (strcmp(argv[optind], FILE_FLAG_SHRT) == 0) {
        ++optind;
        continue;
      }
      // --- ^^^^^
      if (lntracker_addfile(tracker, argv[optind])  != 0) {
        return GETFUN_FAILURE;
      }
      ++optind;
    }
  }

  return GETFUN_SUCCESS;
}

//------------------------------------------------------------------------------

#define FORMAT_BOLD      "\033[1m"
#define FORMAT_REGULAR   "\033[0m"
#define FORMAT_UNDERLINE "\033[4m"

void man_print_title (char *s) {
  printf(FORMAT_BOLD "%s" FORMAT_REGULAR "\n", s);
}

void man(char **argv) {
  char *progname = getprogname(argv);
  man_print_title("NAME");
  printf("\t%s - track text lines\n\n", progname);

  man_print_title("SYNOPSIS");
  printf("\t" FORMAT_BOLD "%s" FORMAT_REGULAR " [" FORMAT_UNDERLINE "OPTION"
      FORMAT_REGULAR "]... [" FORMAT_UNDERLINE "FILE" FORMAT_REGULAR
      "]... \n\n", progname);

  man_print_title("DESCRIPTION");
  printf(
  "\tIf only one FILE is specified, print, for each non empty line occurring \n"
  "\tat least two times in FILE, the numbers increasing sequence of lines \n"
  "\twhose contents are the same and the content of these lines. If more \n"
  "\tthan two FILEs are specified, print, for each FILE, the counts of each \n"
  "\tnon empty line whose content occurs at least one time in all FILEs and \n"
  "\tthe content of the line. If no FILE is specified, read the standard \n"
  "\tinput. \n\n"
  "\tMandatory arguments to long options are mandatory for short options \n"
  "\ttoo.\n\n");

  printf("\t" FORMAT_BOLD "-c, --case" FORMAT_REGULAR "=" FORMAT_UNDERLINE "WORD"
      FORMAT_REGULAR "\n");
  printf("\t\ttransform lines before treating them. Available values for WORD\n"
      "\t\tare " FORMAT_BOLD "lower" FORMAT_REGULAR ", " FORMAT_BOLD "upper"
      FORMAT_REGULAR ", or " FORMAT_BOLD "none" FORMAT_REGULAR
      " (no transformation). Default is " FORMAT_BOLD "none" FORMAT_REGULAR
      " \n\n");

  printf("\t" FORMAT_BOLD "-f, --filter" FORMAT_REGULAR "=" FORMAT_UNDERLINE
      "STRING" FORMAT_REGULAR "\n");
  printf("\t\tkeep of lines only specified characters. Parameter STRING is a \n"
      "\t\tconcatenation of letters among the following: " FORMAT_BOLD "a"
      FORMAT_REGULAR " (alphabetic), " FORMAT_BOLD "c " FORMAT_REGULAR
      " \n\t\t(control), " FORMAT_BOLD "d" FORMAT_REGULAR " (digit), "
      FORMAT_BOLD "n" FORMAT_REGULAR
      " (alphanumerical, i.e. alphabetic or \n\t\tdigit), " FORMAT_BOLD "p"
      FORMAT_REGULAR " (punctuation), " FORMAT_BOLD "s" FORMAT_REGULAR
      " (space), or " FORMAT_BOLD "*" FORMAT_REGULAR " (all). Default is "
      FORMAT_BOLD "*" FORMAT_REGULAR " \n\n");

  printf("\t" FORMAT_BOLD "-s" FORMAT_REGULAR ", " FORMAT_BOLD "--sort"
      FORMAT_REGULAR "=" FORMAT_UNDERLINE "WORD" FORMAT_REGULAR "\n");
  printf("\t\tsort the result. Available values for WORD are " FORMAT_BOLD
      "contents" FORMAT_REGULAR " \n\t\t(lexicographically by lines) or "
      FORMAT_BOLD "plain" FORMAT_REGULAR " (in the lines' order in \n"
      "\t\tthe first of the specified FILEs). Default is " FORMAT_BOLD "plain"
      FORMAT_REGULAR "\n\n");

  printf("\t" FORMAT_BOLD "--help" FORMAT_REGULAR
      "\tdisplay this help and exit \n\n");

  printf("\t" FORMAT_BOLD "--man" FORMAT_REGULAR
      "\tdisplay this help following man-style and exit \n\n");

  man_print_title("AUTHOR");
  printf("\tWritten by Ballet Psychique 2016/31/12.\n\n");

  man_print_title("COPYRIGHT");
  printf("\tThis is free software: you are free to redistribute it."
      " There is NO \n\tWARRANTY. \n");
}
