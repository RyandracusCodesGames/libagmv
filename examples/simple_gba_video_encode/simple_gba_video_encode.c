/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_gba_video_encode.c
*   Date: 7/31/2024
*   Version: 2.0
*   Updated: 8/25/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	/*AUDIO TRACK MUST BE MONO AUDIO SAMPLED BETWEEN 8KHZ AND 32KHZ FOR THE GBA SOUND HARDWARE*/
	/*MUST USE A GBA OPTIMIZATION FLAG TO FINE TUNE CODEC FOR GBA VIDEO OUTPUT*/
	
	AGMV* agmv = CreateAGMV(383,320,240,30);
	AGMV_WavToAudioTrack("after_credits.wav",agmv);
	AGMV_EncodeAGMV(agmv,"after_credits.agmv","after_credits","agmv_frame_",AGMV_IMG_BMP,1,383,320,240,30,AGMV_OPT_GBA_I,AGMV_HIGH_QUALITY,AGMV_LZSS_COMPRESSION);
	
	return 0;
}