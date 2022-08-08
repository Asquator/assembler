/*Performs second processing of the asm file: encodes operands in instructions and treats entry directives*/

#include <stdlib.h>

#include "io.h"
#include "status.h"
#include "code_defs.h"
#include "symbol.h"
#include "proc_p.h"

/*macros to treat register addressing mode*/
#define proc_reg_addr(lst, n) encode_code(REG ## n ## _CODE(valof_tl(lst)))
#define proc_dbreg_addr(lst1, lst2) encode_code(REG1_CODE(valof_tl(lst1)) | REG2_CODE(valof_tl(lst2)))

static void fill_oper_args(TokList lst);

static TokList proc_inst_addr(TokList lst);
static TokList proc_direct_addr(TokList lst);
static TokList proc_struct_addr(TokList lst);

/*proc_fill: iterate through the parsed file and perform second processing stage on each line*/
void proc_fill(){
	TokList tn_ptr;
	set_ic(0);

	while((tn_ptr = next_line_parsed()) != NULL){
		st_inc_linec(); /*increase line counter in err status*/

		tn_ptr = skip_blank_tok(tn_ptr);

		if(is_label(tn_ptr)){
			tn_ptr = tn_ptr->next->next; /*skipping the name and the colon tokens*/
			tn_ptr = skip_blank_tok(tn_ptr);
		}

		if(is_direc(tn_ptr, ENTRY_DIR)){
			proc_direc(tn_ptr);
		}

		else if(is_oper(tn_ptr)){
			fill_oper_args(tn_ptr->next);
		}
	}
}

/*fill_oper_args: encode istruction operands according to their addressing modes*/
static void fill_oper_args(TokList lst){
	const Oper *op = &oplist[valof_tl(lst)];

	/*array of processing functions for 0-2 addressing modes*/
	static TokList (*proc_arr[4]) (TokList) = {proc_inst_addr, proc_direct_addr, proc_struct_addr};
	TokList src_arg, dest_arg;	/*addressing modes*/
	AddrCode src = NO_ADDR, dest = NO_ADDR;	/*target addressing modes*/
	inc_ic(1);	/*skip the opword which was encoded in the first processing stage*/

	/*get addressing modes according to the number of operands*/
	if(op->args == 2){
		lst = src_arg = skip_blank_tok(lst);
		lst = extract_oper_addr(lst, &src);
		lst = catch_comma(lst)->next;
	}

	if(op->args >= 1){
		lst = dest_arg = skip_blank_tok(lst);
		lst = extract_oper_addr(lst, &dest);
	}

	/*if two of the operands are registers*/
	if(src == dest && dest == REG_ADDR){
		proc_dbreg_addr(src_arg, dest_arg);
		return;
	}

	/*process the source operand if it exists*/
	if(op->args == 2){
		if(src != REG_ADDR)	/*special treatment for register mode*/
			proc_arr[src](src_arg);
		else
			proc_reg_addr(src_arg, 1);
	}

	/*process the destination operand if it exists*/
	if(op->args >= 1){
		if (dest != REG_ADDR)
			proc_arr[dest](dest_arg);
		else
			proc_reg_addr(dest_arg, 2);
	}
}

/*proc_entry: process entry directive and export the symbol as entry, returns the next token*/
TokList proc_entry(TokList lst){
	Symbol *sym_p;
	char *label;
	lst = skip_blank_tok(lst); /*skip the directive defining tokens and white spaces, advancing to symbol name*/

	label = startof_tl(lst);
	sym_p = find_symbol(label); /*looking for the symbol name*/

	if(sym_p == NULL){
		fmterr("entry directive with nonexistent name \"%s\"", label);
		return NULL;
	}
	if(sym_p->type == EXT){
		fmterr("external symbol \"%s\" cannot be marked as entry", label);
	}

	make_entry(sym_p); /*marking the symbol as entry*/
	return lst->next;
}

/*each of functions below processes and encodes a corresponding addressing mode. register mode is treated with a macro.
return value is the next token after the operand*/

/*proc_inst_addr: process instant addressing*/
static TokList proc_inst_addr(TokList lst){
	int wcode = valof_tl(lst->next);	/*code is just the value of the token*/
	encode_code(ADD_ARE(wcode, A));	/*adding A-R-E and encoding*/
	return lst->next->next;
}

/*proc_direct_addr: process direct addressing*/
static TokList proc_direct_addr(TokList lst){
	int wcode;
	Symbol *sym_p = find_symbol(startof_tl(lst));	/*looking in symbol table*/
	if(sym_p == NULL){
		tokerr(&lst->tok, "invalid name as operand");
		return NULL;
	}

	wcode = sym_p->val;	/*code is the value of the symbol*/
	if(sym_p->type == EXT)
		add_occur(sym_p, get_ic());	/*saving external symbol occurrence for the linker (or/and ext table display)*/

	encode_code(ADD_ARE(wcode, sym_p->type == EXT ? E : R));	/*adding A-R-E and encoding*/
	return lst->next;
}

/*proc_struct_addr: process struct addressing*/
static TokList proc_struct_addr(TokList lst){
	int mmb;
	lst = proc_direct_addr(lst);	/*lst should point to the dot token of member access expression*/
	if(lst == NULL || lst->next == NULL) return NULL;

	mmb = valof_tl(lst->next);
	if(mmb != 1 && mmb != 2){	/*ensuring the member name is correct*/
		fmterr("expected 1 or 2 as structure member name");
		return NULL;
	}

	encode_code(ADD_ARE(mmb, A));	/*code is the member number, adding A-R-E and encoding*/
	return lst->next->next;
}
