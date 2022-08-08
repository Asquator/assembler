assembler: preproc.o assembler.o err.o macro_list.o symbol_table.o parse.o image.o proc_basic.o proc_fill.o encode.o io.o code_defs.o
	gcc -Wall -ansi -pedantic -g assembler.o macro_list.o symbol_table.o preproc.o parse.o image.o err.o proc_basic.o proc_fill.o encode.o io.o code_defs.o -o assembler

macro_list.o: macro_list.c macro_list.h
	gcc -Wall -ansi -pedantic -c -g macro_list.c -o macro_list.o

symbol_table.o: symbol_table.c symbol.h
	gcc -Wall -ansi -pedantic -c -g symbol_table.c -o symbol_table.o

preproc.o: preproc.c io.h status.h macro_list.h parse.h
	gcc -Wall -ansi -pedantic -c -g preproc.c -o preproc.o

proc_basic.o: proc_basic.c proc_p.h parse.h io.h status.h symbol.h encode.h
	gcc -Wall -ansi -pedantic -c -g proc_basic.c -o proc_basic.o

proc_fill.o: proc_fill.c proc_p.h io.h status.h symbol.h encode.h
	gcc -Wall -ansi -pedantic -c -g proc_fill.c -o proc_fill.o

assembler.o: assembler.c io.h status.h
	gcc -Wall -ansi -pedantic -c -g assembler.c -o assembler.o

err.o: err.c status.h token.h
	gcc -Wall -ansi -pedantic -c -g err.c -o err.o

parse.o: parse.c parse.h io.h status.h encode.h code_defs.h
	gcc -Wall -ansi -pedantic -c -g parse.c -o parse.o

image.o: image.c image.h status.h
	gcc -Wall -ansi -pedantic -c -g image.c -o image.o

encode.o: encode.c image.h token.h encode.h
	gcc -Wall -ansi -pedantic -c -g encode.c -o encode.o

io.o: io.c io.h status.h
	gcc -Wall -ansi -pedantic -c -g io.c -o io.o

code_defs.o: code_defs.c code_defs.h
	gcc -Wall -ansi -pedantic -c -g code_defs.c -o code_defs.o
