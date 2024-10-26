/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: convert_agmv_to_avi.c
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
	AGMV_ConvertAGMVtoAGVI("gba_commerical.agmv");

	return 0;
}