//--- Fonctions d'affichage de l'aide (help / man) -----------------------------

#define FORMAT_BOLD      "\033[1m"
#define FORMAT_REGULAR   "\033[0m"
#define FORMAT_UNDERLINE "\033[4m"

static void man_print_title (char *s) {
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
  "\tIf only one FILE is specified, print, for each non empty line occurring\n"
  "\tat least two times in FILE, the numbers increasing sequence of lines \n"
  "\twhose contents are the same and the content of these lines. If more \n"
  "\tthan two FILEs are specified, print, for each FILE, the counts of each \n"
  "\tnon empty line whose content occurs at least one time in all FILEs and \n"
  "\tthe content of the line. If no FILE is specified, read the standard \n"
  "\tinput. \n\n"
  "\tMandatory arguments to long options are mandatory for short options \n"
  "\ttoo.\n\n");

  printf("\t" FORMAT_BOLD "-c, --case" FORMAT_REGULAR "=" FORMAT_UNDERLINE
      "WORD" FORMAT_REGULAR "\n");
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
