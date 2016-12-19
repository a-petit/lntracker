#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>
#include "lntracker.h"


// proposer une meilleure fonction de hashage ?
static size_t str_hashfun(const char *s);

#define FUN_FAILURE 1
#define FUN_SUCCESS 0

#define FILE_FLAG_SHRT "-"
#define FILE_FLAG_LONG "--"

enum options_type {
  OPT_HELP,
  OPT_CASE,
  OPT_FILTER,
  OPT_SORT
};

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

    printf("*** %c option found: %s\n", c, optarg);

    switch (c) {
      case 'h':
        printf("HELP option\n");
        break;

      case 'c':
        if (strcasecmp(optarg, "upper") == 0) {
          scanopt_set_transform(lntracker_getopt(tracker), TRANSFORM_UPPER);
        } else if (strcasecmp(optarg, "lower") == 0) {
          scanopt_set_transform(lntracker_getopt(tracker), TRANSFORM_LOWER);
        }
        break;

      case 'f':
        while (*optarg != '\0') {
          scanopt *opt = (scanopt *) lntracker_getopt(tracker);
          switch (*optarg) {
            case 'a': scanopt_activate_filter(opt, FILTER_ALPHA); break;
            case 'c': scanopt_activate_filter(opt, FILTER_CNTRL); break;
            case 'd': scanopt_activate_filter(opt, FILTER_DIGIT); break;
            case 'n': scanopt_activate_filter(opt, FILTER_ALNUM); break;
            case 'p': scanopt_activate_filter(opt, FILTER_PUNCT); break;
            case 's': scanopt_activate_filter(opt, FILTER_SPACE); break;
            default: printf("invalid arg: '%c'\n", *optarg);
          }
          ++optarg;
        }
        break;

      case 'S':
        if (strcasecmp(optarg, "plain") == 0) {
          lntracker_set_sort(tracker, SORT_PLAIN);
        } else if (strcasecmp(optarg, "contents") == 0) {
          lntracker_set_sort(tracker, SORT_CONTENTS);
        } else if (strcasecmp(optarg, "reapeated") == 0) {
          lntracker_set_sort(tracker, SORT_REPEATED);
        }
        break;

      case '?':
        break;

      default:
        printf("?? getopt returned character code 0%o ??\n", c);
    }
  }

  if (optind < argc) {
    printf("non-option ARGV-elements: ");
    while (optind < argc) {
      printf("\n%s", argv[optind]);
      // --- FIXME
      if (strcmp(argv[optind], FILE_FLAG_SHRT) == 0) {
        ++optind;
        continue;
      }
      // --- ^^^^^
      if (lntracker_addfile(tracker, argv[optind])  != 0) {
        printf("*** error: adding file\n");
        lntracker_dispose(&tracker);
        return FUN_FAILURE;
      }
      ++optind;
    }
    printf("\n");
    printf("\n");
  }
  return FUN_SUCCESS;
}

int main(int argc, char **argv) {

  lntracker *tracker = lntracker_create((size_t (*)(const char *)) str_hashfun);
  if (tracker == NULL) {
    printf("***error: fail to create a line tracker\n");
    return EXIT_FAILURE;
  }

  getoptions(argc, argv, tracker);

  if (lntracker_parsefiles(tracker) != 0) {
    printf("*** error: parsing files\n");
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
