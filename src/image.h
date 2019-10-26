#ifndef _IMAGE_H
#define _IMAGE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <fd/fd_types.h>
#include "qdbmp.h"


typedef struct _pixel{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} pixel;

typedef struct _image{
	__int2 dims;
	pixel** data;
	__int2 __relative_pos; // just in case this image is a section of another image(sliding window)
} image;


image* make_image(__int2 imagedims);

void free_image(image* ctx);

image* image_cp(image* ctx);

void image_write(image* ctx, const char* fname);

image* buffer_to_image(uint8_t* buff, __int2 imagedims);

uint8_t* image_to_buffer(image* ctx);

image* image_resample(image* ctx, __int2 outdims, pixel (*f)(pixel, pixel));

void image_draw_point(image* ctx, __int2 pt, pixel* clr);

void image_draw_diag(image* ctx, __int2 pt1, __int2 pt2, pixel* clr);

void image_draw_line(image* ctx, __int2 pt1, __int2 pt2, pixel* clr);

void image_draw_square(image* ctx, __int2 bottomleft, __int2 topright, pixel* clr);


pixel pixel_max(pixel pa, pixel pb);

pixel pixel_min(pixel pa, pixel pb);

pixel pixel_avg(pixel pa, pixel pb);

#endif

