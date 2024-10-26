/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_video_and_audio.c
*   Date: 7/31/2024
*   Version: 2.0
*   Updated: 8/23/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	AGMV* agmv = CreateAGMV(1060,240,160,24);
	AGMV_WavToAudioTrack("gba_commercial.wav",agmv);
	AGMV_EncodeAGMV(agmv,"gba_commercial.agmv","gba_commercial","agmv_frame_",AGMV_IMG_BMP,1,1061,240,160,24,AGMV_OPT_IV,AGMV_HIGH_QUALITY,AGMV_LZSS_COMPRESSION);
	
	return 0;
}