/*Interface to program status and error-handling module*/

#include <stdarg.h>
#include "token.h"

#define ADDR_ENTRY 100
#define MAX_MEM_ADDR 256

void st_newfile(char *fname);
void st_inc_linec();
void st_reset_linec();
int st_get_errc();

/*interface to error-handling module*/
enum err_codes {
	OPN_FILE_ERR = 1,
	MEM_ALLOC_ERR,
	WRT_FILE_ERR
};

void throw(int code);

void fmterr(char *fmt, ...);
void tokerr(Token *tok, char *fmt, ...);
