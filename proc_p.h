/*proc_p.h: used exclusively by proc module and contains declarations needed to process a parsed file and make calls to encode the data*/
#include "encode.h"

#define typeof_tl(lst) lst->tok.type
#define valof_tl(lst) lst->tok.val
#define startof_tl(lst) lst->tok.start
#define lenof_tl(lst) lst->tok.len

#define is_punc(lst, c) (lst && typeof_tl(lst) == PUNC_T && valof_tl(lst) == c)
#define is_endl(lst) (!lst || typeof_tl(lst) == ENDL_T)

enum {FALSE, TRUE};

int is_label(TokList lst);
int is_direc(TokList lst, DirCode code);
int is_oper(TokList lst);

void proc_direc(TokList lst);

TokList next_line_parsed();
TokList skip_blank_tok(TokList tn_ptr);
TokList catch_comma(TokList lst);
TokList extract_oper_addr(TokList lst, AddrCode *ac);
