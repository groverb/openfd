#include<stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fd/fd_core.h>
#include <assert.h>

#define HEADER_SZ 54 


extern void __dknetbuffer_to_fdimage(float*, __int2);


void write_buffer(const char* tfname){

	size_t sz = 416 * 416 * 3;

	FILE* tfil = fopen(tfname, "r");
	float* buff = malloc(sizeof(float) * sz);
	
	int c = sz - 1;

	for(int i=0;i<sz; i++){
		fscanf(tfil, "%f,", &(buff[c--]));

	}
	__int2 _indims = {416, 416};


	__dknetbuffer_to_fdimage(buff, _indims);
}

int main(int argc, char** argv){
	

	__int2 _indims= {.x = 416, .y = 416};
	char* _docpath = argv[1];
	fd_config_t conf = {.indims = _indims, .format = rgb_uint8};
	strcpy(conf.docpath , _docpath);

	assert(fd_init(conf) == fd_ok);
	printf("assertion passed\n");
	

	FILE* img = fopen(argv[2], "rb");
	
	assert(img != NULL);

	uint8_t header[HEADER_SZ];
	fread(header, HEADER_SZ, 1, img);
	
	uint8_t* buffer = malloc(sizeof(uint8_t) * _indims.x * _indims.y * 3);
	fread(buffer, _indims.x * _indims.y * 3, 1, img);
	

	fd_result_t res;
	
	write_buffer("normbuffer.txt");
//	return 0;
	assert(fd_get_result_sync(0, buffer, &res) == fd_ok);


	
	
	return 0;




}
