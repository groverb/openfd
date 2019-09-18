#include <common/fd_types.h>
#include <preprocessor/downsample.h>
#include <3rdparty/qdbmp.h>

#define MAKE_TESTS


#ifdef MAKE_TESTS

int TEST(const char* in_fname, const char* out_fname){
/*
	printf("unit tests: \n");
	printf("%d", (int)(u8_max(8,17) == 17));
	printf("%d", (int)(u8_min(8,17) == 8));
	printf("%d", (int)(u8_avg(13,15) == 14));
*/
	// assuming test file is a bmp(54 bytes header)
	unsigned char header[54];

	BMP* inb = BMP_ReadFile(in_fname);
	if(inb == NULL) { printf("NULL image\n"); return -1; }
	uint8_t* buff = inb->Data;
	__int2 indims = {512,512};
	__int2 newindims = {256,256};

	uint8_t* outbuff = downsample_min(buff, indims);

	if(outbuff == NULL){
		printf("downsample() returned NULL\n");
		return -1;
	}

	BMP* outb =  BMP_Create(256,256, 24);	
	free(outb->Data);

	outb->Data = outbuff;
	//free(outpix);	
	BMP_WriteFile(outb, out_fname);	


	BMP_Free(inb);
	BMP_Free(outb);

	//free(outbuff);
	return 0;

}

int main(int argc, char** argv){
	if(argc < 3) { printf("pass args\n"); return -1; }
	printf("ushort : %lu\n, ulong: %lu\n, uchar: %lu\n", sizeof(unsigned short),sizeof(unsigned long int), sizeof(unsigned char));

	printf("\ndownsample test: \n");
	TEST(argv[1],argv[2]);
	return 0;

}
#endif

