/*Provides interface to encode to both sections in specific Base-32 counting system.
Also contains auxiliary functions related to the encoding process*/

#include <stdlib.h>

#include "io.h"
#include "image.h"
#include "encode.h"
#include "status.h"

enum {FALSE, TRUE};

/*Images for data and code*/
static Image data_img = {0, 0, NULL};
static Image code_img = {0, 0, NULL};

/*encode_reset: clear code and data images*/
void encode_reset(){
	clear_image(&data_img);
	clear_image(&code_img);
}

/*six functions below help access the data and code counters*/

int get_ic(){
	return code_img.cnt;
}

int get_dc(){
	return data_img.cnt;
}

void set_ic(size_t val){
	code_img.cnt = val;
}

void set_dc(size_t val){
	data_img.cnt = val;
}

void inc_ic(size_t offset){
	code_img.cnt += offset;
}

void inc_dc(size_t offset){
	data_img.cnt += offset;
}

/*word_to_32: translates code to the specified Base-32 counting system, the result is stored in code32 which has to be at least 3 bytes*/
static void word_to_32(char *code32, int code){
	static const char base32[] = "!@#$%^&*<>abcdefghijklmnopqrstuv";
	unsigned int ucode = (unsigned int)code;
	ucode &= (1 << 10) - 1; /*take only 10 less significant bits*/

	code32[0] = base32[ucode / 32];
	code32[1] = base32[ucode % 32];
	code32[2] = '\0';
}

/*calc_arg_len: compute the length of some instruction's arguments encoded into processor words based on their addressing modes*/
int calc_arg_len(AddrCode addr1, AddrCode addr2){
	int len = 0;
	if(addr1 != NO_ADDR)
		len += (addr1 == STRUCT_ADDR) ? 2 : 1;
	if(addr2 != NO_ADDR)
		len += (addr2 == STRUCT_ADDR) ? 2 : 1;
	if(addr1 == addr2 && addr2 == REG_ADDR) /*oth operands are registers*/
		len -=1;
	return len;
}

/*encode_word: encode a single word in Base-32 to a given image*/
void encode_word(Image *img, int wcode){
	char code32[3];
	word_to_32(code32, wcode);
	write_seq_img(img, code32, 1);
}

/*encode_data: encode a single word in Base-32 to the data image*/
void encode_data(int wcode){
	encode_word(&data_img, wcode);
}

/*encode_code: encode a single word in Base-32 to the code image*/
void encode_code(int wcode){
	encode_word(&code_img, wcode);
}

/*encode_tof: encode a single word in Base-32 to the given file*/
void encode_tof(FILE *fp, int wcode){
	char code32[3];
	word_to_32(code32, wcode);
	fwrite(code32, 2, 1, fp);
}

/*encode_dataseq: encode a sequence in str of length n to the data image*/
void encode_dataseq(char *str, int len){
	int i;
	for(i = 0; i < len; i++)
		encode_data(str[i]);
}

/*write_code_to: write code and data images successively to file*/
void write_code_to(char *fname){
	FILE *fp = fopen_ext_mode(fname, ".ob", "wb");
	register int addr = ADDR_ENTRY;
	int ic = get_ic(), dc = get_dc();
	char ic_code[3], dc_code[3], addr_code[3];
	int cnt;

	word_to_32(ic_code, ic);
	word_to_32(dc_code, dc);

	fprintf(fp, "%s %s\n", ic_code, dc_code);
	for(cnt = 0; cnt < ic; cnt++, addr++){
		word_to_32(addr_code, addr);
		fprintf(fp, "%s ", addr_code);
		img_seq_tof(&code_img, fp, cnt, 1);
		fputc('\n', fp);
	}

	for(cnt = 0; cnt < dc; cnt++, addr++){
		word_to_32(addr_code, addr);
		fprintf(fp, "%s ", addr_code);
		img_seq_tof(&data_img, fp, cnt, 1);
		fputc('\n', fp);
	}
	fclose(fp);
}
