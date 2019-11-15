
#include "py_bridge.h"

#include <Python.h>
#include <fd/fd_types.h>


#include "qdbmp.h"
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "fdimage.h"

static PyObject* pname = NULL;
static PyObject* pmodule = NULL;
static PyObject* pmain_func = NULL, *pinit_func = NULL;
static PyObject* pargs = NULL;
static PyObject* pvalue = NULL;

static PyObject* atopfdlist(uint8_t* buff, size_t buff_len){
	PyObject* ret_fdlist = PyList_New((int)buff_len);
	if(ret_fdlist == NULL){ printf("couldnt make fdlist\n"); exit(-1); }
	for(int i = 0;i< (int)buff_len;i++){
		PyList_SetItem(ret_fdlist, i, PyFloat_FromDouble((double)(buff[i])));
	}
	return ret_fdlist;
}

fd_status init_py_bridge(const char* docpath){

	// setenv("PYTHONPATH", _g_python_path ,1);
	setenv("PYTHONPATH", docpath, 1);
	printf("2\n");

	Py_Initialize();
	pname = PyUnicode_DecodeFSDefault("__eval__");
	if(pname == NULL) { printf("PNAME NULL\n");}

	pmodule = PyImport_Import(pname);
	// Py_DECREF(pname);

	if (pmodule != NULL) {
		pinit_func = PyObject_GetAttrString(pmodule, "init_py_bridge");
		// pinit_func = PyObject_GetAttrString(pmodule, "py_eval");


		// call init_py_bridge
		pargs = PyTuple_New(1);
		PyObject* py_docpath = PyUnicode_DecodeUTF8(docpath, strlen(docpath), NULL );

		if(py_docpath == NULL){ printf("NULL docpath\n");}
		//  PyObject* py_docpath = PyString_FromString(docpath);
		PyTuple_SetItem(pargs, 0, py_docpath);

		pvalue = PyObject_CallObject(pinit_func, pargs );

		return fd_ok;
	}
	PyErr_Print();
	return fd_nullptr;

}


void _py_eval(uint8_t* buff, size_t buff_len, ret_tup* ctx){

	// Py_Initialize();
	//pname = PyUnicode_DecodeFSDefault("__eval__");
	//pmodule = PyImport_Import(pname);

	if(pmodule != NULL){
		pmain_func = PyObject_GetAttrString(pmodule, "py_eval");


		if (pmain_func && PyCallable_Check(pinit_func)) {
			pargs = PyTuple_New(1);

			PyTuple_SetItem(pargs, 0, atopfdlist(buff, buff_len));

			pvalue = PyObject_CallObject(pmain_func, pargs );

			PyObject* cat = PyTuple_GetItem(pvalue, 0);
			PyObject* conf = PyTuple_GetItem(pvalue, 1);
			if(cat == NULL ) { printf("tuple ret null\n"); }
			double cconf = PyFloat_AsDouble(conf);
			int ccat = (int) PyFloat_AsDouble(cat);

			ctx->category = ccat;
			ctx->confidence = cconf;


			// Py_DECREF(pargs);
		}
		else {
			if (PyErr_Occurred())
				PyErr_Print();
		}
		// Py_XDECREF(pmain_func);
		// Py_DECREF(pmodule);
	}
	else {
		PyErr_Print();
	}
}



fd_status free_py_bridge(){
	Py_Finalize();
	return fd_ok;

}

