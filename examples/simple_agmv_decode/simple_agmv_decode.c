/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_agmv_decode.c
*   Date: 7/31/2024
*   Version: 2.0
*   Updated: 7/31/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	AGMV_DecodeAGMV("gba_commercial.agmv",AGMV_IMG_BMP,AGMV_AUDIO_WAV);

	return 0;
}