/*Performs basic processing of the asm file: encoding data-storing directives, saving symbols and encoding operation words*/

#include <stdlib.h>

#include "io.h"
#include "status.h"
#include "code_defs.h"
#include "symbol.h"
#include "proc_p.h"

static int is_correct_label(TokList lst);
static int is_mem_direc(TokList lst);

static void proc_oper_word(TokList lst);
static TokList proc_extern(TokList lst);

static TokList catch_intlit(TokList lst);
static TokList catch_strlit(TokList lst);

/*proc_basic: iterate through the parsed file and perform basic processing of each line*/
void proc_basic(){
	char *label;
  	int symdef;	/*indicates symbol definition*/
	TokList tn_ptr;

	set_ic(0); set_dc(0);	/*reset data and code counters*/
	while((tn_ptr = next_line_parsed()) != NULL){
		st_inc_linec(); /*increase line counter in err status*/
		symdef = FALSE;

		tn_ptr = skip_blank_tok(tn_ptr);	/*skipping spaces*/

		if(is_endl(tn_ptr))	/*skipping blank lines*/
			continue;

		/*if the sentence starts with a label definition*/
		if(is_label(tn_ptr)){
			label = startof_tl(tn_ptr);
			if(!is_correct_label(tn_ptr))
				continue;

			symdef = TRUE;
			tn_ptr = tn_ptr->next->next; /*skipping the name and the colon tokens*/
		}

		tn_ptr = skip_blank_tok(tn_ptr);

		/*checking to exclude the option. entry directives are processed in proc_fill_names()*/
		if(is_direc(tn_ptr, ENTRY_DIR))
			continue;

		/*if the sentence starts with an extern directive*/
		else if(is_direc(tn_ptr, EXTERN_DIR)){
			proc_direc(tn_ptr);
		}

		/*if the list starts with a memory-allocating token*/
		else if(is_mem_direc(tn_ptr)){
			if(symdef)
				add_symbol(label, get_dc(), RELOC, DATA_SEC);	/*adding symbol with DATA_SEC mark to the table*/
			proc_direc(tn_ptr); /*skip the dot and process the directive*/
		}

		/*if the sentence starts with operation token*/
		else if(is_oper(tn_ptr)){
			if(symdef)
				add_symbol(label, get_ic(), RELOC, CODE_SEC);	/*adding symbol with CODE_SEC mark to the table*/
			proc_oper_word(tn_ptr);
		}

		else
			tokerr(&tn_ptr->tok, "invalid entry token, neither a directive nor label nor operation");
	}
}

static int is_correct_label(TokList lst){
	char *name = startof_tl(lst);
	if(find_symbol(name)){	/*symbol already exists*/
		fmterr("redefenition of symbol %s", name);
		return FALSE;
	 }

	 if(is_keyword(name)){
	 	fmterr("preserved word %s can't be used for label name", name);
	 	return FALSE;
	 }
	 return TRUE;
}

/*proc_oper_word: processes the first word in operation sentence*/
static void proc_oper_word(TokList lst){
	const Oper *op = &oplist[valof_tl(lst)];	/*getting the corresponding operation*/
	unsigned int src_msk = op->src, dest_msk = op->dest, wcode;	/*addressing modes masks and operword code*/
	AddrCode src = NO_ADDR, dest = NO_ADDR;	/*target addressing modes*/
	TokList oper_candidate;	/*to preserve the operand start position in list*/

	lst = lst->next;	/*skip the instruction token*/

	if(op->args == 2){	/*if two operands - source operand exists*/
		oper_candidate = lst = skip_blank_tok(lst);
		/*extracting and saving the addressing mode*/
		lst = extract_oper_addr(lst, &src);
		if(!lst || !CORRECT_ADDR(src_msk, src)){	/*checking the addressing mode compatibility using the AM mask*/
			tokerr(&oper_candidate->tok, "incorrect operand in %s operation", op->name);
			return;
		}

		/*ensuring there is a comma before the second operand*/
		if((lst = catch_comma(lst)) == NULL)
			return;
		lst = lst->next; /*skip comma token*/
	}

	if(op->args >= 1){
		oper_candidate = lst = skip_blank_tok(lst);
		lst = extract_oper_addr(lst, &dest);
		if(!lst || !CORRECT_ADDR(dest_msk, dest)){
			tokerr(&oper_candidate->tok, "incorrect operand in %s operation", op->name);
			return;
		}
	}

	/*constructing and encoding the opword*/
	wcode = OPERWORD(op->code, (src >= 0) ? src : 0, (dest >= 0) ? dest : 0, 0);
	encode_code(wcode);

	inc_ic(calc_arg_len(src, dest));	/*advancing the instruction counter by code length*/

	/*ensuring an endline follows the instruction*/
	lst = skip_blank_tok(lst);
	if(!is_endl(lst))
		tokerr(&lst->tok, "instruction ends with unexpected token");
}

/*extract_oper_addr: saves the addressing mode of operand stored in lst and returns the token following the operand. on failure returns null pointer*/
TokList extract_oper_addr(TokList lst, AddrCode *ac){
	if(is_endl(lst)) return NULL;

	/*direct mode (1) */
	if(typeof_tl(lst) == NAME_T && !is_punc(lst->next, '.')){
		*ac = DIRECT_ADDR;
		return lst->next;
	}

	/*register mode (4) */
	if(typeof_tl(lst) == REG_T){
		*ac = REG_ADDR;
		return lst->next;
	}

	/*instant mode -plain number (1) */
	if(!is_endl(lst->next) && is_punc(lst, '#') && typeof_tl(lst->next) == INTLIT_T){
		*ac = INST_ADDR;
		return lst->next->next;
	}

	/*struct mode (3) */
	if(!is_endl(lst->next->next) && typeof_tl(lst) == NAME_T && is_punc(lst->next, '.') && typeof_tl(lst->next->next) == INTLIT_T){
		*ac = STRUCT_ADDR;
		return lst->next->next->next;
	}
	return NULL;
}

