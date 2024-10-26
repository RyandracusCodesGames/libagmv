/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_video_decode.c
*   Date: 7/31/2024
*   Version: 2.0
*   Updated: 7/31/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	AGMV_DecodeVideo("foxlogo.agmv",AGMV_IMG_BMP);
	
	return 0;
}