#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <fd/fd_types.h>

#include "image.h"
#include "qdbmp.h"



#define DBG 0 
#define WRITE_BMP 1

pixel pixel_max(pixel pa, pixel pb){
	pixel ret;
	ret.r = pa.r > pb.r ? pa.r : pb.r;
	ret.g = pa.g > pb.g ? pa.g : pb.g;
	ret.b = pa.b > pb.b ? pa.b : pb.b;
	return ret;
}


pixel pixel_min(pixel pa, pixel pb){
	pixel ret;
	ret.r = pa.r < pb.r ? pa.r : pb.r;
	ret.g = pa.g < pb.g ? pa.g : pb.g;
	ret.b = pa.b < pb.b ? pa.b : pb.b;
	return ret;
}



pixel pixel_avg(pixel pa, pixel pb){
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
	ret->__relative_pos.x = ret->__relative_pos.y = 0;

	return ret;
}

void free_image(image* ctx){

	if(ctx != NULL){
		size_t imgsz = ctx->dims.x * ctx->dims.y;
		for(int i =0;i<(int)imgsz;i++){
			if(ctx->data[i] != NULL){
				free(ctx->data[i]);
			}
		}
		free(ctx->data);
		free(ctx);
	}
}


image* image_cp(image* ctx){
	image* ret = make_image(ctx->dims);

	for(int i=0;i< ctx->dims.x * ctx->dims.y; i++){
		memcpy(ret->data[i], ctx->data[i], sizeof(pixel));;
	}
	return ret;
}

void image_write(image* ctx, const char* fname){
#ifdef WRITE_BMP
	BMP* outbmp = BMP_Create(ctx->dims.x, ctx->dims.y, 24);
	free(outbmp->Data);
	uint8_t* buff = image_to_buffer(ctx);
	outbmp->Data = buff;

	BMP_WriteFile(outbmp, fname);
	//	BMP_Free(outbmp);
#endif
}

image* buffer_to_image(uint8_t* buff, __int2 imagedims){
	image* ret = make_image(imagedims);
	if(ret != NULL){
		size_t imgsz = imagedims.x * imagedims.y;
		size_t buffsz = imgsz * 3;

		for(int i=0, j=0; j<(int)buffsz; i++, j+=3){

			ret->data[i]->r = buff[j];
			ret->data[i]->g = buff[j+1];
			ret->data[i]->b = buff[j+2];

		}
	}
	return ret;
}
uint8_t* image_to_buffer(image* ctx){
	size_t imgsz = ctx->dims.x * ctx->dims.y;
	size_t buffsz = imgsz * 3;
	uint8_t* ret = calloc(sizeof(uint8_t) , buffsz);
	if(ret != NULL){
		for(int i =0,j=0;j<(int) buffsz; i++, j+=3){
			ret[j] = ctx->data[i]->r;
			ret[j+1] = ctx->data[i]->g;
			ret[j+2] = ctx->data[i]->b;
		}
	}
	return ret;
}

image* image_resample(image* ctx, __int2 outdims, pixel (*f)(pixel, pixel)){
	// TODO: user aspect ratio to adjust downsample metrics
	image* ret = make_image(outdims);
	size_t mxpt = ctx->dims.x * ctx->dims.y;
	printf("in here\n");
	if(ret != NULL){
		for(int i =0;i<outdims.x- 4;i++){
			for(int j=0;j< outdims.y -4;j++){
				int pt = (ctx->dims.x * 2) * i + (j * 2);
				printf("i: %d, j: %d, pt: %d, indims: %d,%d, outdims: %d,%d ", i, j, pt, ctx->dims.x, ctx->dims.y, outdims.x, outdims.y);
				printf("%d %d %d %d\n",pt, pt + 1, (pt + ctx->dims.x), (pt + ctx->dims.x + 1 ));

				if((pt + ctx->dims.x + 1 ) > mxpt){
					goto resample_done;
				}
				pixel cmp =  f(*(ctx->data[pt]),
						f( *(ctx->data[pt + 1]), f(*(ctx->data[pt + ctx->dims.x]), *(ctx->data[pt + ctx->dims.x + 1]))));
				
				printf("cmp\n");
				memcpy(ret->data[i * outdims.x + j], &cmp, sizeof(pixel));

			}
		}
	}
resample_done:
	return ret;
}

static inline int _sign(int num){
	return ( (num > 0) ? 1 : ((num < 0) ? -1 : 0) );
}


