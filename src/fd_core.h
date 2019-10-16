#ifndef _FD_CORE_H
#define _FD_CORE_H

#include "fd_types.h"

fd_status fd_init(fd_config_t config);

fd_status fd_configure_input(fd_config_t config);

fd_status fd_submit_image_buffer(int frame_id, void* buffer);

fd_status fd_get_last_result(fd_result_t* result);

fd_status fd_shutdown();


#endif

