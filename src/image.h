#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "fd_types.h"
#include "qdbmp.h"


typedef struct _pixel{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} pixel;

typedef struct _image{
	__int2 dims;
	pixel** data;
} image;


image* make_image(__int2 imagedims);

void free_image(image* ctx);

image* imagecp(image* ctx);

void imagewrite(image* ctx, const char* fname);

image* btoimg(uint8_t* buff, __int2 imagedims);

uint8_t* imgtob(image* ctx);

image* image_resample(image* ctx, __int2 outdims, pixel (*f)(pixel, pixel));

#endif

