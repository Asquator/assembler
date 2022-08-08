/*Input/output definitions and declarations used by the assembler*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAXLINE 80 /*Max line length with closing 0*/
#define MAXWORD 31 /*Max length of a single word (keyword or declarator)*/

char *next_word(char *line, char *word, int max);
char *get_word(char *line, char *word, int max);
int is_keyword(char *word);

FILE *fopen_ext_mode(char *fname, char *ext, char *mode);
