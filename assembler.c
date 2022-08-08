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

/*main: calls assembler modules to compile the given vector of arguments*/
int main(int argc, char **argv){
	int n = argc;		/*saving argc to preserve the order*/
	while(--argc){
		st_newfile(argv[n-argc]);	/*updating file name in error status*/
		preproc(argv[n-argc]);

		if(st_get_errc() != 0)	/*stop assembling the file if errors found*/
			continue;

		parse_file(argv[n-argc]);	/*calling the parser*/

		st_reset_linec();	/*reset error counter*/
		restart_parser();	/*setting parser to the first line*/

		proc_basic();	/*first processing stage*/

		if(st_get_errc() != 0)
			continue;

		shift_datasym_addr(get_ic());	/*shift data symbols by instructions counter*/

		st_reset_linec();	/*reset error counter*/
		restart_parser();
		proc_fill();	/*second processing stage*/

		if(st_get_errc() != 0)
			continue;

		free_parser();

		/*writing output files*/
		write_code_to(argv[n-argc]);
	 	write_ent_file(argv[n-argc]);
		write_ext_file(argv[n-argc]);

		/*reset code arrays and symbol table*/
		encode_reset();
		reset_symtable();
	}
	return 0;
}

