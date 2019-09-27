#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <common/fd_types.h>
#include <common/linked_list.h>
#include <cnn/cnn.h>

#include "c_npy.h"

static fd_status load_layer_weights(layer* ctx, FILE* fptr){
	if(fptr != NULL && ctx != NULL){
		int dims = ctx->indims.x * ctx->indims.y * ctx->indims.z;
		ctx->weights = malloc(dims * sizeof(float));
		for(int i = 0; i < dims;i++){
			fread(&(ctx->weights[i]), sizeof(float), 1, fptr);
		}
		return fd_ok;
	}
	return fd_failed;
}

static fd_status save_layer_weights(layer* ctx, FILE* fptr){
	if(fptr != NULL && ctx != NULL){
		int dims = ctx->indims.x * ctx->indims.y * ctx->indims.z;
		for(int i = 0; i< dims; i++){
			fwrite(&(ctx->weights[i]), sizeof(float), 1, fptr);
		}
		return fd_ok;
	}
	return fd_failed;
}



fd_status load_weights( model* ctx, const char* fname){
	if(ctx != NULL && ctx->layers != NULL){
		FILE* weightsf;
		weightsf = fopen(fname, "rb");
		if(weightsf != NULL){

			node* curr = ctx->layers->HEAD;
			while(curr != NULL){
				if(load_layer_weights((layer*)(curr->val), weightsf) == fd_ok ){
					curr = curr->next;
				} else{
					fclose(weightsf);
					return fd_failed;
				}
			}
			fclose(weightsf);
			return fd_ok;
		}
		return fd_filenotfound;
	}
	return fd_invalidargs;
}

fd_status save_weights(const model* ctx, const char* fname){
	if(ctx != NULL && ctx->layers != NULL){
		FILE* weightsf;
		weightsf = fopen(fname, "wb");
		if(weightsf != NULL){
			node* curr = ctx->layers->HEAD;
			while(curr != NULL){
				if(save_layer_weights((layer*)(curr->val), weightsf) == fd_ok){
					curr = curr->next;
				} else{
					fclose(weightsf);
					return fd_failed;
				}
			}
			fclose(weightsf);
			return fd_ok;
		}
	}

}


fd_status load_weights_npy(model* ctx, const char* fname){
	// cmatrix_t** np_ar = c_npy_matrix_array_read(fname);
	cmatrix_t* np_ar2 = c_npy_matrix_read_file(fname);
	printf("opening %s\n", fname);

	if(np_ar2 != NULL){
		printf("aint null\n");

		//printf("len: %lu\n", c_npy_matrix_array_length(np_ar2));


	}
	printf("failed\n");

	return fd_failed;
}
fd_status load_weights_h5(layer* ctx, const char* fname);


bool compare_weights(model* _m1, model* _m2){

	if(_m1 != NULL && _m2 != NULL){

		node* m1_curr = _m1->layers->HEAD;
		node* m2_curr = _m2->layers->HEAD;

		while(m1_curr->next != NULL && m2_curr->next != NULL){
			layer* m1_curr_layer =(layer*) m1_curr->val;	
			layer* m2_curr_layer =(layer*) m2_curr->val;

			int dims = m1_curr_layer->indims.x * m1_curr_layer->indims.y * m1_curr_layer->indims.z;
			for(int i =0;i<dims;i++){
				if(m1_curr_layer->weights[i] != m2_curr_layer->weights[i]){
					return 0;
				}
			}
			return 1;
		}
	}
	return 0;
}


// test
int main(int argc, char** argv){


	// write sample file
	FILE* outf = fopen("sampleout", "wb");

	layer* conv = malloc(sizeof(layer));
	conv->indims.x= 3;
	conv->indims.y= 3;
	conv->indims.z= 3;

	layer* activation = malloc(sizeof(layer));
	activation->indims.x= 2;
	activation->indims.y = 2;
	activation->indims.z = 3;


	model vgg;
	vgg.layers = make_list();
	push_back(vgg.layers, (void*) conv, 'u');
	push_back(vgg.layers, (void*)activation, 'u');

	int totdim = (3 * 3 * 3) + (2 * 2 * 3);

	for(int i = 0;i<totdim; i++){
		float f = (float)i;
		fwrite(&f, 4, 1, outf);
	}
	fclose(outf);

	assert(load_weights(&vgg, "sampleout") == fd_ok);
	assert(save_weights(&vgg, "newsample") == fd_ok);



	layer* conv_n = malloc(sizeof(layer));
	conv_n->indims.x= 3;
	conv_n->indims.y= 3;
	conv_n->indims.z= 3;

	layer* activation_n = malloc(sizeof(layer));
	activation_n->indims.x= 2;
	activation_n->indims.y = 2;
	activation_n->indims.z = 3;


	model vgg_new;
	vgg_new.layers = make_list();
	push_back(vgg_new.layers, (void*) conv_n, 'u');
	push_back(vgg_new.layers, (void*)activation_n, 'u');


	assert(load_weights(&vgg_new, "newsample") == fd_ok);
	assert(compare_weights(&vgg, &vgg_new));
	printf("All assertions passed\n");


	free(conv->weights);
	free(activation->weights);
	free_list(vgg.layers);

	return 0;
}
