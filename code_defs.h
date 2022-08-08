/*This header contains constant codes and structure definitions to form constant tables*/

#ifndef CODE_DEFS_HEADER
#define CODE_DEFS_HEADER

/*construct instruction address mask, a_i's are 0 or 1*/
#define ADDRMASK(a0, a1, a2, a3) (a0 << INST_ADDR |\
				 (a1 << DIRECT_ADDR) |\
				 (a2 << STRUCT_ADDR) |\
				 (a3 << REG_ADDR))

#define NO_OPERAND 0
#define CORRECT_ADDR(mask, addr) (mask & (1 << addr))	/*true if addr matches the address mask*/

/*addressing modes constants*/
typedef enum AddrCode{
	NO_ADDR = -1,
	INST_ADDR = 0,	/*Instant (immediate) addressing (plain number)*/
	DIRECT_ADDR = 1,	/*Direct addressing (symbols)*/
	STRUCT_ADDR = 2,	/*Struct addressing*/
	REG_ADDR = 3	/*Register addressing*/
} AddrCode;

typedef enum OpCode{
	MOV = 0,
	CMP, ADD, SUB,
	NOT, CLR, LEA,
	INC, DEC, JMP,
	BNE, GET, PRN,
	JSR, RTS, HLT} OpCode;

typedef enum DirCode{
	DATA_DIR = 0, STRING_DIR, STRUCT_DIR, ENTRY_DIR, EXTERN_DIR
} DirCode;

/*Oper: represents an assembly operation encoded into the operations section*/
typedef struct Oper{
	char *name;	/*instruction name*/
	OpCode code;	/*instruction code*/
	unsigned int args : 2;	/*number of args*/
	unsigned int src : 4;	/*source arg addressing mode*/
	unsigned int dest : 4;	/*destination addressing mode*/
} Oper;

/*Direc: represents a directive encoded into the data section*/
typedef struct Direc{
	char *name;
	DirCode code;
} Direc;

#endif

#ifndef CODE_DEFS

/*importing the constant tables*/
extern const Oper oplist[];
extern const Direc dirlist[];
extern const char *other_keywords[];

/*operations and directives counters*/
extern const size_t N_OP;
extern const size_t N_DIR;
extern const size_t N_OKEY;	/*other keywords array*/
#endif
