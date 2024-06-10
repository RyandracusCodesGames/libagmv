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

#include <agidl_mmu_utils.h>

#include <stdlib.h>

#include <agidl_cc_converter.h>
#include <agidl_img_types.h>

void* AGIDL_AllocImgDataMMU(const u32 width, const u32 height, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clr = malloc(sizeof(COLOR16)*width*height);
		return clr;
	}
	COLOR* clr = malloc(sizeof(COLOR)*width*height);
	return clr;
	return NULL;
}

void AGIDL_ConvertRGB2RGBA(const void* src, void* dest, const u32 width, const u32 height, const AGIDL_CLR_FMT srcfmt, const AGIDL_CLR_FMT destfmt){
	const COLOR* clr_data = src;
	COLOR* dest_data = dest;

	for(int i = 0; i < width*height; i++){
		const COLOR clr = clr_data[i];
		dest_data[i] = AGIDL_RGB_TO_RGBA(clr,srcfmt,destfmt);
	}
}

void AGIDL_ConvertRGBA2RGB(const void* src, void* dest, const u32 width, const u32 height, const AGIDL_CLR_FMT srcfmt, const AGIDL_CLR_FMT destfmt){
	const COLOR* clr_data = src;
	COLOR* dest_data = dest;

	for(int i = 0; i < width*height; i++){
		const COLOR clr = clr_data[i];
		dest_data[i] = AGIDL_RGBA_TO_RGB(clr,srcfmt,destfmt);
	}
}

void AGIDL_ColorConvertImgData(void* src, void* dest, const u32 width, const u32 height, const AGIDL_CLR_FMT srcfmt, const AGIDL_CLR_FMT destfmt){
	if(srcfmt == destfmt){
		return;
	}

	const u8 sbits = AGIDL_GetBitCount(srcfmt), dbits = AGIDL_GetBitCount(destfmt);
	
	if((sbits == 24 || sbits == 32) && (dbits == 24 || dbits == 32)){
		COLOR* src_data = src;

		for(int i = 0; i < width*height; i++){
			const COLOR clr = src_data[i];
			const COLOR dst = AGIDL_ColorConvert(clr,srcfmt,destfmt);
			src_data[i] = dst;
		}
	}
	else if(sbits == 16 && dbits == 16){
		COLOR16* src_data = src;

		for(int i = 0; i < width*height; i++){
			const COLOR16 clr = src_data[i];
			const COLOR16 dst = AGIDL_ColorConvert(clr,srcfmt,destfmt);
			src_data[i] = dst;
		}
	}
	else if((sbits == 24 || sbits == 32) && dbits == 16){
		const COLOR* src_data = src;
		COLOR16* dst_data = dest;

		for(int i = 0; i < width*height; i++){
			const COLOR clr = src_data[i];
			const COLOR16 dst = AGIDL_ColorConvert(clr,srcfmt,destfmt);
			dst_data[i] = dst;
		}
	}
	else{
		const COLOR16* src_data = src;
		COLOR* dst_data = dest;

		for(int i = 0; i < width*height; i++){
			const COLOR16 clr = src_data[i];
			const COLOR dst = AGIDL_ColorConvert(clr,srcfmt,destfmt);
			dst_data[i] = dst;
		}
	}
}
