#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <fd/fd_types.h>

#include "fdimage.h"
#include "qdbmp.h"
#include "darknet.h"
#include "config.h"


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


fdimage* make_fdimage(__int2 fdimagedims){
	fdimage* ret = malloc(sizeof(fdimage));
	ret->data = malloc( fdimagedims.x * fdimagedims.y * sizeof(pixel*));
	for(int i=0;i< fdimagedims.x * fdimagedims.y; i++){
		ret->data[i] = malloc(sizeof(pixel));
	}

	memcpy(&ret->dims, &fdimagedims, sizeof(ret->dims));
	ret->__relative_pos.x = ret->__relative_pos.y = 0;

	return ret;
}

void free_fdimage(fdimage* ctx){

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


fdimage* fdimage_cp(fdimage* ctx){
	fdimage* ret = make_fdimage(ctx->dims);

	for(int i=0;i< ctx->dims.x * ctx->dims.y; i++){
		memcpy(ret->data[i], ctx->data[i], sizeof(pixel));;
	}
	return ret;
}

void fdimage_write(fdimage* ctx, const char* fname){
#ifdef WRITE_BMP
	BMP* outbmp = BMP_Create(ctx->dims.x, ctx->dims.y, 24);
	free(outbmp->Data);
	uint8_t* buff = fdimage_to_buffer(ctx);
	outbmp->Data = buff;

	BMP_WriteFile(outbmp, fname);
	//	BMP_Free(outbmp);
#endif
}

fdimage* buffer_to_fdimage(uint8_t* buff, __int2 fdimagedims){
	fdimage* ret = make_fdimage(fdimagedims);
	if(ret != NULL){
		size_t imgsz = fdimagedims.x * fdimagedims.y;
		size_t buffsz = imgsz * 3;

		for(int i=0, j=0; j<(int)buffsz; i++, j+=3){

			ret->data[i]->r = buff[j];
			ret->data[i]->g = buff[j+1];
			ret->data[i]->b = buff[j+2];

		}
	}
	return ret;
}
uint8_t* fdimage_to_buffer(fdimage* ctx){
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

fdimage* fdimage_resample(fdimage* ctx, __int2 outdims, pixel (*f)(pixel, pixel)){
	// TODO: user aspect ratio to adjust downsample metrics
	fdimage* ret = make_fdimage(outdims);
	size_t mxpt = ctx->dims.x * ctx->dims.y;
	if(ret != NULL){
		for(int i =0;i<outdims.x- 4;i++){
			for(int j=0;j< outdims.y -4;j++){
				int pt = (ctx->dims.x * 2) * i + (j * 2);

				if((pt + ctx->dims.x + 1 ) > mxpt){
					goto resample_done;
				}
				pixel cmp =  f(*(ctx->data[pt]),
						f( *(ctx->data[pt + 1]), f(*(ctx->data[pt + ctx->dims.x]), *(ctx->data[pt + ctx->dims.x + 1]))));

				memcpy(ret->data[i * outdims.x + j], &cmp, sizeof(pixel));

			}
		}
	}
resample_done:
	return ret;
}


float* normalize_buffer(uint8_t* buff, size_t len){
	float min = 0.f, max = 255.f;
	float* ret = malloc(len * sizeof(float));
	if(ret != NULL){
		int c = len-1;
		for(int i =0; i<len; i++){
			ret[i] = buff[c--] / max;
		}
	}

	return ret;
}

float* to_dknet_buffer(uint8_t* buff, size_t len){

	int w = DKNET_INPUT_W, h = DKNET_INPUT_H, c = 3;

	float* ret = calloc(sizeof(float), len);

	int i,j,k, ki, ji, ii;
	for(k = 0, ki = 2; k < 3; ++k, --ki){
		for(j = 0, ji = h-1; j < h; ++j, --ji){
			for(i = 0, ii = w-1; i < w; ++i, --ii){
				int dst_index = i + w*ji + w*h*ki;
				int src_index = k + c*i + c*w*j;
				ret[dst_index] = (float)buff[src_index]/255.;
			}
		}
	}


	return ret;
}


float* __to_dknet_buffer(fdimage* ctx){
	char tempfname[500];
	sprintf(tempfname, "%s/__temp.bmp", _g_config->docpath);
	fdimage_write(ctx, tempfname);
	image dknet_image = load_image_color(tempfname, 0, 0);
	image sized = letterbox_image(dknet_image, DKNET_INPUT_H, DKNET_INPUT_W);

	size_t sz = sized.h * sized.w * sized.c;
	float* ret = calloc(sizeof(float) , sz);
	memcpy(ret, sized.data, sz * sizeof(float));
	save_image(sized, "dknetsized.jpg");
	free_image(sized);
	free_image(dknet_image);

	return ret;
}



void __dknetbuffer_to_fdimage(float* buffer, __int2 indims){


	uint8_t* im_buff = malloc(sizeof(uint8_t) * indims.x * indims.y * 3);
	size_t sz = indims.x * indims.y * 3;
	int c =sz-1;

	if(im_buff){


		int i,k, l;
		for(k = 0; k < 3; ++k){
			for(i = 0, l = (416 * 416 )-1; i < 416 * 416; ++i, l--){
				im_buff[l*3+k] = (uint8_t) (255*buffer[i + k*416* 416]);
			}
		}

		static int count = 0;

		char fname[100];
		sprintf(fname, "unnormalized%d.bmp", count++);

		fdimage* ctx = buffer_to_fdimage(im_buff, indims);
		if(ctx){
			fdimage_write(ctx, fname); 
			free_fdimage(ctx);


		}
	}


}

static inline int _sign(int num){
	return ( (num > 0) ? 1 : ((num < 0) ? -1 : 0) );
}


void fdimage_draw_point(fdimage* ctx, __int2 pt, pixel* clr){
	if(ctx != NULL && clr != NULL){
		ctx->data[pt.x * ctx->dims.x + pt.y ]->r = clr->r;
		ctx->data[pt.x * ctx->dims.x + pt.y ]->g = clr->g;
		ctx->data[pt.x * ctx->dims.x + pt.y ]->b = clr->b;
	}
}


void fdimage_draw_diag(fdimage* ctx, __int2 pt1, __int2 pt2, pixel* clr){
	if(ctx != NULL && clr != NULL){
		int deltax = pt2.x - pt1.x;
		int deltay = pt2.y - pt1.y;

		float deltaerr = abs(deltay / (float) deltax);

		float err = 0.f;
		int j = pt1.x;
		__int2 curpt = {.x = j};;

		for(int i = pt1.y; i<pt2.y; i++){
			curpt.y = i;	
			fdimage_draw_point(ctx, curpt, clr);
			err = err + deltaerr;

			if(err >= 0.5f){
				j += _sign(deltay) * 1;
				curpt.x = j;
				err -= 1.f;
			}
		}
	}
}


void fdimage_draw_line(fdimage* ctx, __int2 pt1, __int2 pt2, pixel* clr){
	int deltax = pt2.x - pt1.x;
	int deltay = pt2.y - pt1.y;
	__int2 curpt; 

	if(deltax == 0){ // horizontal 
		curpt.x = pt1.x ;
		for(int i = pt1.y; i<pt2.y;i++){
			curpt.y = i;
			fdimage_draw_point(ctx, curpt, clr);
		}
	}
	else if(deltay == 0){ // veritcal
		curpt.y = pt1.y ;
		for(int i = pt1.x; i<pt2.x;i++){
			curpt.x = i;
			fdimage_draw_point(ctx, curpt, clr);
		}
	}
	else{
		fdimage_draw_diag(ctx, pt1, pt2, clr);
	}

}




void fdimage_draw_square(fdimage* ctx, __int2 bottomleft, __int2 topright, pixel* clr){
	if(ctx != NULL && clr != NULL && bottomleft.x <= ctx->dims.x && bottomleft.y <= ctx->dims.y && topright.x <= ctx->dims.x && topright.y <= ctx->dims.y){
		__int2 topleft = {topright.x, bottomleft.y};
		__int2 bottomright = {bottomleft.x, topright.y};

		fdimage_draw_line(ctx,  bottomleft, bottomright,clr);
		fdimage_draw_line(ctx, bottomleft, topleft, clr);
		fdimage_draw_line(ctx , topleft, topright,  clr);
		fdimage_draw_line(ctx, bottomright, topright, clr);
	}
}

