/*This module expands macros in a given asm file, used by the assembler preprocessor*/

#include "macro_list.h"
#include "status.h"
#include "io.h"

#define ALLOC_LN 4 /*lines to alloc at a time*/

/*macro list definitions*/
struct mNode {
	char *macro;	/*macro name*/
	char **contents;	/*macro contents to expand*/
	struct mNode *next;
};

typedef struct mNode *mNode_ptr;

static mNode_ptr macro_list = NULL;

/*add new macro with its contents to the table, the arguments should be acquired by calling the reader functions*/
void add_macro(char *macro, char **contents){
	/*creating a new node with given values*/
	mNode_ptr newM = malloc(sizeof(struct mNode));
	if(!newM) throw(MEM_ALLOC_ERR);
	newM->macro = malloc(strlen(macro) + 1);
	strcpy(newM->macro, macro);
	newM->contents = contents;
	newM->next = macro_list;
	macro_list = newM;
}

/*find_macro: looks for a macro in the list. if found, returns its contents, otherwise null pointer*/
char **find_macro(char *macro){
		mNode_ptr p;
		for(p = macro_list; p; p = p->next){
				if(strcmp(macro, p->macro) == 0)
				return p->contents;
		}

		return NULL;
}

/*READER FUNCTIONS*/

/*read_mname: reads and saves the next word in line which supposed to be a macro name passed to add_macro*/
char *read_mname(char *line){
	char word[MAXLINE + 1];
	char *name;
	next_word(line, word, MAXLINE);
	name = malloc(strlen(word) + 1);
	if(!name)
		throw(MEM_ALLOC_ERR);

	strcpy(name, word);
	return name;
}

/*read_mcontents: reads and saves the macro contents from fp until the stop-word appears. supposed to be passed to add_macro*/
char **read_mcontents(FILE *fp, char *until){
	char line[MAXLINE + 1];
	char word[MAXWORD + 1];
	char **contents = NULL;
	size_t alloc_b = 0; /*allocated bytes and lines currently allocated*/
	int lines = 0;

	while(fgets(line, MAXLINE, fp) != NULL){
		if(lines % ALLOC_LN == 0){ /*all space used, reallocating memory*/
			alloc_b += ALLOC_LN * sizeof(char *);
			if((contents = realloc(contents, alloc_b)) == NULL) throw(MEM_ALLOC_ERR);
		}

		next_word(line, word, MAXWORD);
		if(strcmp(word, until) == 0){	/*if the stop-word appears*/
			contents[lines] = NULL; /*array of contents ends with null*/
			return contents;
		}

		else {
			/*adding the line to contents array*/
			contents[lines] = malloc(MAXLINE + 1);
			if(contents[lines] == NULL) throw(MEM_ALLOC_ERR);
			strncpy(contents[lines++], line, MAXLINE);
		}
	}
	return NULL;
}


/*write_mcontents: expands macro contents to the given file*/
void write_mcontents(FILE *fp, char **contents){
	while(*contents != NULL)
		if(fputs(*contents++, fp) == EOF)
			throw(WRT_FILE_ERR);
}


static void clear_mNode(mNode_ptr node);

/*clear_maclist: clears the list from previous records and frees the memory*/
void clear_maclist(){
		mNode_ptr prev, p;
		for(prev = p = macro_list; p ; prev = p){
				p = prev->next;
				clear_mNode(prev);
		}
		macro_list = NULL;
}

/*clar_mNode: clears a single macro node and frees the memory*/
static void clear_mNode(mNode_ptr node){
	char **line_ptr = node->contents;
	free(node->macro);
	while(*line_ptr)
		free(*line_ptr++);

	free(node->contents);
	free(node);
}
