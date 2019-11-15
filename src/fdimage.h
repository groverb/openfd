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

typedef struct _fdimage{
	__int2 dims;
	pixel** data;
	__int2 __relative_pos; // just in case this fdimage is a section of another fdimage(sliding window)
} fdimage;


fdimage* make_fdimage(__int2 fdimagedims);

void free_fdimage(fdimage* ctx);

fdimage* fdimage_cp(fdimage* ctx);

void fdimage_write(fdimage* ctx, const char* fname);

fdimage* buffer_to_fdimage(uint8_t* buff, __int2 fdimagedims);

uint8_t* fdimage_to_buffer(fdimage* ctx);

fdimage* fdimage_resample(fdimage* ctx, __int2 outdims, pixel (*f)(pixel, pixel));

void fdimage_draw_point(fdimage* ctx, __int2 pt, pixel* clr);

void fdimage_draw_diag(fdimage* ctx, __int2 pt1, __int2 pt2, pixel* clr);

void fdimage_draw_line(fdimage* ctx, __int2 pt1, __int2 pt2, pixel* clr);

void fdimage_draw_square(fdimage* ctx, __int2 bottomleft, __int2 topright, pixel* clr);

float* normalize_buffer(uint8_t* buffer, size_t len);

float* to_dknet_buffer(uint8_t* buffer, size_t len);
float* __to_dknet_buffer(fdimage* ctx);

pixel pixel_max(pixel pa, pixel pb);

pixel pixel_min(pixel pa, pixel pb);

pixel pixel_avg(pixel pa, pixel pb);

#endif

