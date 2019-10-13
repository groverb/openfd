#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "image.h"
#include "fd_types.h"
#include "qdbmp.h"
#include "linked_list.h"


static int get_fc(image* ctx, __int2 windims){
	return (1 + (ctx->dims.x - windims.x) * (1 + (ctx->dims.y - windims.y)));
}


static image* create_window(image* ctx, __int2 windims, __int2 offset){
	image* ret = make_image(windims);

	size_t winsz = windims.x * windims.y;

	for(int i = offset.x, pt = 0; i < offset.x + (ctx->dims.y * windims.y- 1); 
			i+=ctx->dims.x){

		for(int j = i; j< i + windims.y; j++){
		
			ret->data[pt]->r = ctx->data[j]->r;
			ret->data[pt]->g = ctx->data[j]->g;
			ret->data[pt]->b = ctx->data[j]->b;
			pt++;

		}
	}
	return ret;
}

list* sw_get_frames(image* ctx, __int2 windims, int step){
	list* ret = make_list();
	size_t imgsz = ctx->dims.x * ctx->dims.y;
	size_t winsz = windims.x * windims.y;

	int fc = get_fc(ctx, windims);
	printf("fc: %d\n", fc);

	if(fc == 0){
		image* ctx_cp = imagecp(ctx);
		push_back(ret, ctx_cp, 'u');
	}
	else{
		for(int i = 0, c= 0;i< (ctx->dims.y ) - windims.y; i++){
			for(int j = 0;j< (ctx->dims.x ) - windims.x; j++){
				if(c++ % step == 0){
					__int2 offset = {.x = i * ctx->dims.x + j, .y = 0 };

				push_back(ret, create_window(ctx, windims, offset), 'u');
				}
			}
		}

	}

done:
	return ret;

}

static void foreach_frame(list* l){
	int i = 0;
	if(l != NULL){
		node* cur = l->HEAD;
		while(cur != NULL){
			char fname[10];
			sprintf(fname, "%d.bmp", i);
			imagewrite((image*)(cur->val), fname);
			cur = cur->next;
			i++;
		}
	}

}

// test
int main(int argc, char** argv){

	BMP* inp = BMP_ReadFile(argv[1]);
	__int2 inpdims = {.x = BMP_GetWidth(inp), .y = BMP_GetHeight(inp)};

	image* img1 = btoimg(inp->Data, inpdims); // make_image(inp->Data, inpdims);
	__int2 offset = {.x = 10, .y = 0};
	__int2 windims = {200, 200};

	image* win = create_window(img1, windims, offset);

	imagewrite(win,argv[2]);

	list* frames = sw_get_frames(img1, windims, 30);
	printf("finished with %ld frames\n", frames->size);

	foreach_frame(frames);
	
	free_list(frames);
	free(img1);
	return 0;
}


