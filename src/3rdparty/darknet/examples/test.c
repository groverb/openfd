#include "darknet.h"

int main(int argc, char** argv){

	char* cfgfile = argv[4];
	char* weightsfile =  argv[5];
	char* fname = argv[6];

	network* net = load_network(cfgfile, weightsfile, 0);
	set_batch_network(net, 1);

	
	network_predict(net, X);


	float* buffer = calloc(sizeof(float) * 416 * 416);
	network_predict(net, buffer);

	int nboxes = 0;
	detections* pred = get_network_boxes(net, 416,416, thresh, hier_thresh, 0,1 &nboxes);


}
