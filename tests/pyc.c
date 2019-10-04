#include <Python.h>
#include "fd_types.h"
#include "qdbmp.h"

static PyObject* pName = NULL;
static PyObject* pModule = NULL;
static PyObject* pFunc = NULL;
static PyObject* pArgs = NULL;
static PyObject* pValue = NULL;

static PyObject* atoplist(const uint8_t* buff, size_t buff_len){
	PyObject* ret_list = PyList_New(buff_len);
	for(int i = 0;i<= (int)buff_len;i++){
		PyList_SetItem(ret_list, i, PyFloat_FromDouble((double)(buff[i])));
	}
	return ret_list;
}


fd_status init_py_bridge(){

	setenv("PYTHONPATH",".",1);

	Py_Initialize();
	pName = PyString_FromString("__eval__");
	/* Error checking of pName left out */

	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule != NULL) {
		pFunc = PyObject_GetAttrString(pModule, "eval");
		return fd_ok;
		/* pFunc is a new reference */
	}
}


static int __py_eval(const uint8_t* buff, size_t buff_len){
	if(pModule != NULL){
		if (pFunc && PyCallable_Check(pFunc)) {
			pArgs = PyTuple_New(1);

			PyTuple_SetItem(pArgs, 0, atoplist(buff, buff_len));

			pValue = PyObject_CallObject(pFunc, pArgs );
			Py_DECREF(pArgs);
		}
		else {
			if (PyErr_Occurred())
				PyErr_Print();
		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);
	}
	else {
		PyErr_Print();
		return 1;
	}
	return 0;
}

static void free_py_bridge(){
	Py_Finalize();
}

int eval(const uint8_t* buff, size_t buff_len){
	return __py_eval(buff, buff_len);
}

int main(int argc, char *argv[]){
	init_py_bridge();

	BMP* cbmp = BMP_ReadFile(argv[1]);

	eval(cbmp->Data,  64 * 64 * 3);
	// atoplist(cbmp->Data, 64 * 64 * 3);
	printf("done \n");
	free_py_bridge();	
	return 0;
}


