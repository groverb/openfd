#ifndef _CNN_H
#define _CNN_H

#include <common/linked_list.h>
#include <common/fd_types.h>
#include <math.h>


typedef enum _lname{
	LCONV2D = 0,
	LACTIATION,
	LSOFTMAX
} lname;

typedef struct _layer{
	lname name;
	__int3 indims;
	__int3 outdims;
	float* weights;
	float* bias;
} layer;


typedef struct _model{
	list* layers;
} model;

static float sigmoid_f(float t) { return 1.0f / (1 + exp(-t)); }
static float sigmoid_df(float f_t) { return f_t*(1 - f_t); }

static float tanh_f(float t) { float e = exp(2 * t); return (e - 1) / (e + 1); }     
static float tanh_df(float f_t) { return 1.0f - f_t*f_t; }

static float relu_f(float t) { return fmax(0.0f, t); } 
static float relu_df(float f_t) { return (f_t > 0.0f) ? 1.0f : 0.0f; }

static float leaky_relu_f(float t) { return fmax(0.01f*t, t); }
static float leaku_relu_df(float f_t) { return (f_t > 0.0f) ? 1.0f : 0.01f; }

typedef enum _activationf{
	SIGMOID= 0,
	TANH,
	RELU,
	LEAKY_RELU
} activationf;


static void lactivation_fwd( float (*f)(float), const uint8_t* in, uint8_t* out, size_t in_size){
	for(int i= 0;i<in_size;i++){
		out[i] =f((float)(in[i]));
	}
}

static void lavgpool_fwd(){
	// already impl in preprocessor(avg downsample )
	// TODO: port it over
}
static void lmaxpool_fwd(){

	// already impl in preprocessor(max downsample )
	// TODO: port it over

}

static void lsparsepool_fwd();


static void lconv3d_fwd(const float* in, float* out, __int3 indims, __int3 outdims){
	
}
static void lconv2d_fwd();
static void lsoftmax_fwd();
static void lfull_fwd();
static void lcrossentopy_fwd();

static void forward(layer* _layer, uint8_t* in);

// exposed
void eval(model* ctx, const uint8_t* buffer, __int2 indims);

#endif


