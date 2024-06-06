/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_mmu_utils.c
*   Date: 2/6/2024
*   Version: 0.3b
*   Updated: 4/7/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include "agidl_img_types.h"
#include "agidl_mmu_utils.h"
#include "agidl_cc_converter.h"

void* AGIDL_AllocImgDataMMU(u32 width, u32 height, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clr = (COLOR16*)malloc(sizeof(COLOR16)*width*height);
		return clr;
	}
	else{
		COLOR* clr = (COLOR*)malloc(sizeof(COLOR)*width*height);
		return clr;
	}
	return NULL;
}

void AGIDL_ConvertRGB2RGBA(void* src, void* dest, u32 width, u32 height, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt){
	COLOR* clr_data = (COLOR*)src;
	COLOR* dest_data = (COLOR*)dest;
	
	int i;
	for(i = 0; i < width*height; i++){
		COLOR clr = clr_data[i];
		dest_data[i] = AGIDL_RGB_TO_RGBA(clr,srcfmt,destfmt);
	}
}

void AGIDL_ConvertRGBA2RGB(void* src, void* dest, u32 width, u32 height, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt){
	COLOR* clr_data = (COLOR*)src;
	COLOR* dest_data = (COLOR*)dest;
	
	int i;
	for(i = 0; i < width*height; i++){
		COLOR clr = clr_data[i];
		dest_data[i] = AGIDL_RGBA_TO_RGB(clr,srcfmt,destfmt);
	}
}

void AGIDL_ColorConvertImgData(void* src, void* dest, u32 width, u32 height, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt){
	if(srcfmt == destfmt){
		return;
	}
	
	u8 sbits = AGIDL_GetBitCount(srcfmt), dbits = AGIDL_GetBitCount(destfmt);
	
	if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		COLOR* src_data = (COLOR*)src;
		
		int i;
		for(i = 0; i < width*height; i++){
			COLOR clr = src_data[i];
			COLOR dst = AGIDL_ColorConvert(clr,srcfmt,destfmt);
			src_data[i] = dst;
		}
	}
	else if(sbits == 16 && dbits == 16){
		COLOR16* src_data = (COLOR16*)src;
		
		int i;
		for(i = 0; i < width*height; i++){
			COLOR16 clr = src_data[i];
			COLOR16 dst = AGIDL_ColorConvert(clr,srcfmt,destfmt);
			src_data[i] = dst;
		}
	}
	else if((sbits == 24 || sbits == 32) && dbits == 16){
		COLOR* src_data = (COLOR*)src;
		COLOR16* dst_data = (COLOR16*)dest;
		
		int i;
		for(i = 0; i < width*height; i++){
			COLOR clr = src_data[i];
			COLOR16 dst = AGIDL_ColorConvert(clr,srcfmt,destfmt);
			dst_data[i] = dst;
		}
	}
	else{
		COLOR16* src_data = (COLOR16*)src;
		COLOR* dst_data = (COLOR*)dest;
		
		int i;
		for(i = 0; i < width*height; i++){
			COLOR16 clr = src_data[i];
			COLOR dst = AGIDL_ColorConvert(clr,srcfmt,destfmt);
			dst_data[i] = dst;
		}
	}
}