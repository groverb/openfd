#ifndef _FD_TYPES_H
#define _FD_TYPES_H

// ---------- config
#define API_MAX_RET_FOOD_COUNT 80 

#define EVAL_INPUT_SIZE_X 512 
#define EVAL_INPUT_SIZE_Y 512

#define SW_STEP_SIZE 200 
#define SW_WINDIMS_X 64 
#define SW_WINDIMS_Y 64

#define NN_CONFIDENCE_THRESHOLD 0.999
#define NN_CATEGORY_COUNT 101

// dbg levels (val: 0,1)
#define dbgl1 1
#define dbgl2 0
// ----------



typedef struct {int x; int y; } __int2;
typedef struct {float x; float y; } __float2;
typedef struct {int x; int y; int z; } __int3;
typedef struct {float x; float y; float z; } __float3;

typedef struct _food_pos_t{
	char food_name[30];
	__int2 food_pos;
	__int2 projected_food_pos;
	double __confidence;
} food_pos_t;

typedef struct _fd_result_t{
	int frame_id;
	double timestamp;
	int num_fooditems;
	food_pos_t fooditems[80];
} fd_result_t;


typedef enum _fd_status{
	fd_ok = 0,
	fd_failed,
	fd_notimplemented,
	fd_nullptr,
	fd_notinitialized,
	fd_invalidargs,
	fd_accessviolation,
	fd_filenotfound
} fd_status;

typedef enum _buffer_format{
	rgb_uint8 = 0,
	rgb_float32,
	rgb_float64,
} buffer_format;

typedef struct _fd_config_t{
	__int2 indims;
	buffer_format format;
	char docpath[100];
} fd_config_t;

#endif

