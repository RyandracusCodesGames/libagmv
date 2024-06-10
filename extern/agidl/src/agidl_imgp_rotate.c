/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_rotate.c
*   Date: 12/9/2023
*   Version: 0.2b
*   Updated: 1/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_imgp_rotate.h>

#include <stdlib.h>

#include <agidl_img_bmp.h>

void AGIDL_FlipImgDataHorz(void* data, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* flip = malloc(sizeof(COLOR)*width*height);
		COLOR* clrs = data;

		for(u32 y = 0; y < height; y++){
			for(u32 x = 0; x < width; x++){
				const COLOR clr = AGIDL_GetClr(clrs,x,(height-1)-y,width,height);
				AGIDL_SetClr(flip,clr,x,y,width,height);
			}
		}
		
		AGIDL_ClrMemcpy(clrs,flip,width*height);
		free(flip);
	}
	else{
		COLOR16* flip = malloc(sizeof(COLOR16)*width*height);
		COLOR16* clrs = data;

		for(u32 y = 0; y < height; y++){
			for(u32 x = 0; x < width; x++){
				const COLOR16 clr = AGIDL_GetClr16(clrs,x,(height-1)-y,width,height);
				AGIDL_SetClr16(flip,clr,x,y,width,height);
			}
		}
		
		AGIDL_ClrMemcpy16(clrs,flip,width*height);
		free(flip);
	}
}

void AGIDL_FlipImgDataVert(void* data, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* flip = malloc(sizeof(COLOR)*width*height);
		COLOR* clrs = data;

		for(u32 y = 0; y < height; y++){
			for(u32 x = 0; x < width; x++){
				const COLOR clr = AGIDL_GetClr(clrs,(width-1)-x,y,width,height);
				AGIDL_SetClr(flip,clr,x,y,width,height);
			}
		}
		
		AGIDL_ClrMemcpy(clrs,flip,width*height);
		free(flip);
	}
	else{
		COLOR16* flip = malloc(sizeof(COLOR16)*width*height);
		COLOR16* clrs = data;

		for(u32 y = 0; y < height; y++){
			for(u32 x = 0; x < width; x++){
				const COLOR16 clr = AGIDL_GetClr16(clrs,(width-1)-x,y,width,height);
				AGIDL_SetClr16(flip,clr,x,y,width,height);
			}
		}
		
		AGIDL_ClrMemcpy16(clrs,flip,width*height);
		free(flip);
	}
}

void AGIDL_FlipImgDataHorzAndVert(void* data, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* flip = malloc(sizeof(COLOR)*width*height);
		COLOR* clrs = data;

		for(u32 y = 0; y < height; y++){
			for(u32 x = 0; x < width; x++){
				const COLOR clr = AGIDL_GetClr(clrs,(width-1)-x,(height-1)-y,width,height);
				AGIDL_SetClr(flip,clr,x,y,width,height);
			}
		}
		
		AGIDL_ClrMemcpy(clrs,flip,width*height);
		free(flip);
	}
	else{
		COLOR16* flip = malloc(sizeof(COLOR16)*width*height);
		COLOR16* clrs = data;

		for(u32 y = 0; y < height; y++){
			for(u32 x = 0; x < width; x++){
				const COLOR16 clr = AGIDL_GetClr16(clrs,(width-1)-x,(height-1)-y,width,height);
				AGIDL_SetClr16(flip,clr,x,y,width,height);
			}
		}
		
		AGIDL_ClrMemcpy16(clrs,flip,width*height);
		free(flip);
	}
}

void AGIDL_RotateImgData(void* data, u16* width, u16* height, const AGIDL_CLR_FMT fmt, const AGIDL_ANGLE angle){
	switch(angle){
		case ANGLE_180_DEG:{
			AGIDL_FlipImgDataHorz(data,*width,*height,fmt);
		}break;
		case ANGLE_90_DEG:{
			if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
				const u16 w = *width, h = *height;
				const u16 wr = *height, hr = *width;
				COLOR* clrs = data;
				COLOR* rotate = malloc(sizeof(COLOR)*w*h);

				for(u16 y = 0; y < h; y++){
					for(u16 x = 0; x < w; x++){
						const COLOR clr = AGIDL_GetClr(clrs,x,y,w,h);
						AGIDL_SetClr(rotate,clr,y,x,wr,hr);
					}
				}
				
				*width = wr;
				*height = hr;

				AGIDL_ClrMemcpy(clrs,rotate,w*h);
				free(rotate);
			}
			else{
				const u16 w = *width, h = *height;
				const u16 wr = *height, hr = *width;
				COLOR16* clrs = data;
				COLOR16* rotate = malloc(sizeof(COLOR16)*w*h);

				for(u16 y = 0; y < h; y++){
					for(u16 x = 0; x < w; x++){
						const COLOR16 clr = AGIDL_GetClr16(clrs,x,y,w,h);
						AGIDL_SetClr16(rotate,clr,y,x,wr,hr);
					}
				}
				
				*width = wr;
				*height = hr;

				AGIDL_ClrMemcpy16(clrs,rotate,w*h);
				free(rotate);
			}
		}break;
		case ANGLE_270_DEG:{
			if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
				const u16 w = *width, h = *height;
				const u16 wr = *height, hr = *width;
				COLOR* clrs = data;
				COLOR* rotate = malloc(sizeof(COLOR16)*w*h);

				for(u16 y = 0; y < h; y++){
					for(u16 x = 0; x < w; x++){
						const COLOR clr = AGIDL_GetClr(clrs,x,y,w,h);
						AGIDL_SetClr(rotate,clr,h-y,w-x,wr,hr);
					}
				}
				
				*width = wr;
				*height = hr;
				
				AGIDL_ClrMemcpy(clrs,rotate,w*h);
				free(rotate);
			}
			else{
				const u16 w = *width, h = *height;
				const u16 wr = *height, hr = *width;
				COLOR16* clrs = data;
				COLOR16* rotate = malloc(sizeof(COLOR16)*w*h);

				for(u16 y = 0; y < h; y++){
					for(u16 x = 0; x < w; x++){
						const COLOR16 clr = AGIDL_GetClr16(clrs,x,y,w,h);
						AGIDL_SetClr16(rotate,clr,h-y-1,w-x-1,wr,hr);
					}
				}
				
				*width = wr;
				*height = hr;
				
				AGIDL_ClrMemcpy16(clrs,rotate,w*h);
				free(rotate);
			}
		}break;
	}
}
