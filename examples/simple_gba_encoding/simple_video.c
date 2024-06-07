/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_video.c
*   Date: 6/7/2024
*   Version: 1.0
*   Updated: 6/7/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv.h>

int main(){
	u32 num_of_frames = 265;
	AGMV_ExportRaw8PCM("argonaut.raw",num_of_frames/2.0f); /* IF AGMV_GBA_OPT_I OR AGMV_OPT_GBA_II DIVDE BY 2, IF AGMV_OPT_GBA_III MULTIPLY BY 0.75*/
	AGMV_EncodeVideo("argonaut.agmv","argonaut","agmv_frame_",AGMV_IMG_BMP,1,num_of_frames,320,240,30,AGMV_OPT_GBA_I,AGMV_LOW_QUALITY,AGMV_LZSS_COMPRESSION);
	
	return 0;
}