#ifndef _DOWNSAMPLE_H
#define _DOWNSAMPLE_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include <common/fd_types.h>

uint8_t* downsample_max(const uint8_t* buff, const __int2 indims);

uint8_t* downsample_min(const uint8_t* buff, const __int2 indims);

uint8_t* downsample_rand(const uint8_t* buff, const __int2 indims);

uint8_t* downsample_avg(const uint8_t* buff, const __int2 indims);


#endif

