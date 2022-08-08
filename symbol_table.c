/*Defines and provides interface for the assembler symbol table*/

#include <stdlib.h>
#include "io.h"
#include "symbol.h"
#include "status.h"
#include "encode.h"

/*symbol list implementation*/
typedef struct sNode *sNode_ptr;
static void clear_sNode(sNode_ptr node);
static void write_sym_occur(FILE *fp, Symbol *sym_p);

struct sNode{
	Symbol record;
	struct sNode *next;
};

static sNode_ptr symbol_table = NULL;

/*external and entry symbols counters*/
static int ext_cnt;
static int ent_cnt;

/*add_symbol: adds new label with its value and type to the table iff there is no symbol with the same name.
on success returns a pointer to the new symbol, otherwise null pointer.*/
Symbol *add_symbol(char *name, unsigned val, SymType type, SymSec sec){
	/*creating a new node with given values*/
	Symbol rec;
	sNode_ptr newS = malloc(sizeof(struct sNode));
	if(!newS) throw(MEM_ALLOC_ERR);

	rec.name = malloc(strlen(name) + 1);
	strcpy(rec.name, name);
	rec.val = (sec != EXT_SEC ? ADDR_ENTRY + val : 0);	/*external symbols are saved with value 0*/
	rec.type = type;
	rec.sec = sec;

	/*adding the new node to the table*/
	newS->record = rec;
	newS->next = symbol_table;
	symbol_table = newS;

	/*image of occurrences is created for external labels*/
	if(type == EXT){
		ext_cnt++;
		newS->record.occur = malloc(sizeof(Image));
		clear_image(newS->record.occur);
	}

	return &newS->record;
}

/*find_symbol: looks for a symbol with name in the table, returns macro contents on success, null pointer othwerwise*/
Symbol *find_symbol(char *name){
	sNode_ptr p;
	if(!name) return NULL;
	for(p = symbol_table; p; p = p->next){
		if(strcmp(name, p->record.name) == 0)
		return &p->record;
	}
	return NULL;
}

/*make_entry: mark sym_p as entry symbol*/
void make_entry(Symbol *sym_p){
	sym_p->type = ENT;
	ent_cnt++;
}

/*add_occur: add occurrence of a symbol to the occurrences image. in current implementation works only with external labels*/
void add_occur(Symbol *sym_p, size_t val){
	encode_word(sym_p->occur, val + ADDR_ENTRY);
}

/*shift_datasym_addr: shift values of all data labels by offset*/
void shift_datasym_addr(int offset){
	sNode_ptr p = symbol_table;
	while(p){
		if(p->record.sec == DATA_SEC) /*only data symbols' values are increased*/
			p->record.val += offset;
		p = p->next;
	}
}

/*write_ent_file: write a special file with all entry labels and their values in current file*/
void write_ent_file(char *fname){
	sNode_ptr p;
	FILE *fp;

	if(ent_cnt == 0)	/*if there're no entry labels, no file created*/
		return;

	fp = fopen_ext_mode(fname, ".ent", "w");
	if(fp == NULL) throw(OPN_FILE_ERR);

	for(p = symbol_table; p; p = p->next){
		if(p->record.type != ENT)	/*only entry symbols are logged*/
			continue;

		fprintf(fp, "%s ", p->record.name);	/*write the name*/
		encode_tof(fp, p->record.val);	/*encode the value*/
		fputc('\n', fp);
	}
}

/*write_ext_file: write a special file with all external labels and their occurrences in current file*/
void write_ext_file(char *fname){
	sNode_ptr p;
	FILE *fp;

	if(ent_cnt == 0)	/*if there're no external labels, no file created*/
		return;

	fp = fopen_ext_mode(fname, ".ext", "w");
	if(fp == NULL) throw(OPN_FILE_ERR);

	for(p = symbol_table; p; p = p->next){
		if(p->record.type != EXT)	/*only external labels are logged*/
			continue;

		write_sym_occur(fp, &p->record);	/*writing the occurrences of a symbol to fp*/
	}
}

/*write_sym_occur: write the occurences of a sym_p to fp, in current implementation works only with external labels*/
static void write_sym_occur(FILE *fp, Symbol *sym_p){
	int i;
	for(i = 0; i < sym_p->occur->cnt; i++){
		fprintf(fp, "%s ", sym_p->name);	/*write the name*/
		img_seq_tof(sym_p->occur, fp, i, 1);	/*write one value from the occurences image*/
		fputc('\n', fp);
	}
}

/*reset_symtable: clear the symbol table and free the memory it occupies*/
void reset_symtable(){
	sNode_ptr prev, p;
	for(prev = p = symbol_table; p; prev = p){
			p = prev->next;
			clear_sNode(prev);	/*free the node*/
	}
	symbol_table = NULL;
	ent_cnt = ext_cnt = 0;
}

/*clear_sNode: clear a single symbpl node and free the the memory*/
static void clear_sNode(sNode_ptr node){
	Symbol *sym_p = &node->record;
	free(sym_p->name);
	if(sym_p->type == EXT){
		clear_image(sym_p->occur);
		free(sym_p->occur);
	}
	free(node);
}
