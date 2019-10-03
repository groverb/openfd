#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "fd_types.h"
#include "qdbmp.h"
#include "linked_list.h"

#define DBG 1 


static int get_possible_fcount(const __int3* buffdims, const __int3* windims){
	return (1 + (buffdims->x - windims->x))  * (1 + (buffdims->y - windims->y));
}

static uint8_t* create_window(uint8_t* buff, __int2 offset,  __int3 buffdims, __int3 windims){

	uint8_t* window = calloc(windims.x * windims.y * windims.z , sizeof(uint8_t));
	for(int i =0;i<windims.x * windims.y * windims.z; i++){
		window[i] = 255;
	}

	int t = 0; //c=0; //windims.x * windims.y * windims.z;
	// bool marked = 1;
	for(int i = offset.x ; i< offset.x +(buffdims.x * windims.y * windims.z );
			i+=buffdims.x * buffdims.z ){
		for(int j=i ;j< i + (windims.x * 3);j+=windims.z){
			// if(marked){  buff[j+1] = 255;}

			window[t++] = buff[ j];
			window[t++] = buff[ j +1];
			window[t++] = buff[ j + 2];
		}
		//marked = false;
	}
	return window;
}

list* sw_get_frames(uint8_t* buff, __int3 buffdims, __int3 windims, int step){

	list* ret_flist = make_list();
	size_t buffsz = buffdims.x * buffdims.y * buffdims.z;
	size_t winsz = windims.x * windims.y * windims.z;

	int fcount = get_possible_fcount(&buffdims, &windims);
	uint8_t* ret_buff = NULL;

	if(fcount == 0){
		ret_buff = malloc(buffsz * sizeof(uint8_t));
		memcpy(ret_buff, buff, buffsz);
		push_back(ret_flist, ret_buff,'u');	
	} 
	else {
		// list* frames = make_list();

		// sliding window
		int fc = 0;
		for(int i =0;i<(buffdims.x * buffdims.z) -( windims.x * windims.z) ;i+=buffdims.z){
			for(int j =0;j<(buffdims.y * buffdims.z) -( windims.y * windims.z) ;j+=buffdims.z){
				__int2 offset = {.x  = i * (buffdims.y *3) +j ,.y = 0};
				fc++;
				if(fc == fcount) { goto done; }

				if(fc % step == 0){
					push_back(ret_flist, create_window(buff,offset, buffdims, windims),'u');	
				}
			}
		}
		printf("fc: %d, fcount: %d\n", fc,fcount);
	}
done:
	return ret_flist;
}


// test
int main(int argc, char** argv){

	BMP* inp = BMP_ReadFile(argv[1]);

	__int3 inpdims = {.x = BMP_GetHeight(inp), .y = BMP_GetWidth(inp), .z = 3};
	printf("inpdims: %d, %d \n", inpdims.x, inpdims.y);
	__int3 windims = {.x = 256, .y = 256, .z = 3};

	__int2 temp_off = {.x = 73728+600, .y = 0};
	
	
	list* newf5 = sw_get_frames(inp->Data,  inpdims, windims, 50 );

#if DBG
	node* cur = newf5->HEAD;
	int fc = 0;
	while(cur != NULL){

		BMP* temp = BMP_Create(256, 256, 24);
		free(temp->Data);
		temp->Data = cur->val;
		char fname[20];
		sprintf(fname, "./bmpout/%d.bmp", fc);
		BMP_WriteFile(temp, fname);
		cur = cur->next;
		fc++;
		BMP_Free(temp);
	}
#endif
	
	free_list(newf5);
	BMP_Free(inp);
	
	return 0;
}


