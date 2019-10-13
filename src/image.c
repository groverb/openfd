#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "image.h"

#include "fd_types.h"
#include "qdbmp.h"


#define DBG 0 
#define WRITE_BMP 1

static pixel pixel_max(pixel pa, pixel pb){
	pixel ret;
	ret.r = pa.r > pb.r ? pa.r : pb.r;
	ret.g = pa.g > pb.g ? pa.g : pb.g;
	ret.b = pa.b > pb.b ? pa.b : pb.b;
	return ret;
}


static pixel pixel_min(pixel pa, pixel pb){
	pixel ret;
	ret.r = pa.r < pb.r ? pa.r : pb.r;
	ret.g = pa.g < pb.g ? pa.g : pb.g;
	ret.b = pa.b < pb.b ? pa.b : pb.b;
	return ret;
}



static pixel pixel_avg(pixel pa, pixel pb){
	pixel ret; 
	ret.r = (uint8_t)((pa.r + pb.r)/2.f);
	ret.g = (uint8_t)((pa.g + pb.g)/2.f);
	ret.b = (uint8_t)((pa.b + pb.b)/2.f);
	return ret;
}

static pixel pixel_rand(pixel* a, pixel* b){
	return (rand() & 1) ? *a : *b;
}

image* make_image(__int2 imagedims){
	image* ret = malloc(sizeof(image));
	ret->data = malloc( imagedims.x * imagedims.y * sizeof(pixel*));
	for(int i=0;i< imagedims.x * imagedims.y; i++){
		ret->data[i] = malloc(sizeof(pixel));
	}

	memcpy(&ret->dims, &imagedims, sizeof(ret->dims));
	return ret;
}

image* imagecp(image* ctx){
	image* ret = make_image(ctx->dims);
	memcpy(ret->data, ctx->data, sizeof(pixel) * ctx->dims.x * ctx->dims.y);
	return ret;
}

void imagewrite(image* ctx, const char* fname){
#ifdef WRITE_BMP
	BMP* outbmp = BMP_Create(ctx->dims.x, ctx->dims.y, 24);
	free(outbmp->Data);
	uint8_t* buff = imgtob(ctx);
	outbmp->Data = buff;

	BMP_WriteFile(outbmp, fname);
//	BMP_Free(outbmp);
#endif

}
image* btoimg(uint8_t* buff, __int2 imagedims){
	image* ret = make_image(imagedims);
	if(ret != NULL){
		printf("not null\n");
		size_t imgsz = imagedims.x * imagedims.y;
		size_t buffsz = imgsz * 3;

		for(int i=0, j=0; j< buffsz; i++, j+=3){
			// printf("i: %d, j: %d\n", i, j);

			ret->data[i]->r = buff[j];
			ret->data[i]->g = buff[j+1];
			ret->data[i]->b = buff[j+2];

		}
		return ret;
	}
}

void free_image(image* ctx){
	if(ctx != NULL){
		size_t imgsz = ctx->dims.x * ctx->dims.y;
		for(int i =0;i<imgsz;i++){
			free(ctx->data[i]);
		}
		free(ctx->data);
		free(ctx);
	}
}

uint8_t* imgtob(image* ctx){
	size_t imgsz = ctx->dims.x * ctx->dims.y;
	size_t buffsz = imgsz * 3;
	uint8_t* ret = malloc(sizeof(uint8_t) * buffsz);
	if(ret != NULL){
		for(int i =0,j=0;j< buffsz; i++, j+=3){
			pixel* temp = ctx->data[i]; 
			ret[j] = temp->r;
			ret[j+1] = temp->g;
			ret[j+2] = temp->b;
		}
	}
	return ret;
}

image* image_resample(image* ctx, __int2 outdims, pixel (*f)(pixel, pixel)){
	image* ret = make_image(outdims);

	if(ret != NULL){
		for(int i =0;i<outdims.y;i++){
			for(int j=0;j<outdims.x;j++){
				int pt = (ctx->dims.x * 2) * i + (j * 2);

				pixel cmp =  f(*(ctx->data[pt]),
						f( *(ctx->data[pt + 1]), f(*(ctx->data[pt + ctx->dims.x]), *(ctx->data[pt + ctx->dims.x + 1]))));

				memcpy(ret->data[i * outdims.x + j], &cmp, sizeof(pixel));

			}
		}
	}
	return ret;
}


#if DBG

int main(int argc, char** argv){

	if(argc < 2 ) { printf("pass args\n"); exit(-1); }
	printf("1\n");
	printf("s of pixel: %ld\n", sizeof(pixel));

	BMP* rd = BMP_ReadFile(argv[1]);
	__int2 indims = {.x = BMP_GetWidth(rd), .y = BMP_GetHeight(rd)};
	// __int2 indims = {.x = BMP_GetHeight(rd), .y = BMP_GetWidth(rd)};

	printf("loaded image size: %d %d\n", indims.x, indims.y);

	printf("2\n");
	image* img1 = btoimg(rd->Data, indims);

	printf("2.5\n");
	__int2 newdims = {indims.x/2, indims.y/2};

	image* newimage = image_resample(img1, newdims, &pixel_max);
	printf("2.75\n");
	uint8_t* buff1 = imgtob(newimage);

	printf("3\n");
	BMP* wt = BMP_Create(newdims.x, newdims.y, 24);
	free(wt->Data);
	wt->Data = buff1;

	printf("4\n");
	BMP_WriteFile(wt, argv[2]);
	//free(buff1);

	BMP_Free(rd);
	BMP_Free(wt);

	free_image(img1);
	free_image(newimage);



	printf("5\n");
	return 0;

}
#endif
