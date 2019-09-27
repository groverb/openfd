#include <preprocessor/downsample.h>

static uint8_t u8_max(uint8_t a, uint8_t b){ return a > b ? a : b; }
static uint8_t u8_min(uint8_t a, uint8_t b){ return a < b ? a : b; }
static uint8_t u8_avg(uint8_t a, uint8_t b){ return (uint8_t)((a+b)/2.f); }
static uint8_t u8_rand(uint8_t a, uint8_t b){ return (rand() & 1) ? a : b; }

typedef struct {uint8_t r; uint8_t g; uint8_t b;} pixel;

static pixel* get_buff_as_pixels(uint8_t* buff, int buff_len){
	int ret_len = (int)(buff_len /3.f);
	pixel* ret = malloc( ret_len * sizeof(pixel));

	for(int i =0;i<buff_len; i+=3){
		pixel temp = {.r = buff[i], .g = buff[i+1], .b = buff[i+2]};
		ret[(int)(i/3)] = temp; 
	}

	return ret;

}

static uint8_t* downsample(uint8_t (*f)(uint8_t,uint8_t), const uint8_t* ctx, const __int2 indims){
	if(indims.x != indims.y){ printf("pass square image\n"); return NULL; }
	__int2 outdims = {indims.x / 2, indims.y /2 };

	uint8_t* ret_ctx = calloc( (outdims.x * outdims.y * 3) , sizeof(uint8_t ));

	if(ret_ctx){

		for(int i = 0; i < (outdims.x ) ; i++){
			for(int j = 0; j < (outdims.y) ; j++){

				int acc = (indims.x * 3 * 2) * i + (j * 6 );

				ret_ctx[i * (outdims.x * 3) + (j *3)] =  
					f(ctx[acc + 0] ,
							f(ctx[ acc + 3] ,
								f(ctx[(acc + 0 ) +  (indims.x * 3)] , ctx[(acc +  (indims.x * 3) + 3) ] )));

				ret_ctx[i * (outdims.x*3) + (j*3) +1] =
					f(ctx[acc + 0+1] ,
							f(ctx[ acc + 3 +1] ,
								f(ctx[(acc + 0 ) +  (indims.x * 3)+1] , ctx[(acc +  (indims.x*3) + 3) +1])));


				ret_ctx[i * (outdims.x*3) + ( j*3) +2] =  
					f(ctx[acc + 0 + 2] ,
							f(ctx[ acc + 3 + 2] ,
								f(ctx[(acc + 0 ) +  (indims.x * 3) +2 ] , ctx[(acc +  (indims.x*3) + 3) +2] )));
			}
		}

	}
	return ret_ctx;
}

uint8_t* downsample_max(const uint8_t* buff, const __int2 indims){
	return downsample(&u8_max, buff, indims);
}
uint8_t* downsample_min(const uint8_t* buff, const __int2 indims){
	return downsample(&u8_min, buff, indims);
}
uint8_t* downsample_rand(const uint8_t* buff, const __int2 indims){
	return downsample(&u8_rand, buff, indims);
}
uint8_t* downsample_avg(const uint8_t* buff, const __int2 indims){
	return downsample(&u8_avg, buff, indims);
}


