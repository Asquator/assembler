/*Contains constant tables and definitions*/

#define CODE_DEFS

#include <stdlib.h>
#include "code_defs.h"

/*instructions table*/
const Oper oplist[] = {
	{"mov", MOV, 2, ADDRMASK(1,1,1,1), ADDRMASK(0,1,1,1)},
	{"cmp", CMP, 2, ADDRMASK(1,1,1,1), ADDRMASK(1,1,1,1)},
	{"add", ADD, 2, ADDRMASK(1,1,1,1), ADDRMASK(0,1,1,1)},
	{"sub", SUB, 2, ADDRMASK(1,1,1,1), ADDRMASK(0,1,1,1)},
	{"not", NOT, 1, NO_OPERAND, ADDRMASK(0,1,1,1)},
	{"clr", CLR, 1, NO_OPERAND, ADDRMASK(0,1,1,1)},
	{"lea", LEA, 2, ADDRMASK(0,1,1,1), ADDRMASK(0,1,1,0)},
	{"inc", INC, 1, NO_OPERAND, ADDRMASK(0,1,1,1)},
	{"dec", DEC, 1, NO_OPERAND, ADDRMASK(0,1,1,1)},
	{"jmp", JMP, 1, NO_OPERAND, ADDRMASK(0,1,1,1)},
	{"bne", BNE, 1, NO_OPERAND, ADDRMASK(0,1,1,1)},
	{"get", GET, 1, NO_OPERAND, ADDRMASK(0,1,1,1)},
	{"prn", PRN, 1, NO_OPERAND, ADDRMASK(1,1,1,1)},
	{"jsr", JSR, 1, NO_OPERAND, ADDRMASK(0,1,1,1)},
	{"rts", RTS, 0, NO_OPERAND, NO_OPERAND},
	{"hlt", HLT, 0, NO_OPERAND, NO_OPERAND}
};

/*directives table*/
const Direc dirlist[] = {
	{"data", DATA_DIR},
	{"string", STRING_DIR},
	{"struct", STRUCT_DIR},
	{"entry", ENTRY_DIR},
	{"extern", EXTERN_DIR}
};

const char *other_keywords[] = {"macro", "endmacro"};

/*lengths of oplist and dirlist arrays*/
const size_t N_OP = (sizeof(oplist) / sizeof(oplist[0]));
const size_t N_DIR = (sizeof(dirlist) / sizeof(dirlist[0]));
const size_t N_OKEY = (sizeof(other_keywords) / sizeof(other_keywords[0]));
