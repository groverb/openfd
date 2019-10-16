#ifndef _FD_TYPES_H
#define _FD_TYPES_H


typedef struct {int x; int y; } __int2;
typedef struct {float x; float y; } __float2;
typedef struct {int x; int y; int z; } __int3;
typedef struct {float x; float y; float z; } __float3;

typedef struct _food_pose_t{
	char* food_name;
	__int2 food_pose;
	__int2 projected_food_pose;
} food_pose_t;

typedef struct _fd_result_t{
	int frame_id;
	double timestamp;
	int num_fooditems;
	food_pose_t food_poses[10];
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
} fd_config_t;

#endif

