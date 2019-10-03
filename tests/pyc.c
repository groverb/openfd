#include <Python.h>


static PyObject* pName = NULL;
static PyObject* pModule = NULL;
static PyObject* pFunc = NULL;
static PyObject* pArgs = NULL;
static PyObject* pValue = NULL;

static PyObject* atoplist(const uint8_t* buff, size_t buff_len){
	PyObject* ret_list = PyList_New(buff_len);

	for(int i = 0;i<(int)buff_len;i++){
		PyList_SetItem(ret_list, i, PyFloat_FromDouble((double)(buff[i])));
	}

	return ret_list;
}
static void init_py_bridge(){

	setenv("PYTHONPATH",".",1);

	Py_Initialize();
	pName = PyString_FromString("__eval__");
	/* Error checking of pName left out */

	pModule = PyImport_Import(pName);
	Py_DECREF(pName);

	if (pModule != NULL) {
		pFunc = PyObject_GetAttrString(pModule, "eval");
		/* pFunc is a new reference */
	}
}

int eval(const uint8_t* buff, size_t buff_len){
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
			// fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
		}
		Py_XDECREF(pFunc);
		Py_DECREF(pModule);
	}
	else {
		PyErr_Print();
		// fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
		return 1;
	}
	return 0;
}

static void free_py_bridge(){
	Py_Finalize();
}

int main(int argc, char *argv[]){
	init_py_bridge();
	uint8_t buff[] = {1,2,3,44,12};
	eval(buff, 5);
	printf("\n");
	buff[2]  = 99;
	eval(buff, 5);
	free_py_bridge();	
	return 0;
}


