/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_mirror.c
*   Date: 12/13/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <agidl_imgp_mirror.h>
#include <agidl_imgp_rotate.h>

void * AGIDL_MirrorImgData(void* data, u32* width, u32* height, AGIDL_CLR_FMT fmt, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		switch(mirror){
			case MIRROR_LEFT_TO_RIGHT:{
				int w = *width;
				int h = *height;
				COLOR* clrs = (COLOR*)data;
				COLOR* mirror = (COLOR*)malloc(sizeof(COLOR)*w*h);
				AGIDL_ClrMemcpy(mirror,clrs,w*h);
				AGIDL_FlipImgDataVert(mirror,w,h,fmt);
				COLOR* final = (COLOR*)malloc(sizeof(COLOR)*(w*2)*h);
				
				u32 x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR clr = AGIDL_GetClr(clrs,x,y,w,h);
						AGIDL_SetClr(final,clr,x,y,w*2,h);
					}
					for(x = w; x < w*2; x++){
						COLOR clr = AGIDL_GetClr(mirror,x-w,y,w,h);
						AGIDL_SetClr(final,clr,x,y,w*2,h);
					}
				}
				
				*width = w*2;
				
				free(clrs);
				
				return final;
			}break;
			case MIRROR_RIGHT_TO_LEFT:{
				int w = *width;
				int h = *height;
				COLOR* clrs = (COLOR*)data;
				COLOR* mirror = (COLOR*)malloc(sizeof(COLOR)*w*h);
				AGIDL_ClrMemcpy(mirror,clrs,w*h);
				AGIDL_FlipImgDataVert(mirror,w,h,fmt);
				COLOR* final = (COLOR*)malloc(sizeof(COLOR)*(w*2)*h);
				
				u32 x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR clr = AGIDL_GetClr(mirror,x,y,w,h);
						AGIDL_SetClr(final,clr,x,y,w*2,h);
					}
					for(x = w; x < w*2; x++){
						COLOR clr = AGIDL_GetClr(clrs,x-w,y,w,h);
						AGIDL_SetClr(final,clr,x,y,w*2,h);
					}
				}
				
				*width = w*2;
				
				free(clrs);
				
				return final;
			}break;
			case MIRROR_TOP_TO_BOTTOM:{
				int w = *width;
				int h = *height;
				COLOR* clrs = (COLOR*)data;
				COLOR* mirror = (COLOR*)malloc(sizeof(COLOR)*w*h);
				AGIDL_ClrMemcpy(mirror,clrs,w*h);
				AGIDL_FlipImgDataHorz(mirror,w,h,fmt);
				COLOR* final = (COLOR*)malloc(sizeof(COLOR)*w*(h*2));
				
				u32 x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR clr = AGIDL_GetClr(clrs,x,y,w,h);
						AGIDL_SetClr(final,clr,x,y,w,h*2);
					}
				}
				
				for(y = h; y < h*2; y++){
					for(x = 0; x < w; x++){
						COLOR clr = AGIDL_GetClr(mirror,x,y-h,w,h);
						AGIDL_SetClr(final,clr,x,y,w,h*2);
					}
				}
				
				*height = h*2;
				
				free(clrs);
				
				return final;
			}break;
			case MIRROR_BOTTOM_TO_TOP:{
				int w = *width;
				int h = *height;
				COLOR* clrs = (COLOR*)data;
				COLOR* mirror = (COLOR*)malloc(sizeof(COLOR)*w*h);
				AGIDL_ClrMemcpy(mirror,clrs,w*h);
				AGIDL_FlipImgDataHorz(mirror,w,h,fmt);
				COLOR* final = (COLOR*)malloc(sizeof(COLOR)*w*(h*2));
				
				u32 x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR clr = AGIDL_GetClr(mirror,x,y,w,h);
						AGIDL_SetClr(final,clr,x,y,w,h*2);
					}
				}
				
				for(y = h; y < h*2; y++){
					for(x = 0; x < w; x++){
						COLOR clr = AGIDL_GetClr(final,x,y-h,w,h);
						AGIDL_SetClr(final,clr,x,y,w,h*2);
					}
				}
				
				*height = h*2;
				
				free(clrs);
				
				return final;
			}break;
		}
	}
	else{
		switch(mirror){
			case MIRROR_LEFT_TO_RIGHT:{
				int w = *width;
				int h = *height;
				COLOR16* clrs = (COLOR16*)data;
				COLOR16* mirror = (COLOR16*)malloc(sizeof(COLOR16)*w*h);
				AGIDL_ClrMemcpy16(mirror,clrs,w*h);
				AGIDL_FlipImgDataVert(mirror,w,h,fmt);
				COLOR16* final = (COLOR16*)malloc(sizeof(COLOR16)*(w*2)*h);
				
				u32 x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR16 clr = AGIDL_GetClr16(clrs,x,y,w,h);
						AGIDL_SetClr16(final,clr,x,y,w*2,h);
					}
					for(x = w; x < w*2; x++){
						COLOR16 clr = AGIDL_GetClr16(mirror,x-w,y,w,h);
						AGIDL_SetClr16(final,clr,x,y,w*2,h);
					}
				}
				
				*width = w*2;
				
				free(clrs);
				
				return final;
			}break;
			case MIRROR_RIGHT_TO_LEFT:{
				int w = *width;
				int h = *height;
				COLOR16* clrs = (COLOR16*)data;
				COLOR16* mirror = (COLOR16*)malloc(sizeof(COLOR16)*w*h);
				AGIDL_ClrMemcpy16(mirror,clrs,w*h);
				AGIDL_FlipImgDataVert(mirror,w,h,fmt);
				COLOR16* final = (COLOR16*)malloc(sizeof(COLOR16)*(w*2)*h);
				
				u32 x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR16 clr = AGIDL_GetClr16(mirror,x,y,w,h);
						AGIDL_SetClr16(final,clr,x,y,w*2,h);
					}
					for(x = w; x < w*2; x++){
						COLOR16 clr = AGIDL_GetClr16(clrs,x-w,y,w,h);
						AGIDL_SetClr16(final,clr,x,y,w*2,h);
					}
				}
				
				*width = w*2;
				
				free(clrs);
				
				return final;
			}break;
			case MIRROR_TOP_TO_BOTTOM:{
				int w = *width;
				int h = *height;
				COLOR16* clrs = (COLOR16*)data;
				COLOR16* mirror = (COLOR16*)malloc(sizeof(COLOR16)*w*h);
				AGIDL_ClrMemcpy16(mirror,clrs,w*h);
				AGIDL_FlipImgDataHorz(mirror,w,h,fmt);
				COLOR16* final = (COLOR16*)malloc(sizeof(COLOR16)*w*(h*2));
				
				u32 x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR16 clr = AGIDL_GetClr16(clrs,x,y,w,h);
						AGIDL_SetClr16(final,clr,x,y,w,h*2);
					}
				}
				
				for(y = h; y < h*2; y++){
					for(x = 0; x < w; x++){
						COLOR16 clr = AGIDL_GetClr16(mirror,x,y-h,w,h);
						AGIDL_SetClr16(final,clr,x,y,w,h*2);
					}
				}
				
				*height = h*2;
				
				free(clrs);
				
				return final;
			}break;
			case MIRROR_BOTTOM_TO_TOP:{
				int w = *width;
				int h = *height;
				COLOR16* clrs = (COLOR16*)data;
				COLOR16* mirror = (COLOR16*)malloc(sizeof(COLOR16)*w*h);
				AGIDL_ClrMemcpy16(mirror,clrs,w*h);
				AGIDL_FlipImgDataHorz(mirror,w,h,fmt);
				COLOR16* final = (COLOR16*)malloc(sizeof(COLOR16)*w*(h*2));
				
				u32 x,y;
				for(y = 0; y < h; y++){
					for(x = 0; x < w; x++){
						COLOR16 clr = AGIDL_GetClr16(mirror,x,y,w,h);
						AGIDL_SetClr16(final,clr,x,y,w,h*2);
					}
				}
				
				for(y = h; y < h*2; y++){
					for(x = 0; x < w; x++){
						COLOR16 clr = AGIDL_GetClr16(final,x,y-h,w,h);
						AGIDL_SetClr16(final,clr,x,y,w,h*2);
					}
				}
				
				*height = h*2;
				
				free(clrs);
				
				return final;
			}break;
		}
	}
	
	return NULL;
}