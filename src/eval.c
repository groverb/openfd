#include "eval.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "image.h"
#include "linked_list.h"
#include "py_bridge.h"
#include "config.h"

fd_status exec_eval_pipeline(uint8_t* buffer, fd_result_t* result){

	image* ctx = buffer_to_image((uint8_t*) buffer, _g_config->indims);

	// sliding window
	__int2 windims = {64, 64};
	int step = 30;

	if(ctx != NULL){
		list* frames = sw_get_frames(ctx, windims, step);
		list* results = make_list();

		// parse list
		node* cur = frames->HEAD;
		while(cur->next != NULL){
			// eval each frame
			fd_result_t* out = eval((image*)(cur->val));	
			assert(out != NULL);

			push_back(results, out, 'u');
		}
	}
	return fd_nullptr;

}


fd_result_t* eval(image* ctx){
	fd_result_t* ret = malloc(sizeof(fd_result_t));

	if(ret != NULL && ctx != NULL){	
		if(ctx->dims.x != 64 || ctx->dims.y != 64){
		}

		uint8_t* ctx_buff = image_to_buffer(ctx);
		if(ctx_buff !=  NULL){
			int cat = _py_eval(ctx_buff, ctx->dims.x * ctx->dims.y * 3);
			printf("core eval(): category: %d\n", cat);


		}

	}
	return ret;
}


