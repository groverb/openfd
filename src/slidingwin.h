#ifndef _SLIDING_WINDOW_H
#define _SLIDING_WINDOW_H

#include <fd/fd_types.h>
#include "image.h"
#include "linked_list.h"

list* sw_get_frames(image* ctx, __int2 windims, int step);

#endif