void image_draw_point(image* ctx, __int2 pt, pixel* clr){
	if(ctx != NULL && clr != NULL){
		ctx->data[pt.x * ctx->dims.x + pt.y ]->r = clr->r;
		ctx->data[pt.x * ctx->dims.x + pt.y ]->g = clr->g;
		ctx->data[pt.x * ctx->dims.x + pt.y ]->b = clr->b;
	}
}


void image_draw_diag(image* ctx, __int2 pt1, __int2 pt2, pixel* clr){
	if(ctx != NULL && clr != NULL){
		int deltax = pt2.x - pt1.x;
		int deltay = pt2.y - pt1.y;

		float deltaerr = abs(deltay / (float) deltax);

		float err = 0.f;
		int j = pt1.x;
		__int2 curpt = {.x = j};;

		for(int i = pt1.y; i<pt2.y; i++){
			curpt.y = i;	
			image_draw_point(ctx, curpt, clr);
			err = err + deltaerr;

			if(err >= 0.5f){
				j += _sign(deltay) * 1;
				curpt.x = j;
				err -= 1.f;
			}
		}
	}
}


void image_draw_line(image* ctx, __int2 pt1, __int2 pt2, pixel* clr){
	int deltax = pt2.x - pt1.x;
	int deltay = pt2.y - pt1.y;
	__int2 curpt; 

	if(deltax == 0){ // horizontal 
		curpt.x = pt1.x ;
		for(int i = pt1.y; i<pt2.y;i++){
			curpt.y = i;
			image_draw_point(ctx, curpt, clr);
		}
	}
	else if(deltay == 0){ // veritcal
		curpt.y = pt1.y ;
		for(int i = pt1.x; i<pt2.x;i++){
			curpt.x = i;
			image_draw_point(ctx, curpt, clr);
		}
	}
	else{
		image_draw_diag(ctx, pt1, pt2, clr);
	}

}




void image_draw_square(image* ctx, __int2 bottomleft, __int2 topright, pixel* clr){
	if(ctx != NULL && clr != NULL && bottomleft.x <= ctx->dims.x && bottomleft.y <= ctx->dims.y && topright.x <= ctx->dims.x && topright.y <= ctx->dims.y){
		__int2 topleft = {topright.x, bottomleft.y};
		__int2 bottomright = {bottomleft.x, topright.y};

		image_draw_line(ctx,  bottomleft, bottomright,clr);
		image_draw_line(ctx, bottomleft, topleft, clr);
		image_draw_line(ctx , topleft, topright,  clr);
		image_draw_line(ctx, bottomright, topright, clr);
	}
}


#if 0 


int main(int argc, char** argv){
	FILE* imgf;
	imgf = fopen(argv[1], "rb");

	uint8_t header[54];
	fread(header, 1, 54, imgf);

	__int2 indims = {.x = atoi(argv[2]) , .y=atoi(argv[3])  };

	size_t buffsz = indims.x * indims.y * 3;

	uint8_t* buff = malloc(sizeof(uint8_t) * buffsz);
	fread(buff, 1, buffsz, imgf);

	image* img = buffer_to_image(buff, indims);
	__int2 pta = {20,20};
	__int2 ptb = {510,510};
	pixel color = {255, 255, 0}; //green

	printf("pta: %d, %d\n", pta.x, pta.y);
	printf("ptb: %d, %d\n", ptb.x, ptb.y);
	image_draw_square(img,pta, ptb, &color);

	image_draw_line(img, pta, ptb, &color);

	image_write(img, "withline.bmp");

	return 0;

}

int main(int argc, char** argv){

	if(argc < 2 ) { printf("pass args\n"); exit(-1); }
	printf("1\n");
	printf("s of pixel: %ld\n", sizeof(pixel));

	BMP* rd = BMP_ReadFile(argv[1]);
	__int2 indims = {.x = BMP_GetWidth(rd), .y = BMP_GetHeight(rd)};
	// __int2 indims = {.x = BMP_GetHeight(rd), .y = BMP_GetWidth(rd)};

	printf("loaded image size: %d %d\n", indims.x, indims.y);

	printf("2\n");
	image* img1 = buffer_to_image(rd->Data, indims);

	printf("2.5\n");
	__int2 newdims = {indims.x/2, indims.y/2};

	image* newimage = image_resample(img1, newdims, &pixel_max);
	printf("2.75\n");
	uint8_t* buff1 = image_to_buffer(newimage);

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