/*is_label: returns true iff a given token list starts with a label (symbol definition) */
int is_label(TokList lst){
	return typeof_tl(lst) == NAME_T && is_punc(lst->next, ':');
}

/*is_direc: returns true iff a given token list starts with a directive */
int is_direc(TokList lst, DirCode code){
	if(!lst || !lst->next) return FALSE;
	/*true if a dot token is followed by memory allocating directive token*/
	return is_punc(lst, '.') && typeof_tl(lst->next) == DIR_T && valof_tl(lst->next) == code;
}

/*is_oper: returns true iff a given token list starts with an operation*/
int is_oper(TokList lst){
	if(!lst || !lst->next) return FALSE;
	return typeof_tl(lst) == OPER_T;
}

/*is_mem_direc: returns directive code iff a given token list starts with a memory-allocating directive, otherwise -1*/
static int is_mem_direc(TokList lst){
	return is_direc(lst, DATA_DIR) || is_direc(lst, STRING_DIR) || is_direc(lst, STRUCT_DIR);
}

/*proc_extern: processes extern directive*/
static TokList proc_extern(TokList lst){
	char *label;
	lst = skip_blank_tok(lst);
	label = startof_tl(lst);

	if(find_symbol(label)){	/*symbol already exists*/
		fmterr("redefenition of symbol %s", label);
		return NULL;
	}
	add_symbol(label, 0, EXT, EXT_SEC);	/*saving the symbol and marking it as external*/
	return lst->next;
}

static TokList proc_data(TokList lst);
static TokList proc_string(TokList lst);
static TokList proc_struct(TokList lst);

/*proc_mem_direc: processes directive*/
void proc_direc(TokList lst){
	TokList proc_entry(TokList lst); /*entry directive processor, operates in the second stage*/
	static TokList (*dir_proc_arr[5])(TokList) = {proc_data, proc_string, proc_struct, proc_entry, proc_extern};	/*array of processing functions*/

	lst = lst->next; /*skip the dot token*/

	if(is_endl(lst->next)){	/*if no arguments*/
		fmterr("directive with no actual data");
		return;
	}

	lst = dir_proc_arr[valof_tl(lst)](lst->next);

	if(lst && typeof_tl(lst) != ENDL_T)	/*if extraneous text after the directive*/
		tokerr(&lst->tok, "directive ends with unexpected token");
}

/*proc_data: processes .data directive and encodes the data*/
static TokList proc_data(TokList lst){
	do{
		/*ensuring there is an integer value and encoding it to the data section*/
		if((lst = catch_intlit(lst)) == NULL) return NULL;
		encode_data(valof_tl(lst));

		lst = lst->next;
		if(is_endl(lst->next)) break;

		/*ensuring there is a comma between the arguments*/
		if((lst = catch_comma(lst)) == NULL) return NULL;
		lst = lst->next; /*skip comma token*/

	} while(!is_endl(lst));

	return lst; /*return the next token*/
}

/*proc_string: processes .string directive and encodes the data*/
static TokList proc_string(TokList lst){
	if((lst = catch_strlit(lst)) == NULL) return NULL;

	encode_dataseq((char *)startof_tl(lst) + 1, lenof_tl(lst) - 2);	/*extract the string from the token and encode it*/

	encode_data('\0');
	return lst->next;	/*return the next token*/
}

/*proc_struct: processes ..struct directive and encodes the data*/
static TokList proc_struct(TokList lst){

	/*ensuring there is an integer value and encoding it to the data section*/
	if((lst = catch_intlit(lst)) == NULL) return NULL;
	encode_data(lst->tok.val);

	lst = lst->next;

	/*ensuring there is a comma between the arguments*/
	if((lst = catch_comma(lst)) == NULL) return NULL;
	lst = lst->next; /*skip comma token*/

	lst = proc_string(lst);	/*process the remaining string*/

	return lst;	/*return the next token*/
}

/*skip_blank_tok: skips all blank tokens in tn_ptr and return the next one*/
TokList skip_blank_tok(TokList tn_ptr){
	while(tn_ptr && (tn_ptr)->tok.type == BLANK_T) tn_ptr = (tn_ptr)->next; /*if can be used instead of while*/
	return tn_ptr;
}

/*each one of the catchers below ensure that the next token in lst is of the required types, and produces an error on failure*/

/*catch_comma: ensure comma*/
TokList catch_comma(TokList lst){
	lst = skip_blank_tok(lst);
	if(!lst || !is_punc(lst, ',')){
			tokerr(&lst->tok, "expected comma");
			return NULL;
	}
	return lst;
}

/*catch_intlit: ensure integer literal*/
static TokList catch_intlit(TokList lst){
	lst = skip_blank_tok(lst);
	if(!lst || typeof_tl(lst) != INTLIT_T){
		tokerr(&lst->tok, "expected integer literal");
		return NULL;
	}
	return lst;
}

/*catch_strlit: ensure string literal*/
static TokList catch_strlit(TokList lst){
	lst = skip_blank_tok(lst);
	if(!lst || typeof_tl(lst) != STRLIT_T){
		tokerr(&lst->tok, "expected string literal");
		return NULL;
	}
	return lst;
}

