#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "fdimage.h"
#include "fdlist.h"
#include "py_bridge.h"
#include "config.h"
#include "slidingwin.h"
#include "dknet_bridge.h"

fd_status exec_eval_pipeline(uint8_t* buffer, fd_result_t* result){
	fdimage* ctx = buffer_to_fdimage((uint8_t*) buffer, _g_config->indims);
	fdimage_write(ctx, "eval_inp.bmp");
	// sliding window
	__int2 windims = {SW_WINDIMS_X, SW_WINDIMS_Y};

	if(ctx != NULL){
		fdlist* frames = NULL;

		if(ctx->dims.x > EVAL_INPUT_SIZE_Y || ctx->dims.y > EVAL_INPUT_SIZE_Y){

			__int2 outdims = {EVAL_INPUT_SIZE_X, EVAL_INPUT_SIZE_Y};

			fdimage* ctx_small = fdimage_resample(ctx, outdims, &pixel_max);
			frames = sw_get_frames(ctx_small, windims, SW_STEP_SIZE);
			free_fdimage(ctx_small);
		}	
		else{
			frames = sw_get_frames(ctx, windims, SW_STEP_SIZE);
		}	
		fdlist* evaluations= make_fdlist();
		// parse fdlist
		if(frames != NULL){
			fdnode* cur = frames->HEAD;
			do{	
				// eval each frame
				food_pos_t* out = eval((fdimage*)(cur->val));

				if(out != NULL){
					push_back(evaluations, out, 'u');
				}
				if(cur->next != NULL){	cur = cur->next;}
			}	while(cur->next != NULL);
			assert(prepare_result(evaluations, result) == fd_ok);
			//dbg
#if dbgl1
			pixel GREEN = {0, 255, 0};
			pixel RED = {255, 0, 0};
			pixel BLUE = {0, 0, 255};


			for(int i =0;i<result->num_fooditems;i++){
				// __int2 topright = {result->fooditems[i].pos_topright.x + 64 , result->fooditems[i].pos_topright.y + 64};

				if(strcmp(result->fooditems[i].food_name, "sashimi")== 0){

					fdimage_draw_square(ctx, result->fooditems[i].pos_bottomleft, result->fooditems[i].pos_topright, &RED); 
				}

				if(strcmp(result->fooditems[i].food_name, "donuts") == 0){
					fdimage_draw_square(ctx, result->fooditems[i].pos_bottomleft, result->fooditems[i].pos_topright, &GREEN); 
				}
				else{
					
					
					fdimage_draw_square(ctx, result->fooditems[i].pos_bottomleft, result->fooditems[i].pos_topright, &BLUE); 
					fdimage_draw_point(ctx,  result->fooditems[i].pos_topright, &GREEN); 
					fdimage_draw_point(ctx,  result->fooditems[i].pos_bottomleft, &BLUE); 
				}
			}
			fdimage_write(ctx, "final.bmp");
#endif

			free_fdlist(evaluations);
			free_fdlist_custom(frames, &free_fdimage, fdimage);
			free_fdimage(ctx);

			return fd_ok;
		}
	}
	return fd_nullptr;
}

fd_status prepare_result(fdlist* evaluations, fd_result_t* res){
	if(evaluations != NULL && res != NULL){

		if(evaluations->size == 0){
			res->num_fooditems = 0;
		}
		else{
			res->num_fooditems = (evaluations->size < API_MAX_RET_FOOD_COUNT) ? evaluations->size : API_MAX_RET_FOOD_COUNT;

			fdnode* cur = evaluations->HEAD;

			for(int i =0;i<res->num_fooditems; i++){
				memcpy(&(res->fooditems[i]), (food_pos_t*)(cur->val), sizeof(food_pos_t));
				if(cur->next != NULL) { cur = cur->next; }
			}
		}
		return fd_ok;
	}
	return fd_nullptr;
}

food_pos_t* eval(fdimage* ctx){
	food_pos_t* ret = NULL; 
	if(ctx != NULL){	
		uint8_t* ctx_buff = fdimage_to_buffer(ctx);
#if PYTHON_EVAL 
		if(ctx_buff !=  NULL){
			ret_tup res;
			_py_eval(ctx_buff, ctx->dims.x * ctx->dims.y * 3, &res);

			if(res.confidence >= NN_CONFIDENCE_THRESHOLD){
				ret = malloc(sizeof(food_pos_t));
				if(ret != NULL){
					strcpy(ret->food_name, _g_categories_lut[res.category] );
					ret->pos_topright.x = ctx->__relative_pos.x;
					ret->pos_topright.y = ctx->__relative_pos.y;
					ret->__confidence = res.confidence;

				}
			}
			free(ctx_buff);
		}
#elif DKNET_EVAL
		int buff_len = ctx->dims.x * ctx->dims.y * 3;

		float* dknet_buffer = to_dknet_buffer(ctx_buff, buff_len);
		assert(dknet_buffer != NULL);
		ret = malloc(sizeof(food_pos_t));
		if(ret != NULL){

			if(dknet_eval(dknet_buffer, ret) ==  fd_nofood){
				free(ret);
				return NULL;
			}
			free(dknet_buffer);
		}
#endif
	}
	return ret;
}


