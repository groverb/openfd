#include "darknet.h"
#include <fd/fd_types.h>

static network* _net = NULL; 

fd_status init_dknet_bridge();


fd_status dknet_eval(float* buffer, size_t buff_len, fd_result_t* result);

food_pos_t dknet_get_food_pos(detections* dets);


