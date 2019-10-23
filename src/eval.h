#ifndef _EVAL_H
#define _EVAL_H

#include <stdint.h>
#include <fd/fd_types.h>
#include "image.h"

fd_status exec_eval_pipeline(uint8_t* buffer, fd_result_t* result);

fd_result_t* eval(image* ctx);

#endif
