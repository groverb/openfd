#ifndef _SLIDING_WINDOW_H
#define _SLIDING_WINDOW_H

#include <fd/fd_types.h>
#include "fdimage.h"
#include "fdlist.h"

fdlist* sw_get_frames(fdimage* ctx, __int2 windims, int step);

#endif

