/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_video.c
*   Date: 6/7/2024
*   Version: 1.1
*   Updated: 6/13/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv.h>

int main(){
	u32 num_of_frames = 265;
	
	AGMV* agmv = CreateAGMV(num_of_frames-1,320,240,30);
	AGMV_RawSignedPCMToAudioTrack("argonaut.raw",agmv,1,16000);
	AGMV_EncodeAGMV(agmv,"argonaut.agmv","argonaut","agmv_frame_",AGMV_IMG_BMP,1,num_of_frames,320,240,30,AGMV_OPT_GBA_I,AGMV_LOW_QUALITY,AGMV_LZSS_COMPRESSION);
	
	return 0;
}