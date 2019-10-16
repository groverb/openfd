#include <Python.h>
#include "fd_types.h"
#include "qdbmp.h"
#include <string.h>
#include <stdlib.h>

static PyObject* pname = NULL;
static PyObject* pmodule = NULL;
static PyObject* pmain_func = NULL, *pinit_func = NULL;
static PyObject* pargs = NULL;
static PyObject* pvalue = NULL;

static PyObject* atoplist(const uint8_t* buff, size_t buff_len){
	PyObject* ret_list = PyList_New((int)buff_len);
	if(ret_list == NULL){ printf("couldnt make list\n"); exit(-1); }
	for(int i = 0;i< (int)buff_len;i++){
		PyList_SetItem(ret_list, i, PyFloat_FromDouble((double)(buff[i])));
	}
	printf("ending list size: %ld\n", PyList_Size(ret_list));
	return ret_list;
}

static void load_categories(const char* docpath, char** out, size_t catsz){
	char catfpath[20];
	sprintf(catfpath, "%s/categories.txt", docpath);
	FILE* catf = fopen(catfpath, "r");
	if(catf != NULL){
		for(int i =0;i<catsz; i++){
			out[i] = malloc(20 * sizeof(char));
			fscanf(catf, "%s", out[i]);
		}
		fclose(catf);
	}
}


fd_status init_py_bridge(const char* docpath){

	setenv("PYTHONPATH",".",1);

	Py_Initialize();
	pname = PyUnicode_DecodeFSDefault("__eval__");
	pmodule = PyImport_Import(pname);
	// Py_DECREF(pname);

	if (pmodule != NULL) {
		pinit_func = PyObject_GetAttrString(pmodule, "init_py_bridge");
		// pmain_func = PyObject_GetAttrString(pmodule, "py_eval");
		if(pmain_func == NULL){
			printf("EVAL FUNTION NOT FOUND");
		}

		// call init_py_bridge
		pargs = PyTuple_New(1);
		PyObject* py_docpath = PyUnicode_DecodeFSDefault(docpath);
		// PyObject* py_docpath = PyString_FromString(docpath);
		PyTuple_SetItem(pargs, 0, py_docpath);



		pvalue = PyObject_CallObject(pinit_func, pargs );

		return fd_ok;
	}
}


int __py_eval(const uint8_t* buff, size_t buff_len){
	printf("in here\n");
	if(pmodule != NULL){
		printf("p mod not null\n");
		pinit_func = PyObject_GetAttrString(pmodule, "py_eval");


		if (pinit_func && PyCallable_Check(pinit_func)) {
			pargs = PyTuple_New(1);

			PyTuple_SetItem(pargs, 0, atoplist(buff, buff_len));

			pvalue = PyObject_CallObject(pinit_func, pargs );

			int ret = (int)PyFloat_AsDouble(pvalue);
			printf("Got back: %d\n", ret);
			return ret;

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
		return -1;
	}
	return -1;
}

static void free_py_bridge(){
	Py_XDECREF(pmain_func);
	Py_XDECREF(pinit_func);
	Py_DECREF(pmodule);
	Py_Finalize();
}

int eval(const uint8_t* buff, size_t buff_len){
	return __py_eval(buff, buff_len);
}

int main(int argc, char *argv[]){
	BMP* cbmp = BMP_ReadFile(argv[2]);

	assert(init_py_bridge(argv[1]) == fd_ok);
	int catsz = 101;

	char* categories[101]; //  = malloc(sizeof(char) * catsz);
	load_categories(argv[3], categories, catsz);

	printf("h: %lu, w: %lu\n", BMP_GetHeight(cbmp), BMP_GetWidth(cbmp));
	printf("HERE\n");
	printf("predicted category: %s\n", categories[eval(cbmp->Data,  64 * 64 * 3)]) ;
	// atoplist(cbmp->Data, 64 * 64 * 3);
	printf("done \n");

	for(int i =0;i<catsz; i++){
		free(categories[i]);
	}

	// free_py_bridge();	

	return 0;
}


