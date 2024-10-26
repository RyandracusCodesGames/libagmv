/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_audio_decode.c
*   Date: 7/31/2024
*   Version: 2.0
*   Updated: 7/31/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	AGMV_DecodeAudio("gba_commercial.agmv",AGMV_AUDIO_WAV);
	AGMV_DecodeAudio("gba_commercial.agmv",AGMV_AUDIO_AIFF);

	return 0;
}