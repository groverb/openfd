#ifndef _EVAL_H
#define _EVAL_H

#include <stdint.h>
#include <fd/fd_types.h>
#include "fdimage.h"
#include "fdlist.h"



fd_status exec_eval_pipeline(uint8_t* buffer, fd_result_t* result);

fd_status prepare_result(fdlist* evaluations, fd_result_t* res);

food_pos_t* eval(fdimage* ctx);

#endif

