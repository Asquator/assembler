/*This file handles errors in running the assembler and contains the program status info*/

#include <stdio.h>
#include <stdlib.h>
#include "status.h"

#define prog "assembler"

/*standard error message opening*/
#define err_msg() printf("%s: error: in file: %s line: %d\n", prog, status.fname, status.linec);

/*print fmt formatted error message taking the arguments from args of type va_list*/
#define fmt_err_print(args, fmt) \
	status.errc++; \
	va_start(args, fmt); \
	putchar('\t');	\
	vprintf(fmt, args); \
	putchar('\n'); \

/*program status that contains information about the compilation - filename, lines counter, errors counter*/
static struct prog_status{
	char *fname;
	int linec;
	int errc;
} status;

/*st_newfile: set new filename in program status and reset the status counters*/
void st_newfile(char *fname){
	status.fname = fname;
	status.linec = 0;
	status.errc = 0;
}

/*st_get_errc: get error counter from program status*/
int st_get_errc(){
	return status.errc;
}

/*st_inc_linec: increase line counter in program status*/
void st_inc_linec(){
	status.linec++;
}

/*st_reset_linec: reset line counter in program status*/
void st_reset_linec(){
	status.linec = 0;
}

/*throw: throw an error corresponding to the exit code and immediately stops the program (treating assembler errors only)*/
void throw(int code){
	switch(code){
		case OPN_FILE_ERR:
			fprintf(stderr, "%s: Error while opening file, terminating\n", prog);
			break;

		case WRT_FILE_ERR:
			fprintf(stderr, "%s: Error while writing file, terminating\n", prog);
			break;

		case MEM_ALLOC_ERR:
			fprintf(stderr, "%s: Error while allocating memory, terminating\n", prog);
			break;
	}
	exit(code);
}

/*fmterr: display formatted error to stdout*/
void fmterr(char *fmt, ...){
	va_list args;
	err_msg();
	fmt_err_print(args, fmt);
	va_end(args);

	putchar('\n');
}

/*tokerr: display token error to stdout*/
void tokerr(Token *tok, char *fmt, ...){
	va_list args;
	err_msg();
	printf("\tInvalid token: \'%.*s\'", tok->len, tok->start);
	fmt_err_print(args, fmt);
	va_end(args);

	putchar('\n');
}



