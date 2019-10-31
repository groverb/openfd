#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define USE_DARKNET 1
#define USE_VGG16 0

#if USE_DARKNET
#include "darknet.h"
#endif

#include <fd/fd_core.h>
#define HEADER_SZ 122 

#if USE_DARKNET
extern void __fd_get_result_dknet(network* net, list* options, image** alphabet, char* names_list, char** names, char *filename, float thresh, float hier_thresh, char *outfile, int fullscreen);

#endif

int main(int argc, char** argv){
	fd_config_t conf = {.format = rgb_uint8};
	conf.indims.x= atoi(argv[2]); 
	conf.indims.y=  atoi(argv[3]);
	// conf.docpath = malloc(100 * sizeof(char));	
	sprintf(conf.docpath, "%s", argv[4]);

#if USE_DARKNET
	char* datacfg = argv[4];
	char* namesfile = argv[5];
	char* weightsfile = argv[6];
	char* cfgfile = argv[7];
	
	list* options = read_data_cfg(datacfg);
	char* name_list = option_find_str(options, "names", namesfile);
	char** names = get_labels(name_list);
	image** abet = load_alphabet();
	network* net = load_network(cfgfile, weightsfile, 0);
	set_batch_network(net, 1);
	srand(2222222);


#else
	assert(fd_init(conf) == fd_ok);

#endif
	while(1){
		char usr_input[200];
		printf("enter [filename] [height] [width]\n");

		scanf("%s %d %d", usr_input, &(conf.indims.x), &(conf.indims.y) );

#if USE_VGG16
		assert(fd_configure_input(conf) == fd_ok);
		FILE* imgf;
		imgf = fopen(usr_input, "rb");

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
#elif USE_DARKNET

		__fd_get_result_dknet(net, options, abet, name_list, names,  usr_input, 0.5, 0.5,"fd_output", 0);
#endif
	}

	fd_shutdown();
	return 0;
}


