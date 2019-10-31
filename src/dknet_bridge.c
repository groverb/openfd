#include "darknet.h"
#include <fd/fd_types.h>

static network* _net = NULL; 

fd_status init_dknet_bridge(){

	char cfgfile[100], weightsfile[100];
	sprintf(cfgfile, "%s/", _g_cofig->docpath);
	sprintf(weightsfile, "%s/", _g_cofig->docpath);

	net = load_network(cfgfile, weightsfile, 0);
	set_batch_network(net, 1);

	return fd_ok;
}


fd_status dknet_eval(float* buffer, size_t buff_len, fd_result_t* result){ // normalized buffer
	float* buffer = calloc(sizeof(float) * 416 * 416);
	if(_net != NULL){
		network_predict(net, buffer);
		int nboxes = 0;
		detections* pred = get_network_boxes(net, DKNET_INPUT_H, DKNET_INPUT_W, DKNET_THRESHOLD, DKNET_HIER_THRESH, 0,1 &nboxes);
#if dbglv1
		draw_detections();
#endif
		dknet_prepare_result(pred, result);
	}
}


food_pos_t dknet_get_food_pos(detections* dets){
	food_pos_t* ret = malloc(sizeof(food_pos_t));
	int i,j;
	for(i = 0; i < num; ++i){
		char labelstr[4096] = {0};
		int class = -1;
		for(j = 0; j < classes; ++j){
			if (dets[i].prob[j] > thresh){
				if (class < 0) {
					strcat(labelstr, names[j]);
					class = j;
				} else {
					strcat(labelstr, ", ");
					strcat(labelstr, names[j]);
				}
#if dbgl1
				printf("%s: %.0f%%\n", names[j], dets[i].prob[j]*100);
#endif

			}
		}
		if(class >= 0){
			int width = im.h * .006;

			int offset = class*123457 % classes;
			float red = get_color(2,offset,classes);
			float green = get_color(1,offset,classes);
			float blue = get_color(0,offset,classes);
			float rgb[3];

			rgb[0] = red;
			rgb[1] = green;
			rgb[2] = blue;
			box b = dets[i].bbox;

			int left  = (b.x-b.w/2.)*im.w;
			int right = (b.x+b.w/2.)*im.w;
			int top   = (b.y-b.h/2.)*im.h;
			int bot   = (b.y+b.h/2.)*im.h;
#if dbgl1
			printf("bounding box: %s: %d, %d, %d, %d\n", labelstr, left, right, top, bot);            
#endif
			if(left < 0) left = 0;
			if(right > im.w-1) right = im.w-1;
			if(top < 0) top = 0;
			if(bot > im.h-1) bot = im.h-1;

			ret->pos_topright.x = top;
			ret->pos_topright.y = right;

			ret->pos_bottomleft.x = bot;
			ret->pos_topright.y = left;

			strcpy(ret->food_name, labelstr); 
		}
	}
}
