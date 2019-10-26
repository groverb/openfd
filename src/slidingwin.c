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
			 printf("i: %d, j: %d\n", i, j);
			ret->data[pt]->r = ctx->data[j]->r;
			ret->data[pt]->g = ctx->data[j]->g;
			ret->data[pt]->b = ctx->data[j]->b;
			pt++;

		}
	}
	static int __idk = 0;
	char fpath[30];
	sprintf(fpath, "bmpo/%d.bmp", __idk++);
	image_write(ret, fpath);
	return ret;
}


static image* __create_window(image* ctx, __int2 windims, __int2 offset){

	__int2 end = {offset.x + SW_WINDIMS_X, offset.y + SW_WINDIMS_Y};

	int startpt = offset.x * ctx->dims.x + offset.y;
	int endpt = (end.x * (ctx->dims.x) + end.y) -1;
	// printf("start: %d, end: %d\n", startpt, endpt);
	image* ret = make_image(windims);
	memcpy(&(ret->__relative_pos), &offset, sizeof(__int2));

	int c = 0, acp = 0;
	for(int i=startpt; i<endpt; i++){
		if(acp > (SW_WINDIMS_X * SW_WINDIMS_Y) -1 || i > (ctx->dims.x * ctx->dims.y)-1){ break; }
		
	//	 printf("i: %d\t", i);
	// printf("start: %d, end: %d acp: %d\n", startpt, endpt, acp);
		ret->data[acp]->r = ctx->data[i]->r;
		ret->data[acp]->g = ctx->data[i]->g;
		ret->data[acp]->b = ctx->data[i]->b;
		c++;
		acp++;
		if(c == SW_WINDIMS_X ) { c = 0; i+=((ctx->dims.x - (SW_WINDIMS_X )) ); }
		
	}
	static int __idk = 0;
	char fpath[30];
	sprintf(fpath, "bmpo/%d.bmp", __idk++);
	image_write(ret, fpath);
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
		printf("beginning\n");
		int c = 0;
		
		for(int i = 0;i< (ctx->dims.x  ) - (windims.x ) ; i++){
			for(int j = 0;j< (ctx->dims.y ) -( windims.y ) ; j++){
				if(c++ % step == 0){

					__int2 offset = {j,i};
					__int2 end = {j+64, i+64};
					if(end.x >= ctx->dims.x || end.y >= ctx->dims.y) { goto done; }
					static int dc = 0;
					printf("%d drawing %d, %d " , dc++, i, j);
					printf("to %d, %d\n" , end.x, end.y);
				 //	image_draw_square(ctx, offset, end,&blue);

					push_back(ret, __create_window(ctx, windims, offset), 'u');

				}
			}
		}

	}
	image_write(ctx, "swfinished.bmp");
done:
	printf("sw done\n");
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

