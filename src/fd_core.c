#include <stdio.h>
/*
#include <fd/fd_core.h>
#include <fd/fd_types.h>
*/

#include "fd_core.h"
#include "fd_types.h"
#include "image.h"

#include <stdint.h>

#include "linked_list.h"

fd_config_t* _g_config = NULL;
static list* _waitq = NULL;
static list* _doneq = NULL;


fd_status init(fd_config_t config){
	_waitq = make_list();
	_doneq = make_list();

	_g_config = malloc(sizeof(fd_config_t));
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

fd_status fd_shutdown(){
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

