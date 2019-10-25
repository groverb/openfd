#ifndef _PY_BRIDGE_H_
#define _PY_BRIDGE_H_

#include <Python.h>
#include <fd/fd_types.h>

#include "qdbmp.h"
#include <string.h>
#include <stdlib.h>

typedef struct _py_ret_tup{
	int category;
	double confidence;
} py_ret_tup;

fd_status init_py_bridge(const char* docpath);

py_ret_tup _py_eval(uint8_t* buff, size_t buff_len);

fd_status free_py_bridge();


#endif
