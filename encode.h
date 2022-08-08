/*Interface to write code to arrays and other functions the encoder uses*/

#ifndef ENCODE_HEADER
#define ENCODE_HEADER

#include "code_defs.h"
#include "image.h"

#define OPERWORD(oc, src, dest, are) (((oc) << 6) | ((src) << 4) | ((dest) << 2) | (are))	/*construct operation word*/
#define ADD_ARE(code, are) ((code << 2) | (are))	/*add A-R-E bits*/

/*registers codes*/
#define REG1_CODE(code) (code << 6)
#define REG2_CODE(code) (code << 2)

/*control 2 bits to mark the location of operands*/
enum A_R_E {A = 0, R = 1 << 1, E = 1};

int calc_arg_len(AddrCode addr1, AddrCode addr2);

/*interface to access instruction and data counters*/
int get_ic(void);
int get_dc(void);
void set_ic(size_t val);
void set_dc(size_t val);
void inc_ic(size_t offset);
void inc_dc(size_t offset);

void encode_word(Image *img, int wcode);
void encode_data(int wcode);
void encode_dataseq(char *str, int len);
void encode_code(int wcode);
void encode_tof(FILE *fp, int wcode);

void encode_reset();
#endif
