
#include "py_bridge.h"

#include <Python.h>
#include <fd/fd_types.h>


#include "qdbmp.h"
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "image.h"

static PyObject* pname = NULL;
static PyObject* pmodule = NULL;
static PyObject* pmain_func = NULL, *pinit_func = NULL;
static PyObject* pargs = NULL;
static PyObject* pvalue = NULL;

static PyObject* atoplist(uint8_t* buff, size_t buff_len){
	PyObject* ret_list = PyList_New((int)buff_len);
	if(ret_list == NULL){ printf("couldnt make list\n"); exit(-1); }
	for(int i = 0;i< (int)buff_len;i++){
		PyList_SetItem(ret_list, i, PyFloat_FromDouble((double)(buff[i])));
	}
	return ret_list;
}

static void load_categories(const char* docpath, char** out, size_t catsz){
	char catfpath[20];
	sprintf(catfpath, "%s/categories.txt", docpath);
	FILE* catf = fopen(catfpath, "r");
	if(catf != NULL){
		for(int i =0;i<(int)catsz; i++){
			out[i] = malloc(20 * sizeof(char));
			fscanf(catf, "%s", out[i]);
		}
		fclose(catf);
	}
}


fd_status init_py_bridge(const char* docpath){

	// setenv("PYTHONPATH", _g_python_path ,1);
	setenv("PYTHONPATH", ".", 1);

	Py_Initialize();
	pname = PyUnicode_DecodeFSDefault("__eval__");
	if(pname == NULL) { printf("PNAME NULL\n");}

	pmodule = PyImport_Import(pname);
	// Py_DECREF(pname);

	if (pmodule != NULL) {
		pinit_func = PyObject_GetAttrString(pmodule, "init_py_bridge");
		// pinit_func = PyObject_GetAttrString(pmodule, "py_eval");


		// call init_py_bridge
		pargs = PyTuple_New(0);
		// PyObject* py_docpath = PyUnicode_DecodeFSDefault(docpath);
		//  PyObject* py_docpath = PyString_FromString(docpath);
		// PyTuple_SetItem(pargs, 0, py_docpath);

		pvalue = PyObject_CallObject(pinit_func, pargs );

		return fd_ok;
	}
	return fd_nullptr;

}


py_ret_tup _py_eval(uint8_t* buff, size_t buff_len){

	// Py_Initialize();
	//pname = PyUnicode_DecodeFSDefault("__eval__");
	//pmodule = PyImport_Import(pname);

	if(pmodule != NULL){
		pmain_func = PyObject_GetAttrString(pmodule, "py_eval");


		if (pmain_func && PyCallable_Check(pinit_func)) {
			pargs = PyTuple_New(1);

			PyTuple_SetItem(pargs, 0, atoplist(buff, buff_len));

			pvalue = PyObject_CallObject(pmain_func, pargs );

			PyObject* cat = PyTuple_GetItem(pvalue, 0);
			PyObject* conf = PyTuple_GetItem(pvalue, 1);
			if(cat == NULL ) { printf("tuple ret null\n"); }
			double cconf = PyFloat_AsDouble(conf);
			int ccat = (int) PyFloat_AsDouble(cat);

			py_ret_tup ret = {.category = ccat, .confidence = cconf};

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
	}
}



fd_status free_py_bridge(){
	// dbg
	
	printf("available py refrences: \n");
	printf("pmainfunc: %d, pinitfunc: %d, pname: %d, pargs: %d, pvalue: %d, pmodule: %d\n", Py_REFCNT(pmain_func),    Py_REFCNT(pinit_func),
			Py_REFCNT(pname),
			Py_REFCNT(pargs),
			Py_REFCNT(pvalue), Py_REFCNT(pmodule)
			);

/*
	while(Py_REFCNT(pmain_func) ==1 ){ Py_XDECREF(pmain_func);} 
	while(Py_REFCNT(pinit_func) ==1){ Py_XDECREF(pinit_func);} 
	while(Py_REFCNT(pname)==1 ){ Py_XDECREF(pname);} 
	while(Py_REFCNT(pargs)==1 ){ Py_XDECREF(pargs);} 
	while(Py_REFCNT(pvalue)==1 ){ Py_XDECREF(pvalue);} 
	while(Py_REFCNT(pmodule) ==1){ Py_XDECREF(pmodule);}
*/
	 Py_Finalize();
printf("pmainfunc: %d, pinitfunc: %d, pname: %d, pargs: %d, pvalue: %d, pmodule: %d\n", Py_REFCNT(pmain_func),    Py_REFCNT(pinit_func),
			Py_REFCNT(pname),
			Py_REFCNT(pargs),
			Py_REFCNT(pvalue), Py_REFCNT(pmodule)
			);
	return fd_ok;

}

#if 0 
int main(int argc, char** argv){
	printf("opening %s\n", argv[2]);

	// BMP* cbmp = BMP_ReadFile(argv[2]);

	assert(init_py_bridge(argv[1]) == fd_ok);
	int catsz = 101;

	char* categories[101]; //  = malloc(sizeof(char) * catsz);
	load_categories(argv[3], categories, catsz);

	// printf("h: %lu, w: %lu\n", BMP_GetHeight(cbmp), BMP_GetWidth(cbmp));
	printf("HERE\n");
	// 	printf("predicted category: %s\n", categories[eval(cbmp->Data,  64 * 64 * 3)]) ;
	// atoplist(cbmp->Data, 64 * 64 * 3);
	printf("done \n");

	for(int i =0;i<catsz; i++){
		free(categories[i]);
	}

	// free_py_bridge();	

	return 0;
}

#endif

