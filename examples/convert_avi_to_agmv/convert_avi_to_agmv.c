/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: convert_avi_to_agmv.c
*   Date: 8/13/2024
*   Version: 2.0
*   Updated: 8/13/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agmv.h>

int main(){
	
	AGMV_EnableDither();
	AGMV_ConvertAVItoAGMV("hill.avi",AGMV_OPT_I,AGMV_HIGH_QUALITY,AGMV_NONE_COMPRESSION);

	return 0;
}