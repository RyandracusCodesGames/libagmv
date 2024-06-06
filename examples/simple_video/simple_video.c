/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_video.c
*   Date: 5/19/2024
*   Version: 1.0
*   Updated: 6/3/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv.h>

int main(){
	
	AGMV_EncodeVideo("foxlogo.agmv","foxlogo","foxlogo",AGMV_IMG_BMP,1,215,320,240,24,AGMV_OPT_III,AGMV_LOW_QUALITY,AGMV_LZSS_COMPRESSION);
	
	return 0;
}