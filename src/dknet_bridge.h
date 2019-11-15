#include "darknet.h"
#include <fd/fd_types.h>


fd_status init_dknet_bridge();

fd_status dknet_eval(float* buffer, food_pos_t* foodpos);

