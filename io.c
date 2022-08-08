/*Collection of IO functions used by the assembler*/

#include <stdlib.h>

#include "io.h"
#include "status.h"
#include "code_defs.h"

enum {FALSE, TRUE};

/*get_word: get at most max characters of next word in the given line into word, return a pointer to the next character in line*/
char *get_word(char *line, char *word, int max){
	int i;
	if(line == NULL) return NULL;

	/*copying*/
	for(i = 0; *line && !isspace(*line) && i < max; i++)
		*word++ = *line++;

	*word = '\0';
	return line;
}

/*next_word: skip white spaces and call get_word - get at most max characters of the next word and return a pointer to the remainder of line*/
char *next_word(char *line, char *word, int max){
	if(line == NULL) return NULL;
	/*skipping spaces*/
	while(*line && isspace(*line)) line++;
	return get_word(line, word, max);
}

/*fopen_ext_mode: opens a file with the given name, extension and mode, returns a pointer to it*/
FILE *fopen_ext_mode(char *fname, char *ext, char *mode){
	FILE *fp;
	char *fnameNew; /*new name with an extension*/

	fnameNew = malloc(strlen(fname) + strlen(ext) + 1);
	if(!fnameNew) throw(MEM_ALLOC_ERR);
	sprintf(fnameNew, "%s%s", fname, ext);

	/*opening file*/
	if(!(fp = fopen(fnameNew, mode)))
		throw(OPN_FILE_ERR);

	free(fnameNew);
	return fp;
}

/*is_keyword: returns true if word is preserved by the assembler*/
int is_keyword(char *word){
	int i;
	for(i = 0; i < N_OP; i++)
		if(strcmp(word, oplist[i].name) == 0)
			return TRUE;

	for(i = 0; i < N_DIR; i++)
		if(strcmp(word, dirlist[i].name) == 0)
			return TRUE;

	for(i = 0; i < N_OKEY; i++)
		if(strcmp(word, other_keywords[i]) == 0)
			return TRUE;

	return FALSE;
}
