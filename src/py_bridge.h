#ifndef _PY_BRIDGE_H_
#define _PY_BRIDGE_H_

#include <Python.h>
#include <fd/fd_types.h>

#include "qdbmp.h"
#include <string.h>
#include <stdlib.h>


fd_status init_py_bridge(const char* docpath);

int _py_eval(const uint8_t* buff, size_t buff_len);

fd_status free_py_bridge();


#endif
