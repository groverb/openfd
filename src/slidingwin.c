#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slidingwin.h"

#include "qdbmp.h"
#include "fdlist.h"


static inline int get_fc(fdimage* ctx, __int2 windims){
	return (1 + (ctx->dims.x - windims.x) * (1 + (ctx->dims.y - windims.y)));
}

/* 
 * Provided the buffer offset, generaes 
 * a frame of size SW_WINDIMS_H * SW_WINDIMS_W
 * gets called in a loop from sw_get_frames()
 */
static fdimage* create_window(fdimage* ctx, __int2 windims, __int2 offset){

	__int2 end = {offset.x + SW_WINDIMS_X, offset.y + SW_WINDIMS_Y};

	int startpt = offset.x * ctx->dims.x + offset.y;
	int endpt = (end.x * (ctx->dims.x) + end.y) -1;
	fdimage* ret = make_fdimage(windims);
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
	fdimage_write(ret, fpath);
#endif
	return ret;
}

/* 
 * core function to produce frames from an 
 * image based on sliding window algorithm
 */
fdlist* sw_get_frames(fdimage* ctx, __int2 windims, int step){
	fdlist* ret = make_fdlist();

	int fc = get_fc(ctx, windims);
	if(fc == 1){
		fdimage* ctx_cp = fdimage_cp(ctx);

		if(ctx_cp == NULL){ printf("fdimage_cp() returned null\n"); }
		push_back(ret, ctx_cp, 'u');
	}
	else{
		int c = 0;

		for(int i = 0;i< (ctx->dims.x  ) - (windims.x ) ; i++){
			for(int j = 0;j< (ctx->dims.y ) -( windims.y ) ; j++){
				if(c++ % step == 0){

					__int2 offset = {j,i};
					__int2 end = {j+SW_WINDIMS_X, i+SW_WINDIMS_Y};
					if(end.x >= ctx->dims.x || end.y >= ctx->dims.y) { goto sw_done; }
					push_back(ret, create_window(ctx, windims, offset), 'u');

				}
			}
		}

	}
sw_done:
#if dbgl2
	fdimage_write(ctx, "swdone.bmp");
#endif
	return ret;

}

