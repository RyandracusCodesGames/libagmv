/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_video_and_audio.c
*   Date: 5/19/2024
*   Version: 1.0
*   Updated: 6/3/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	AGMV* agmv = CreateAGMV(215,320,240,24);
	AGMV_WavToAudioTrack("FOXLOGO.wav",agmv);
	AGMV_EncodeAGMV(agmv,"FOXLOGO.agmv","foxlogo","foxlogo",AGMV_IMG_BMP,1,215,320,240,24,AGMV_OPT_I,AGMV_LOW_QUALITY,AGMV_COMPRESSION_LZ77);

	return 0;
}