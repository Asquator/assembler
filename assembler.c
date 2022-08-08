/*assembler.c: assembler-like program for the specified architecture*/

#include <stdio.h>
#include "status.h"
#include "encode.h"
#include "parse.h"

void preproc(char *fname);
void write_code_to(char *fname);
void write_ent_file(char *fname);
void write_ext_file(char *fname);

void reset_symtable();
void shift_datasym_addr(int offset);

void proc_basic();
void proc_fill();

static void call_proc();
static void free_all();

/*main: calls assembler modules to compile the given vector of arguments*/
int main(int argc, char **argv){
	int n = argc;		/*saving argc to preserve the order*/
	while(--argc){
		st_newfile(argv[n-argc]);	/*updating file name in error status*/
		preproc(argv[n-argc]);	/*calling the preprocessor*/

		if(st_get_errc() != 0)	/*stop assembling the file if errors found*/
			continue;

		parse_file(argv[n-argc]);	/*calling the parser*/

		call_proc();	/*calling the proc module*/

		/*writing output files*/
		if(st_get_errc() == 0){
			write_code_to(argv[n-argc]);
		 	write_ent_file(argv[n-argc]);
			write_ext_file(argv[n-argc]);
		}
	}
	free_all();
	return 0;
}

/*call_proc: call the main processing module, stages 1 and 2*/
static void call_proc(){
	/*reset code arrays and symbol table*/
	encode_reset();
	reset_symtable();

	restart_parser();	/*setting parser to the first line*/
	proc_basic();	/*first processing stage*/

	if(st_get_errc() != 0)
		return;

	shift_datasym_addr(get_ic());	/*shift data symbols by instructions counter*/

	restart_parser();	/*setting parser to the first line*/
	proc_fill();	/*second processing stage*/
}

/*reset_assembler: free all dynamic memory*/
static void free_all(){
	encode_reset();
	reset_symtable();
	clear_parser();
}

