/*Contains functions to parse assembly source code into tokens*/
/*Provides distinct parsers to catch specific tokens as well as interface to parse entire files*/

#include <stdlib.h>
#include "io.h"
#include "status.h"
#include "code_defs.h"
#include "encode.h"
#include "parse.h"

#define ALLOC_LN 10

enum {FALSE, TRUE};

static void free_toklist();
static void parse_line(TokList *toklist, char *line);

/*parse_token: identifies and returns the next token from line*/
Token parse_token(char *line){
	Token tok;
	int	val;	/*token value, depends on the token type, see the specification*/
	int len;	/*token string length*/

	if((len = parse_blank(line))){
		tok.type = BLANK_T;
	}

	else if(parse_endl(line)){
		tok.type = ENDL_T;
		len = 0;
	}

	else if((val = parse_punc(line))){
		tok.type = PUNC_T;
		tok.val = val;
		len = 1;
	}

	else if((val = parse_reg(line)) >= 0){
		tok.type = REG_T;
		tok.val = val;
		len = 2;
	}

	else if((val = parse_oper(line)) >= 0){
		tok.type = OPER_T;
		tok.val = val;
		len = strlen(oplist[val].name);
	}

	else if((val = parse_direc(line)) >= 0){
		tok.type = DIR_T;
		tok.val = val;
		len = strlen(dirlist[val].name);
	}

	else if((len = parse_name(line))){
		tok.type = NAME_T;
	}

	else if((len = parse_strlit(line)) >= 0){
		tok.type = STRLIT_T;
	}

	else if((len = parse_intlit(line, &val)) > 0){
		tok.type = INTLIT_T;
		tok.val = val;
	}

	else{
		tok.type = INVALID_T;
		len = 1;
	}

	/*saving the token string length*/
	tok.len = len;

	/*saving the string itself*/
	tok.start = malloc(len + 1);
	strncpy(tok.start, line, len);
	tok.start[len] = '\0';

	return tok;
}

/*each function below returns the a value dependent on the token type if it manages to parse it*/

/*parse_blank: parses blank token, returns the length on success, otherwise 0*/
int parse_blank(char *line){
	int len;
	for(len = 0; isspace(*line); len++, line++)
		if(*line == '\n')
			return FALSE;
	return len;
}

/*parse_endl: parses endline token, returns the length on success, otherwise 0*/
int parse_endl(char *line){
	int len;
	if(*line == ';') return TRUE;
	for(len = 0; isspace(*line); len++, line++)
		if(*line == '\n')
			return TRUE;

	/*non-white characters follow*/
	if(*line == '\0') return TRUE;
	return FALSE;
}

/*parse_punc: parses punctuation token, returns the character casted to int on success, otherwise 0 */
int parse_punc(char *line){
	return (*line == ',' || *line == ':' || *line == '.' || *line== '#') ? *line : '\0';
}

/*parse_name: parses name token, returns the length on success, otherwise 0*/
int parse_name(char *line){
	int len;

	if(!line || !isalpha(*line)) return 0;	/*first character should be a letter*/
	for(len = 1, line++; isalnum(*line); line++, len++);	/*name should contain only AN characters*/

	return len;
}

/*parse_strlit: parses string literal token, returns the length on success, otherwise -1*/
int parse_strlit(char *line){
	int len;
	if(*line++ != '\"') return -1;

	for(len = 1; *line; line++, len++)
		if(*line == '\"') break;

	if(*line != '\"') return -1;
	return len + 1; /*count the closing quote*/
}

/*parse_intlit: parses integer literal token, on success returns the length and saves the value in res, otherwise returns 0*/
int parse_intlit(char *line, int *res){
	char *ptr;

	/*to assure that the number starts from the given place and no white spaces skipped*/
	if(!isdigit(*line) && *line != '+' && *line != '-')
		return 0;

	if((*line == '-' || *line == '+') && !isdigit(line[1]))
		return 0;

	*res = strtol(line, &ptr, 10);
	return ptr - line; /*returning length of the integer literal*/
}

/*parse_oper: parses operation token, on success returns the opcode, otherwise 0*/
int parse_oper(char *line){
	int i;
	char word[MAXWORD + 1];
	get_word(line, word, MAXWORD);

	/*looking for the operation in instructions table*/
	for(i = 0; i < N_OP; i++)
		if(strcmp(oplist[i].name, word) == 0)
			return oplist[i].code;
	return -1;
}

