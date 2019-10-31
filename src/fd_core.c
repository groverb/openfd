#include <stdio.h>
#include <assert.h>
#include <stdint.h>

#include <fd/fd_core.h>
#include <fd/fd_types.h>

#include "image.h"
#include "slidingwin.h"
#include "linked_list.h"
#include "eval.h"
#include "py_bridge.h"
#include "darknet.h"
#include "dknet_bridge.h"

fd_config_t* _g_config = NULL;
char _g_categories_lut[NN_CATEGORY_COUNT][30];
static list* _waitq = NULL;
static list* _doneq = NULL;

static fd_status load_categories(){
	FILE* catf;
	char catfpath[150];
	sprintf(catfpath, "%s/categories.txt", _g_config->docpath);
	catf = fopen(catfpath, "r");
	if(catf != NULL){
		for(int i = 0;i<NN_CATEGORY_COUNT;i++){
			fscanf(catf, "%s\n", _g_categories_lut[i]);

		}
		fclose(catf);
		return fd_ok;
	}
	return fd_filenotfound;
}


fd_status fd_init(fd_config_t config){
	_waitq = make_list();
	_doneq = make_list();
	
	_g_config = malloc(sizeof(fd_config_t));
	memcpy(_g_config, &config, sizeof(fd_config_t));
#if PYTHON_EVAL
#ifdef PYTHON_SERVER
	char* external_docpath = getenv("PYTHON_SERVER_PATH");
	if(external_docpath != NULL){
		strcpy(_g_config->docpath, external_docpath );
	}
#endif
	
	assert(load_categories() == fd_ok);
	return init_py_bridge(_g_config->docpath);

#elif DKNET_EVAL
	return init_dknet_bridge();
#endif
	}

fd_status fd_configure_input(fd_config_t config){
	memcpy(_g_config, &config, sizeof(fd_config_t));
	return fd_ok;
}

fd_status fd_submit_image_buffer(int frameid, void* buffer){

	if(buffer == NULL){ return fd_invalidargs; }
	if(_waitq == NULL){ return fd_notinitialized; }

	image* ctx = buffer_to_image((uint8_t*) buffer, _g_config->indims);

	if(ctx != NULL){
		push_back(_waitq, ctx, 'u');
		return fd_ok;
	}
	return fd_nullptr;

}

fd_status fd_get_last_result(fd_result_t* result){

	if(result == NULL){ return fd_invalidargs; }
	if(_doneq == NULL) { return fd_notinitialized; }

	const node* head = list_head(_doneq);
	if(head != NULL && head->val != NULL){
		memcpy(result, head->val, sizeof(fd_result_t));
		pop_front(_doneq);
		return fd_ok;
	}
	return fd_nullptr;

}

fd_status fd_get_result_sync(int frameid, void* buffer, fd_result_t* result){
	return exec_eval_pipeline((uint8_t*) buffer, result);
}

fd_status __fd_get_result_sync( char* fname, fd_result_t* res){
	food_pos_t* ret = dknet_get_food_pos(fname);
	if(ret != NULL){
		memcpy(&(ret[0]), ret, sizeof(food_pos_t));
		return fd_ok;
	}
	return fd_nullptr;
}


fd_status fd_shutdown(){
	assert(free_py_bridge() == fd_ok);
	if(_g_config != NULL && _doneq != NULL && _waitq != NULL){
		free(_g_config);
		_g_config = NULL;

		free(_doneq);
		_doneq = NULL;

		free(_waitq);
		_waitq = NULL;


		return fd_ok;
	}
	return fd_notinitialized;
}

