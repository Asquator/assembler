/*This file defines dynamic image arrays and provides tools to maintain them*/
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "status.h"

#define IMG_BLKSIZE 2
#define ALLOC_BLK 5  /*minimum blocks to alloc at once while writing in image arrays*/

static void exp_image(Image *img, size_t nblk);

/*write_seq: writes a sequence of nblk blocks from src
to the given image from current position, expanding it if needed.*/
void write_seq_img(Image *img, void *src, size_t nblk){
	if(img->cnt + nblk > img->size) /*no enough space*/
		exp_image(img, nblk);

	memcpy(img->data + (IMG_BLKSIZE * img->cnt), src, nblk * IMG_BLKSIZE);
	img->cnt += nblk;
}

/*img_seq_tof: writes a sequence of nblk blocks from img to fp starting at blkstart*/
void img_seq_tof(Image *img, FILE *fp, size_t blkstart, size_t nblk){
	int num = fwrite(img->data + (blkstart * IMG_BLKSIZE), IMG_BLKSIZE, nblk, fp);
	if(num != nblk) throw(WRT_FILE_ERR);
}

/*free_image: frees the image array and resets the the counter to 0*/
void free_image(Image *img){
	if(img->data)
		free(img->data);
	reset_image(img);
}

/*reset_image: resets image to the default (empty) */
void reset_image(Image *img){
	img->cnt = 0;
	img->size = 0;
	img->data = NULL;
}

/*exp_image: expand the image storage by nbytes bytes*/
static void exp_image(Image *img, size_t nblk){
	if(nblk < ALLOC_BLK)	/*ensuring minimal number of blocks*/
		nblk = ALLOC_BLK;

	img->data = realloc(img->data, (img->size + nblk) * IMG_BLKSIZE);
	if(img->data == NULL) throw(MEM_ALLOC_ERR);
	img->size += nblk;
}

