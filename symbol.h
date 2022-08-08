/*Defines symbol structure and provides interface to symbol table*/

#include "image.h"

/*type of a symbol: relocatable, external or entry (includes the first logically)*/
typedef enum SymType {RELOC, EXT, ENT} SymType;

/*section of a symbol: data, code or external (unknown at compilation time)*/
typedef enum SymSec {DATA_SEC, CODE_SEC, EXT_SEC} SymSec;

typedef struct Symbol{
	char *name;	/*labrl*/
	unsigned val;	/*address value*/
	Image *occur;	/*stores occurrences of the symbol in code, in current implementation used for external symbols only*/
	unsigned type : 2;
	unsigned sec : 2;
} Symbol;


/*interface to manage the symbol table*/
Symbol *add_symbol(char *name, unsigned val, SymType type, SymSec sec);
Symbol *find_symbol(char *name);
void add_occur(Symbol *sym_p, size_t val);
void make_entry(Symbol *sym_p);
void reset_symtable();
