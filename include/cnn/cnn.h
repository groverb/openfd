#ifndef _CNN_H
#define _CNN_H

#include <common/linked_list.h>
#include <common/fd_types.h>


typedef enum _lname{
	LCONV2D = 0,
	LACTIATION,
	LSOFTMAX
} lname;

typedef struct _layer{
	__int3 indims;
	__int3 outdims;
	float* weights;
	lname name;
} layer;


typedef struct _model{
	list* layers;
} model;



#endif
