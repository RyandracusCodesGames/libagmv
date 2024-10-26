/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: simple_video_encode.c
*   Date: 7/31/2024
*   Version: 2.0
*   Updated: 8/3/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	AGMV_EncodeVideo("foxlogo.agmv","foxlogo","foxlogo",AGMV_IMG_BMP,1,215,320,240,24,AGMV_OPT_I,AGMV_MID_QUALITY,AGMV_LZSS_COMPRESSION,NULL);
	
	return 0;
}