/*parse_direc: parses directive token, on success returns the dircode, otherwise 0*/
int parse_direc(char *line){
	int i;
	char word[MAXWORD + 1];
	get_word(line, word, MAXWORD);

	/*looking for the directive in directives table*/
	for(i = 0; i < N_DIR; i++)
		if(strcmp(dirlist[i].name, word) == 0)
			return dirlist[i].code;
	return -1;
}

/*parse_reg: parses register token, on success returns the register number (0-8), otherwise -1*/
int parse_reg(char *line){
	return (line[0] == 'r' && '0' <= line[1] && line[1] < '8' && !isalnum(line[2])) ? line[1] - '0' : -1;
}

/*PARSING ENTIRE FILES*/

/*dynamic array of parsed lines each of which represented by a list of tokens and a number of allocated lines*/
static TokList *parsed_file;
static int alloc_linec;

/*pointer to the line to be returned by next_line_parsed()*/
static TokList *cur_line;

static void exp_parsed_storage();
static TokList new_tnode(Token *tok_p);

/*next_line_parsed: returns next line from the parsed file storage*/
TokList next_line_parsed(){
	return *cur_line++;
}

/*parses the given file line by line and stores the results in the parsed file storage*/
void parse_file(char *fname){
	char line[MAXLINE + 1];
	int linec = 0;
	FILE *fpsrc;

	clear_parser();

	/*opening the target file*/
	if((fpsrc = fopen_ext_mode(fname, ".am", "r")) == NULL)
		throw(OPN_FILE_ERR);

	exp_parsed_storage();	/*initial expansion of storage*/
	while(fgets(line, MAXLINE, fpsrc) != NULL){
		if(linec + 2 > alloc_linec)	/*no enough memory, should be able to contain 2 extra lines (the line parsed and NULL terminator)*/
			exp_parsed_storage();

		/*parsing the next line and saving it in the storage*/
		parsed_file[linec] = NULL;
		parse_line(&parsed_file[linec++], line);
	}
	parsed_file[linec] = NULL;
	fclose(fpsrc);
}

/*parse_line: parses a line of consequent tokens and saves them to toklist*/
static void parse_line(TokList *toklist, char *line){
	Token tok;
	TokList newT, last;	/*holding the last parsed token to add new ones faster*/
	*toklist = NULL;

	do{
		tok = parse_token(line);
		newT = new_tnode(&tok);

		if(*toklist == NULL)	/*it's the first token parsed*/
			last = *toklist = newT;

		else{
			last->next = newT;
			last = last->next;
		}

		line += tok.len; /*skipping the token in line*/
	} while(tok.type != INVALID_T && tok.type != ENDL_T);	/*going until the line ends or an error occurs in it*/
}

/*new_tnode: creates new token node and returns a pointer to it*/
static TokList new_tnode(Token *tok_p){
	TokList newT = malloc(sizeof(TokNode));
	if(!newT) throw(MEM_ALLOC_ERR);
	memcpy(&newT->tok, tok_p, sizeof(Token));
	newT->next = NULL;
	return newT;
}

/*free_toklist: frees memory occupied by toklist*/
static void free_toklist(TokList toklist){
		TokList prev, p;
		for(prev = p = toklist; p ; prev = p){
				p = prev->next;
				free(prev->tok.start);
				free(prev);
		}
}

/*restart_parser: sets cur_line to point to the first line in storage (to iterate over the same file again)*/
void restart_parser(){
	cur_line = parsed_file;
}

/*exp_parsed_storage: expand the parsed file storage by ALLOC_LN lines*/
static void exp_parsed_storage(){
	parsed_file = realloc(parsed_file, (alloc_linec + ALLOC_LN) * sizeof(TokList));
	if(parsed_file == NULL)
		throw(MEM_ALLOC_ERR);
	alloc_linec += ALLOC_LN;
}

/*clear_parser: free the parsed file storage and reset allocated lines counter*/
void clear_parser(){
	TokList *p = parsed_file;
	alloc_linec = 0;

	if(!parsed_file) /*nothing to free*/
		return;

	while(*p != NULL){	/*while there are parsed lines*/
		free_toklist(*p++);
	}

	free(parsed_file);
	parsed_file = NULL;
}
