#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <fd/fd_core.h>
#define HEADER_SZ 122

int main(int argc, char** argv){
	fd_config_t conf = {.format = rgb_uint8};
	conf.indims.x= atoi(argv[2]); 
	conf.indims.y=  atoi(argv[3]);
	
	sprintf(conf.docpath, "%s", argv[4]);

	assert(fd_init(conf) == fd_ok);

	FILE* imgf;
	imgf = fopen(argv[1], "rb");

	uint8_t header[HEADER_SZ];
	fread(header, 1, HEADER_SZ, imgf);

	size_t buffsz = conf.indims.x * conf.indims.y * 3;

	uint8_t* buff = malloc(sizeof(uint8_t) * buffsz);
	fread(buff, 1, buffsz, imgf);

	if(buff != NULL){
		fd_result_t res;

		assert(fd_get_result_sync(0, (void* )buff, &res) == fd_ok);
		printf("num food items: %d\n", res.num_fooditems);

		for(int i =0;i<res.num_fooditems;i++){
			printf("food name: %s, \t food position: %d, %d\tconfidence(norm): %lf\n", res.fooditems[i].food_name, res.fooditems[i].food_pos.x, res.fooditems[i].food_pos.y, res.fooditems[i].__confidence);

		}
	}

	 free(buff);
	fclose(imgf);	
	fd_shutdown();

	return 0;
}
