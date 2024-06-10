/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_grayscale.c
*   Date: 11/27/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_imgp_grayscale.h>
#include <agidl_cc_mixer.h>
#include <agidl_img_types.h>

void AGIDL_GrayscaleImgData(void *data, u32 width, u32 height, AGIDL_CLR_FMT fmt){
	int i;
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* clrdata = (COLOR*)data;

		for(i = 0; i < width*height; i++){
			clrdata[i] = AGIDL_GrayscaleColor(clrdata[i],fmt);
		}
	}
	else{
		COLOR16* clrdata = (COLOR16*)data;

		for(i = 0; i < width*height; i++){
			clrdata[i] = AGIDL_GrayscaleColor(clrdata[i],fmt);
		}
	}
}
