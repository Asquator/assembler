/*Interface to manage the macro list used by assembler preprocessor*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add_macro(char *macro, char **contents);
char **find_macro(char *macro);
void clear_maclist();

char **read_mcontents(FILE *fp, char *until);
void write_mcontents(FILE *fp, char **contents);
