/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_grayscale.c
*   Date: 11/27/2023
*   Version: 0.1b
*   Updated: 1/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include "agidl_imgp_grayscale.h"
#include "agidl_cc_mixer.h"
#include "agidl_img_types.h"

void AGIDL_GrayscaleImgData(void *data, u32 width, u32 height, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* clrdata = (COLOR*)data;
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				AGIDL_SetClr(clrdata,AGIDL_GrayscaleColor(AGIDL_GetClr(clrdata,x,y,width,height),fmt),x,y,width,height);
			}
		}
	}
	else{
		COLOR16* clrdata = (COLOR16*)data;
		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){
				AGIDL_SetClr16(clrdata,AGIDL_GrayscaleColor(AGIDL_GetClr16(clrdata,x,y,width,height),fmt),x,y,width,height);
			}
		}
	}
}
