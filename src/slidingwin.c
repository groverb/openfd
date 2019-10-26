#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slidingwin.h"

#include "qdbmp.h"
#include "linked_list.h"


static int get_fc(image* ctx, __int2 windims){
	return (1 + (ctx->dims.x - windims.x) * (1 + (ctx->dims.y - windims.y)));
}

static image* create_window(image* ctx, __int2 windims, __int2 offset){

	__int2 end = {offset.x + SW_WINDIMS_X, offset.y + SW_WINDIMS_Y};
	int startpt = offset.x * ctx->dims.x + offset.y;

	image* ret = make_image(windims);
	memcpy(&(ret->__relative_pos), &offset, sizeof(__int2));
	size_t winsz = windims.x * windims.y;

	int pt = 0;

	for(int i = startpt; i < startpt + ((ctx->dims.y * windims.y) ); 
			i+=ctx->dims.x){

		for(int j = i; j< i + windims.y; j++){
			ret->data[pt]->r = ctx->data[j]->r;
			ret->data[pt]->g = ctx->data[j]->g;
			ret->data[pt]->b = ctx->data[j]->b;
			pt++;

		}
	}
#if dbgl2
	static int __idk = 0;
	char fpath[30];
	sprintf(fpath, "bmpo/%d.bmp", __idk++);
	image_write(ret, fpath);
#endif

	return ret;
}


static image* __create_window(image* ctx, __int2 windims, __int2 offset){

	__int2 end = {offset.x + SW_WINDIMS_X, offset.y + SW_WINDIMS_Y};

	int startpt = offset.x * ctx->dims.x + offset.y;
	int endpt = (end.x * (ctx->dims.x) + end.y) -1;
	image* ret = make_image(windims);
	memcpy(&(ret->__relative_pos), &offset, sizeof(__int2));

	int c = 0, acp = 0;
	for(int i=startpt; i<endpt; i++){
		if(acp > (SW_WINDIMS_X * SW_WINDIMS_Y) -1 || i > (ctx->dims.x * ctx->dims.y)-1){ break; }

		ret->data[acp]->r = ctx->data[i]->r;
		ret->data[acp]->g = ctx->data[i]->g;
		ret->data[acp]->b = ctx->data[i]->b;
		c++;
		acp++;
		if(c == SW_WINDIMS_X ) { c = 0; i+=((ctx->dims.x - (SW_WINDIMS_X )) ); }

	}
#if dbgl2
	static int __idk = 0;
	char fpath[30];
	sprintf(fpath, "bmpo/%d.bmp", __idk++);
	image_write(ret, fpath);
#endif
	return ret;
}


list* sw_get_frames(image* ctx, __int2 windims, int step){
	list* ret = make_list();
	size_t imgsz = ctx->dims.x * ctx->dims.y;
	size_t winsz = windims.x * windims.y;

	int fc = get_fc(ctx, windims);
	if(fc == 1){
		image* ctx_cp = image_cp(ctx);

		if(ctx_cp == NULL){ printf("image_cp() returned null\n"); }
		push_back(ret, ctx_cp, 'u');
	}
	else{
		pixel blue = {0, 0, 255};
		int c = 0;

		for(int i = 0;i< (ctx->dims.x  ) - (windims.x ) ; i++){
			for(int j = 0;j< (ctx->dims.y ) -( windims.y ) ; j++){
				if(c++ % step == 0){

					__int2 offset = {j,i};
					__int2 end = {j+SW_WINDIMS_X, i+SW_WINDIMS_Y};
					if(end.x >= ctx->dims.x || end.y >= ctx->dims.y) { goto sw_done; }

					pixel blue = {0,0,255};
					push_back(ret, __create_window(ctx, windims, offset), 'u');

				}
			}
		}

	}
sw_done:
#if dbgl2
	image_write(ctx, "swdone.bmp");
#endif
	return ret;

}

static void foreach_frame(list* l){
	int i = 0;
	if(l != NULL){
		node* cur = l->HEAD;
		while(cur != NULL){
			char fname[10];
			sprintf(fname, "%d.bmp", i);
			cur = cur->next;
			i++;
		}
	}

}

#if 0
// test
int main(int argc, char** argv){

	BMP* inp = BMP_ReadFile(argv[1]);
	__int2 inpdims = {.x = BMP_GetWidth(inp), .y = BMP_GetHeight(inp)};

	image* img1 = buffer_to_image(inp->Data, inpdims); // make_image(inp->Data, inpdims);
	__int2 offset = {.x = 10, .y = 0};
	__int2 windims = {atoi(argv[3]), atoi(argv[3])};

	image* win = create_window(img1, windims, offset);

	image_write(win,argv[2]);

	list* frames = sw_get_frames(img1, windims, 30);
	printf("finished with %ld frames\n", frames->size);

	foreach_frame(frames);

	free_list(frames);
	free(img1);
	return 0;
}

#endif

