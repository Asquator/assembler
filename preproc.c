/*Assembler preprocessor that treats macros, ignoring errors*/
#include <stdlib.h>
#include "io.h"
#include "status.h"
#include "macro_list.h"
#include "parse.h"

static void expand_macros(FILE *fpsrc, FILE *fpexp);
static void proc_new_macro(char *name, char **contents);

enum {FALSE, TRUE};

/*preproc: gets a filename and preprocesses the asm code*/
int preproc(char *fname){
	FILE *fpsrc, *fpexp; /*source and expanded files*/
	fpsrc = fopen_ext_mode(fname, ".as", "r");
	fpexp = fopen_ext_mode(fname, ".am", "w");

	if(!fpexp || !fpsrc) throw(OPN_FILE_ERR);

	expand_macros(fpsrc, fpexp);

	fclose(fpsrc);
	fclose(fpexp);

	clear_maclist(); /*preparing the list for new asm file*/
	return 0;
}

/*expand macros: copy fpsrc to fpexp while saving and expanding macros*/
static void expand_macros(FILE *fpsrc, FILE *fpexp){
		char line[MAXLINE + 1];
		char word[MAXWORD + 1];
		char *last = line;	/*pointer to the next word*/

		/*name and macro contents*/
		char **contents;

		while(fgets(line, MAXLINE, fpsrc) != NULL){
				st_inc_linec();

				last = next_word(line, word, MAXWORD);
				if(strcmp(word, "macro") == 0){ /*new macro declaration*/
					last = next_word(last, word, MAXWORD);	/*get supposed macro name into word*/
					if(!parse_endl(last)){	/*ensuring there is no text after macro definition*/
						fmterr("macro definition ends with unexpected text: %s", last);
						continue;
					}

					contents = read_mcontents(fpsrc, "endmacro");	/*reading contents until the stop-word*/
					proc_new_macro(word, contents);
				}

				else if((contents = find_macro(word)) != NULL) /*there is an existing macro*/
						write_mcontents(fpexp, contents); /*write macro contents to the target file*/

				else
					if(fputs(line, fpexp) == EOF)
						throw(WRT_FILE_ERR);
		}
}

/*CHECKING AND PROCESSING MACRO DEFINITIONS*/

/*proc_new_macro: process new macro deifinition with given name and contents.
if the definition is correct, it will be added to the table, otherwise an error will be displayed*/
static void proc_new_macro(char *name, char **contents){
	int name_len = strlen(name);

	if(is_keyword(name)){	/*ensure the name isn't a keyword*/
		fmterr("preserved word %s can't be used as macro name", name);
		return;
	}

	if(name_len == 0 || parse_name(name) != name_len){	/*ensure the name is correct (token length equals the name length)*/
		fmterr("invalid macro name %s", name);
		return;
	}

	if(!contents){	/*ensure that valid contents object passed*/
		fmterr("invalid contents in %s macro definition", name);
		return;
	}

	add_macro(name, contents);
}


