#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "image.h"
#include "linked_list.h"
#include "py_bridge.h"
#include "config.h"
#include "slidingwin.h"

pixel GREEN = {0, 255, 0};
pixel RED = {255, 0, 0};
pixel BLUE = {0, 0, 255};

fd_status exec_eval_pipeline(uint8_t* buffer, fd_result_t* result){
	image* ctx = buffer_to_image((uint8_t*) buffer, _g_config->indims);
	// sliding window
	__int2 windims = {SW_WINDIMS_X, SW_WINDIMS_Y};
	int step = SW_STEP_SIZE;

	if(ctx != NULL){
		list* frames = NULL;
		if(ctx->dims.x > EVAL_INPUT_SIZE_Y || ctx->dims.y > EVAL_INPUT_SIZE_Y){
			__int2 outdims = {EVAL_INPUT_SIZE_X, EVAL_INPUT_SIZE_Y};

			image* ctx_small = image_resample(ctx, outdims, &pixel_max);
			frames = sw_get_frames(ctx_small, windims, SW_STEP_SIZE);
			printf("done %d making frames\n", frames->size);
			image_write(ctx_small, "ctxsmall.bmp");
		
			free_image(ctx_small);
		}	
		else{
			frames = sw_get_frames(ctx, windims, SW_STEP_SIZE);
		}	
		list* evaluations= make_list();

		// parse list
		if(frames != NULL){
			node* cur = frames->HEAD;
			int fid = 0,c=0;
			printf("parsing list\n");

			do{	
			//	printf("starting eval\n");
				// eval each frame
				food_pos_t* out = eval((image*)(cur->val));	
				if(out != NULL){
					push_back(evaluations, out, 'u');
				}
				cur = cur->next;
		//	printf("finished list item: %d\n", c++);
			}	while(cur->next != NULL);
			assert(prepare_result(evaluations, result) == fd_ok);
			//dbg
			for(int i =0;i<result->num_fooditems;i++){
				__int2 topright = {result->fooditems[i].food_pos.x + 64 , result->fooditems[i].food_pos.y + 64};
			
				if(strcmp(result->fooditems[i].food_name, "sashimi")== 0){
				
					image_draw_square(ctx, result->fooditems[i].food_pos, topright, &RED); 
				}
					
					if(strcmp(result->fooditems[i].food_name, "donuts") == 0){
					image_draw_square(ctx, result->fooditems[i].food_pos, topright, &GREEN); 
				}
			}
			
#if dbg
			image_write(ctx, "final.bmp");
#endif

			free_list(evaluations);
			free_list_custom(frames, &free_image, image);
			free_image(ctx);

			return fd_ok;
		}
	}
	return fd_nullptr;
}

fd_status prepare_result(list* evaluations, fd_result_t* res){
	if(evaluations != NULL && res != NULL){

		if(evaluations->size == 0){
			res->num_fooditems = 0;
		}
		else{
			res->num_fooditems = (evaluations->size < API_MAX_RET_FOOD_COUNT) ? evaluations->size : API_MAX_RET_FOOD_COUNT;

			node* cur = evaluations->HEAD;

			for(int i =0;i<res->num_fooditems; i++){
				memcpy(&(res->fooditems[i]), (food_pos_t*)(cur->val), sizeof(food_pos_t));
				cur = cur->next;
			}
		}
		return fd_ok;
	}
	return fd_nullptr;
}

food_pos_t* eval(image* ctx){
	food_pos_t* ret = NULL; 
	if(ctx != NULL){	
		uint8_t* ctx_buff = image_to_buffer(ctx);
		if(ctx_buff !=  NULL){
			py_ret_tup res = _py_eval(ctx_buff, ctx->dims.x * ctx->dims.y * 3);

			if(res.confidence >= NN_CONFIDENCE_THRESHOLD){
				ret = malloc(sizeof(food_pos_t));
				if(ret != NULL){
					strcpy(ret->food_name, _g_categories_lut[res.category] );
					ret->food_pos.x = ctx->__relative_pos.x;
					ret->food_pos.y = ctx->__relative_pos.y;
					ret->__confidence = res.confidence;

#if dbg
__int2 endp = {ret->food_pos.x + SW_WINDIMS_X, ret->food_pos.y + SW_WINDIMS_Y};
image_draw_square(ctx, ret->food_pos, endp, &GREEN);

char fpath[30];
sprintf(fpath, "positive/%d_%d.bmp", ret->food_pos.x, ret->food_pos.y);
image_write(ctx, fpath);
#endif
				}
			}
			free(ctx_buff);
		}

	}
	return ret;
}


