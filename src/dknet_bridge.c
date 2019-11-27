#include "darknet.h"
#include <fd/fd_types.h>
#include "config.h"

#include <math.h>

#define IMAX(x,y) ((x)>(y))?(x):(y)
#define IMIN(x,y) ((x)<(y))?(x):(y)

extern void __dknetbuffer_to_fdimage(float*, __int2);

static network* _net = NULL; 
static char** names;



fd_status init_dknet_bridge(){

	char cfgfile[300], weightsfile[300], namesfile[300], datacfg[300];

	sprintf(cfgfile, "%s/yolov2-food100.cfg", _g_config->docpath);
	sprintf(namesfile, "%s/food100.names", _g_config->docpath);
	sprintf(weightsfile, "%s/yolov2-food100.weights", _g_config->docpath);
	sprintf(datacfg, "%s/food100.data", _g_config->docpath);

	list* options = read_data_cfg(datacfg);
	char* name_fdlist = option_find_str(options, "names", namesfile);
	names = get_labels(name_fdlist);

	_net = load_network(cfgfile, weightsfile, 0);
	set_batch_network(_net, 1);
	srand(2222222);

	return fd_ok;
}


static fd_status dknet_prepare_food_pos(detection* dets, int classes, int num, food_pos_t* ctx ){
	int i,j;
	double ctx_conf;
	for(i = 0; i < num; ++i){
		char labelstr[4096] = {0};
		int class = -1;
		for(j = 0; j < classes; ++j){
			if (dets[i].prob[j] > DKNET_THRESHOLD){
				if (class < 0) {
					strcat(labelstr, names[j]);
					class = j;
				} else {
					strcat(labelstr, ", ");
					strcat(labelstr, names[j]);
				}
				ctx_conf = dets[i].prob[j];
#if dbgl1
				printf("%s: %.0f%%\n", names[j], dets[i].prob[j]*100);
#endif

			}
		}
		if(class >= 0){
			box b = dets[i].bbox;

			int left  = (b.x-b.w/2.)*DKNET_INPUT_W;
			int right = (b.x+b.w/2.)*DKNET_INPUT_W;
			int top   = (b.y-b.h/2.)*DKNET_INPUT_H;
			int bot   = (b.y+b.h/2.)*DKNET_INPUT_H;
			
			left = IMAX(left, 5);
			left = IMIN(left, DKNET_INPUT_H - 5);
			
			right = IMAX(right, 5);
			right = IMIN(right, DKNET_INPUT_H - 5);

			top = IMAX(top, 5);
			top = IMIN(top, DKNET_INPUT_H - 5);

			bot= IMAX(bot, 5);
			bot = IMIN(bot, DKNET_INPUT_H - 5);


#if dbgl1
			printf("bounding box: %s: %d, %d, %d, %d\n", labelstr, left, right, top, bot);            
#endif
			/*
			if(left < 0) left = 0;
			if(right > DKNET_INPUT_W-1) right = DKNET_INPUT_W-1;
			if(top < 0) top = 0;
			if(bot > DKNET_INPUT_H-1) bot = DKNET_INPUT_H-1;
	*/

			if(ctx != NULL){
				ctx->pos_topright.x = bot;
				ctx->pos_topright.y =right;

				ctx->pos_bottomleft.x = top;
				ctx->pos_bottomleft.y = left;

				strcpy(ctx->food_name, labelstr);
				printf("assigning confidence : %lf\n", ctx_conf);
				ctx->__confidence = ctx_conf; 
				return fd_ok;
			}
			else {
				return fd_invalidargs;
			}
		}
	}
	return fd_nofood;
}


// input: 416 * 416 normalized buffer

fd_status dknet_eval(float* buffer, food_pos_t* foodpos){

	layer l = _net->layers[_net->n-1];
	__int2 _indims = {416, 416};
	__dknetbuffer_to_fdimage(buffer, _indims);
	
	image dknetimg = make_image(416, 416,3);
	size_t bufflen = sizeof(float) * 416 * 416 * 3;

	memcpy(dknetimg.data, buffer, bufflen);
	

	save_image(dknetimg, "dknet_inp.jpg");

	network_predict(_net, buffer);

	int nboxes = 0;
	detection *dets = get_network_boxes(_net, DKNET_INPUT_W, DKNET_INPUT_H, DKNET_THRESHOLD, DKNET_HIER_THRESHOLD, 0, 1, &nboxes);
	
	return dknet_prepare_food_pos(dets, l.classes, nboxes, foodpos);

}



