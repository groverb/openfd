#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "image.h"
#include "linked_list.h"
#include "py_bridge.h"
#include "config.h"
#include "slidingwin.h"


static void free_image_list(list* l){
	node* temp;
	int i;
if(l->size >= 1){
			temp = l->HEAD;
			free_image((image*)(temp->val));
			free(temp);
			free(l);
			return;
		}
	while(l->HEAD != NULL){
		temp = l->HEAD;
		l->HEAD = l->HEAD->next;
		
		if(l->size >= 1){
			free_image((image*)(temp->val));
			
		}

		printf("i: %d\t size: %d\n",i++, l->size);
		free(temp);
		temp = NULL;
	}
	l->size = 0;
	int finalsz = l->size;

	free(l);
	l = NULL;

}
fd_status exec_eval_pipeline(uint8_t* buffer, fd_result_t* result){
	image* ctx = buffer_to_image((uint8_t*) buffer, _g_config->indims);

	// sliding window
	__int2 windims = {SW_WINDIMS_X, SW_WINDIMS_Y};
	int step = SW_STEP_SIZE;

	if(ctx != NULL){
		list* frames = sw_get_frames(ctx, windims, SW_STEP_SIZE);
		list* evaluations= make_list();

		// parse list
		node* cur = frames->HEAD;
		int fid = 0;
		do{
			// eval each frame
			food_pos_t* out = eval((image*)(cur->val));	
			if(out != NULL){
				push_back(evaluations, out, 'u');

			}
		}	while(cur->next != NULL);

		assert(prepare_result(evaluations, result) == fd_ok);
		
		free_list(evaluations);
	//  free_image_list(frames);
		free_list_custom(frames, &free_image, image);

	free_image(ctx);

		return fd_ok;

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
		if(ctx->dims.x != 64 || ctx->dims.y != 64){
		}

		uint8_t* ctx_buff = image_to_buffer(ctx);
		if(ctx_buff !=  NULL){
			py_ret_tup res = _py_eval(ctx_buff, ctx->dims.x * ctx->dims.y * 3);

			if(res.confidence >= NN_CONFIDENCE_THRESHOLD){
				ret = malloc(sizeof(food_pos_t));
				if(ret != NULL){
					strcpy(ret->food_name, _g_categories_lut[res.category] );
					ret->food_pos.x = ctx->__relative_pos.x;
					ret->food_pos.y = ctx->__relative_pos.y;
				}
			}
			free(ctx_buff);
		}

	}
	return ret;
}


