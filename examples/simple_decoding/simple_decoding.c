/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_decoding.c
*   Date: 5/19/2024
*   Version: 1.0
*   Updated: 5/30/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	AGMV_DecodeAGMV("FOXLOGO.agmv",AGMV_IMG_BMP,AGMV_AUDIO_WAV);

	return 0;
}