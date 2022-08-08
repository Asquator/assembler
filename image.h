/*Interface to manage dynamic image structs*/

#ifndef IMAGE_HEADER
#define IMAGE_HEADER

#include <stdio.h>

typedef unsigned char byte; /*type that stores a single byte*/

typedef struct Image{
	size_t size; /*max blocks*/
	size_t cnt; /*block counter*/
	byte *data;
} Image;

void clear_image(Image *img);
void write_seq_img(Image *img, void *src, size_t nblk);
void img_seq_tof(Image *img, FILE *fp, size_t blkstart, size_t nblk);

#endif
