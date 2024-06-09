/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_scale.c
*   Date: 12/9/2023
*   Version: 0.2b
*   Updated: 2/14/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <string.h>
#include "agidl_imgp_scale.h"

#include <tgmath.h>

#include "agidl_cc_mixer.h"
#include "agidl_math_utils.h"

void * AGIDL_HalfImgDataNearest(void* data, u16* width, u16* height, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		u16 worg = *width;
		u16 horg = *height;
		
		u16 w = worg >> 1;
		u16 h = horg >> 1;
		
		COLOR16* org_data = (COLOR16*)data;
		COLOR16* clr_data = (COLOR16*)malloc(sizeof(COLOR16)*w*h);
		
		u16 x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				u16 x2 = (x << 1);
				u16 y2 = (y << 1);
				
				COLOR16 clr = AGIDL_GetClr16(org_data,x2,y2,worg,horg);
				AGIDL_SetClr16(clr_data,clr,x,y,w,h);
			}
		}
		
		free(data);
		
		*width = w;
		*height = h;
		
		return clr_data;
	}
	else{
		u16 worg = *width;
		u16 horg = *height;
		
		u16 w = worg >> 1;
		u16 h = horg >> 1;
		
		COLOR* org_data = (COLOR*)data;
		COLOR* clr_data = (COLOR*)malloc(sizeof(COLOR)*w*h);
		
		u16 x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				u16 x2 = (x << 1);
				u16 y2 = (y << 1);
				
				COLOR clr = AGIDL_GetClr(org_data,x2,y2,worg,horg);
				AGIDL_SetClr(clr_data,clr,x,y,w,h);
			}
		}
		
		free(data);
		
		*width = w;
		*height = h;
		
		return clr_data;
	}
}

void * AGIDL_DoubleImgDataNearest(void* data, u16* width, u16* height, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		u16 worg = *width;
		u16 horg = *height;
		
		u16 w = worg << 1;
		u16 h = horg << 1;
		
		COLOR16* org_data = (COLOR16*)data;
		COLOR16* clr_data = (COLOR16*)malloc(sizeof(COLOR16)*w*h);
		
		u16 x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				u16 x2 = (x >> 1);
				u16 y2 = (y >> 1);
				
				COLOR16 clr = AGIDL_GetClr16(org_data,x2,y2,worg,horg);
				AGIDL_SetClr16(clr_data,clr,x,y,w,h);
			}
		}
		
		free(data);
		
		*width = w;
		*height = h;
		
		return clr_data;
	}
	else{
		u16 worg = *width;
		u16 horg = *height;
		
		u16 w = worg << 1;
		u16 h = horg << 1;
		
		COLOR* org_data = (COLOR*)data;
		COLOR* clr_data = (COLOR*)malloc(sizeof(COLOR)*w*h);
		
		u16 x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				u16 x2 = (x >> 1);
				u16 y2 = (y >> 1);
				
				COLOR clr = AGIDL_GetClr(org_data,x2,y2,worg,horg);
				AGIDL_SetClr(clr_data,clr,x,y,w,h);
			}
		}
		
		free(data);
		
		*width = w;
		*height = h;
		
		return clr_data;
	}
}

void * AGIDL_HalfImgDataBilerp(void* data, u16* width, u16* height, AGIDL_CLR_FMT fmt){
		if(AGIDL_GetBitCount(fmt) == 16){
		u16 worg = *width;
		u16 horg = *height;
		
		u16 w = worg >> 1;
		u16 h = horg >> 1;
		
		COLOR16* org_data = (COLOR16*)data;
		COLOR16* clr_data = (COLOR16*)malloc(sizeof(COLOR16)*w*h);
		
		u16 x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				u16 x2 = (x << 1);
				u16 y2 = (y << 1);
				
				COLOR16 clr1 = AGIDL_GetClr16(org_data,x2,y2,worg,horg);
				COLOR16 clr2 = AGIDL_GetClr16(org_data,x2+1,y2,worg,horg);
				COLOR16 clr3 = AGIDL_GetClr16(org_data,x2,y2+1,worg,horg);
				COLOR16 clr4 = AGIDL_GetClr16(org_data,x2+1,y2+1,worg,horg);
				
				u8 r1 = AGIDL_GetR(clr1,fmt);
				u8 g1 = AGIDL_GetG(clr1,fmt);
				u8 b1 = AGIDL_GetB(clr1,fmt);
				
				u8 r2 = AGIDL_GetR(clr2,fmt);
				u8 g2 = AGIDL_GetG(clr2,fmt);
				u8 b2 = AGIDL_GetB(clr2,fmt);
				
				u8 r3 = AGIDL_GetR(clr3,fmt);
				u8 g3 = AGIDL_GetG(clr3,fmt);
				u8 b3 = AGIDL_GetB(clr3,fmt);
				
				u8 r4 = AGIDL_GetR(clr4,fmt);
				u8 g4 = AGIDL_GetG(clr4,fmt);
				u8 b4 = AGIDL_GetB(clr4,fmt);
				
				u8 rtop = r1 + ((r2 - r1) >> 1);
				u8 gtop = g1 + ((g2 - g1) >> 1);
				u8 btop = b1 + ((b2 - b1) >> 1);
				
				u8 rbottom = r3 + ((r4 - r3) >> 1);
				u8 gbottom = g3 + ((g4 - g3) >> 1);
				u8 bbottom = b3 + ((b4 - b3) >> 1);
				
				u8 rfinal = rtop + ((rbottom - rtop) >> 1);
				u8 gfinal = gtop + ((gbottom - gtop) >> 1);
				u8 bfinal = btop + ((bbottom - btop) >> 1);
				
				AGIDL_SetClr16(clr_data,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,w,h);
			}
		}
		
		free(data);
		
		*width = w;
		*height = h;
		
		return clr_data;
	}
	else{
		u16 worg = *width;
		u16 horg = *height;
		
		u16 w = worg >> 1;
		u16 h = horg >> 1;
		
		COLOR* org_data = (COLOR*)data;
		COLOR* clr_data = (COLOR*)malloc(sizeof(COLOR)*w*h);
		
		u16 x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				u16 x2 = (x << 1);
				u16 y2 = (y << 1);
				
				COLOR clr1 = AGIDL_GetClr(org_data,x2,y2,worg,horg);
				COLOR clr2 = AGIDL_GetClr(org_data,x2+1,y2,worg,horg);
				COLOR clr3 = AGIDL_GetClr(org_data,x2,y2+1,worg,horg);
				COLOR clr4 = AGIDL_GetClr(org_data,x2+1,y2+1,worg,horg);
				
				u8 r1 = AGIDL_GetR(clr1,fmt);
				u8 g1 = AGIDL_GetG(clr1,fmt);
				u8 b1 = AGIDL_GetB(clr1,fmt);
				
				u8 r2 = AGIDL_GetR(clr2,fmt);
				u8 g2 = AGIDL_GetG(clr2,fmt);
				u8 b2 = AGIDL_GetB(clr2,fmt);
				
				u8 r3 = AGIDL_GetR(clr3,fmt);
				u8 g3 = AGIDL_GetG(clr3,fmt);
				u8 b3 = AGIDL_GetB(clr3,fmt);
				
				u8 r4 = AGIDL_GetR(clr4,fmt);
				u8 g4 = AGIDL_GetG(clr4,fmt);
				u8 b4 = AGIDL_GetB(clr4,fmt);
				
				u8 rtop = r1 + ((r2 - r1) >> 1);
				u8 gtop = g1 + ((g2 - g1) >> 1);
				u8 btop = b1 + ((b2 - b1) >> 1);
				
				u8 rbottom = r3 + ((r4 - r3) >> 1);
				u8 gbottom = g3 + ((g4 - g3) >> 1);
				u8 bbottom = b3 + ((b4 - b3) >> 1);
				
				u8 rfinal = rtop + ((rbottom - rtop) >> 1);
				u8 gfinal = gtop + ((gbottom - gtop) >> 1);
				u8 bfinal = btop + ((bbottom - btop) >> 1);
				
				AGIDL_SetClr(clr_data,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,w,h);
			}
		}
		
		free(data);
		
		*width = w;
		*height = h;
		
		return clr_data;
	}
}

void * AGIDL_ScaleImgDataNearest(void* data, u16* width, u16* height, float sx, float sy, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		u16 worg = *width;
		u16 horg = *height;
		
		COLOR* clrs = (COLOR*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR* scale = (COLOR*)malloc(sizeof(COLOR)*newwidth*newheight);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				u16 x2 = (x*xscale);
				u16 y2 = (y*yscale);
				
				COLOR clr = AGIDL_GetClr(clrs,x2,y2,worg,horg);
				AGIDL_SetClr(scale,clr,x,y,newwidth,newheight);
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);

		return scale;
	}
	else{
		u16 worg = *width;
		u16 horg = *height;
		
		COLOR16* clrs = (COLOR16*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR16* scale = (COLOR16*)malloc(sizeof(COLOR16)*newwidth*newheight);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				u16 x2 = (x*xscale);
				u16 y2 = (y*yscale);
				
				COLOR16 clr = AGIDL_GetClr16(clrs,x2,y2,worg,horg);
				AGIDL_SetClr16(scale,clr,x,y,newwidth,newheight);
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);
		
		return scale;
	}
}

void * AGIDL_ScaleImgDataBilerp(void* data, u16* width, u16* height, float sx, float sy, AGIDL_CLR_FMT fmt){
	if(floor(sx) == 0.5f && floor(sy) == 0.5f){
		return AGIDL_HalfImgDataBilerp(data,width,height,fmt);
	}
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		u16 worg = *width;
		u16 horg = *height;
		
		COLOR* clrs = (COLOR*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR* scale = (COLOR*)malloc(sizeof(COLOR)*newwidth*newheight);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		float x_diff, y_diff;
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				float x2 = (x*xscale);
				float y2 = (y*yscale);
				
				float x_floor = floor(x2);
				float y_floor = floor(y2);
				float x_ceil = ceil(x2);
				float y_ceil = ceil(y2);
				
				COLOR clr1 = AGIDL_GetClr(clrs,x_floor,y_floor,worg,horg);
				COLOR clr2 = AGIDL_GetClr(clrs,x_ceil,y_floor,worg,horg);
				COLOR clr3 = AGIDL_GetClr(clrs,x_floor,y_ceil,worg,horg);
				COLOR clr4 = AGIDL_GetClr(clrs,x_ceil,y_ceil,worg,horg);
				
				if(x2 == 0 || y2 == 0){
					clr1 = AGIDL_GetClr(clrs,x2,y2,worg,horg);
					clr2 = AGIDL_GetClr(clrs,x2+1,y2,worg,horg);
					clr3 = AGIDL_GetClr(clrs,x2,y2+1,worg,horg);
					clr4 = AGIDL_GetClr(clrs,x2+1,y2+1,worg,horg);
					
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 + ((r2-r1) >> 1);
					u8 gtop = g1 + ((g2-g1) >> 1);
					u8 btop = b1 + ((b2-b1) >> 1);
					
					u8 rbot = r3 + ((r4-r3) >> 1);
					u8 gbot = g3 + ((g4-g3) >> 1);
					u8 bbot = b3 + ((b4-b3) >> 1);
					
					u8 rfinal = rtop + ((rbot-rtop) >> 1);
					u8 gfinal = gtop + ((gbot-gtop) >> 1);
					u8 bfinal = btop + ((bbot-btop) >> 1);
					
					AGIDL_SetClr(scale,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
				}
				else{
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 * (x_ceil - x2) + r2 * (x2 - x_floor);
					u8 gtop = g1 * (x_ceil - x2) + g2 * (x2 - x_floor);
					u8 btop = b1 * (x_ceil - x2) + b2 * (x2 - x_floor);
					
					u8 rbot = r3 * (x_ceil - x2) + r4 * (x2 - x_floor);
					u8 gbot = g3 * (x_ceil - x2) + g4 * (x2 - x_floor);
					u8 bbot = b3 * (x_ceil - x2) + b4 * (x2 - x_floor);
					
					u8 rfinal = rtop * (y_ceil - y2) + rbot * (y2 - y_floor);
					u8 gfinal = gtop * (y_ceil - y2) + gbot * (y2 - y_floor);
					u8 bfinal = btop * (y_ceil - y2) + bbot * (y2 - y_floor);
					
					AGIDL_SetClr(scale,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
				}
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);

		return scale;
	}
	else{
		u16 worg = *width;
		u16 horg = *height;
		
		COLOR16* clrs = (COLOR16*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR16* scale = (COLOR16*)malloc(sizeof(COLOR16)*newwidth*newheight);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		float x_diff, y_diff;
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				float x2 = (x*xscale);
				float y2 = (y*yscale);
				
				float x_floor = floor(x2);
				float y_floor = floor(y2);
				float x_ceil = ceil(x2);
				float y_ceil = ceil(y2);
				
				COLOR16 clr1 = AGIDL_GetClr16(clrs,x_floor,y_floor,worg,horg);
				COLOR16 clr2 = AGIDL_GetClr16(clrs,x_ceil,y_floor,worg,horg);
				COLOR16 clr3 = AGIDL_GetClr16(clrs,x_floor,y_ceil,worg,horg);
				COLOR16 clr4 = AGIDL_GetClr16(clrs,x_ceil,y_ceil,worg,horg);
				
				if(x2 == 0 || y2 == 0){
					clr1 = AGIDL_GetClr16(clrs,x2,y2,worg,horg);
					clr2 = AGIDL_GetClr16(clrs,x2+1,y2,worg,horg);
					clr3 = AGIDL_GetClr16(clrs,x2,y2+1,worg,horg);
					clr4 = AGIDL_GetClr16(clrs,x2+1,y2+1,worg,horg);
					
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 + ((r2-r1) >> 1);
					u8 gtop = g1 + ((g2-g1) >> 1);
					u8 btop = b1 + ((b2-b1) >> 1);
					
					u8 rbot = r3 + ((r4-r3) >> 1);
					u8 gbot = g3 + ((g4-g3) >> 1);
					u8 bbot = b3 + ((b4-b3) >> 1);
					
					u8 rfinal = rtop + ((rbot-rtop) >> 1);
					u8 gfinal = gtop + ((gbot-gtop) >> 1);
					u8 bfinal = btop + ((bbot-btop) >> 1);
					
					AGIDL_SetClr16(scale,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
				}
				else{
					
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 * (x_ceil - x2) + r2 * (x2 - x_floor);
					u8 gtop = g1 * (x_ceil - x2) + g2 * (x2 - x_floor);
					u8 btop = b1 * (x_ceil - x2) + b2 * (x2 - x_floor);
					
					u8 rbot = r3 * (x_ceil - x2) + r4 * (x2 - x_floor);
					u8 gbot = g3 * (x_ceil - x2) + g4 * (x2 - x_floor);
					u8 bbot = b3 * (x_ceil - x2) + b4 * (x2 - x_floor);
					
					u8 rfinal = rtop * (y_ceil - y2) + rbot * (y2 - y_floor);
					u8 gfinal = gtop * (y_ceil - y2) + gbot * (y2 - y_floor);
					u8 bfinal = btop * (y_ceil - y2) + bbot * (y2 - y_floor);
					
					AGIDL_SetClr16(scale,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
				}
				
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);

		return scale;
	}
}

void * AGIDL_FastScaleImgDataBilerp(void* data, u16* width, u16* height, float sx, float sy, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		u16 worg = *width;
		u16 horg = *height;
		
		COLOR* clrs = (COLOR*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR* scale = (COLOR*)malloc(sizeof(COLOR)*newwidth*newheight);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		float x_diff, y_diff;
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				u16 x2 = (x*xscale);
				u16 y2 = (y*yscale);
				
				COLOR clr1 = AGIDL_GetClr(clrs,x2,y2,worg,horg);
				COLOR clr2 = AGIDL_GetClr(clrs,x2+1,y2,worg,horg);
				COLOR clr3 = AGIDL_GetClr(clrs,x2,y2+1,worg,horg);
				COLOR clr4 = AGIDL_GetClr(clrs,x2+1,y2+1,worg,horg);
				
				u8 r1 = AGIDL_GetR(clr1,fmt);
				u8 g1 = AGIDL_GetG(clr1,fmt);
				u8 b1 = AGIDL_GetB(clr1,fmt);
				
				u8 r2 = AGIDL_GetR(clr2,fmt);
				u8 g2 = AGIDL_GetG(clr2,fmt);
				u8 b2 = AGIDL_GetB(clr2,fmt);
				
				u8 r3 = AGIDL_GetR(clr3,fmt);
				u8 g3 = AGIDL_GetG(clr3,fmt);
				u8 b3 = AGIDL_GetB(clr3,fmt);
				
				u8 r4 = AGIDL_GetR(clr4,fmt);
				u8 g4 = AGIDL_GetG(clr4,fmt);
				u8 b4 = AGIDL_GetB(clr4,fmt);
				
				u8 rtop = r1 + ((r2-r1) >> 1);
				u8 gtop = g1 + ((g2-g1) >> 1);
				u8 btop = b1 + ((b2-b1) >> 1);
				
				u8 rbot = r3 + ((r4-r3) >> 1);
				u8 gbot = g3 + ((g4-g3) >> 1);
				u8 bbot = b3 + ((b4-b3) >> 1);
				
				u8 rfinal = rtop + ((rbot-rtop) >> 1);
				u8 gfinal = gtop + ((gbot-gtop) >> 1);
				u8 bfinal = btop + ((bbot-btop) >> 1);
				
				AGIDL_SetClr(scale,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);

		return scale;
	}
	else{
		u16 worg = *width;
		u16 horg = *height;
		
		COLOR16* clrs = (COLOR16*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR16* scale = (COLOR16*)malloc(sizeof(COLOR16)*newwidth*newheight);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		float x_diff, y_diff;
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				u16 x2 = (x*xscale);
				u16 y2 = (y*yscale);
				
				COLOR16 clr1 = AGIDL_GetClr16(clrs,x2,y2,worg,horg);
				COLOR16 clr2 = AGIDL_GetClr16(clrs,x2+1,y2,worg,horg);
				COLOR16 clr3 = AGIDL_GetClr16(clrs,x2,y2+1,worg,horg);
				COLOR16 clr4 = AGIDL_GetClr16(clrs,x2+1,y2+1,worg,horg);
				
				u8 r1 = AGIDL_GetR(clr1,fmt);
				u8 g1 = AGIDL_GetG(clr1,fmt);
				u8 b1 = AGIDL_GetB(clr1,fmt);
				
				u8 r2 = AGIDL_GetR(clr2,fmt);
				u8 g2 = AGIDL_GetG(clr2,fmt);
				u8 b2 = AGIDL_GetB(clr2,fmt);
				
				u8 r3 = AGIDL_GetR(clr3,fmt);
				u8 g3 = AGIDL_GetG(clr3,fmt);
				u8 b3 = AGIDL_GetB(clr3,fmt);
				
				u8 r4 = AGIDL_GetR(clr4,fmt);
				u8 g4 = AGIDL_GetG(clr4,fmt);
				u8 b4 = AGIDL_GetB(clr4,fmt);
				
				u8 rtop = r1 + ((r2-r1) >> 1);
				u8 gtop = g1 + ((g2-g1) >> 1);
				u8 btop = b1 + ((b2-b1) >> 1);
				
				u8 rbot = r3 + ((r4-r3) >> 1);
				u8 gbot = g3 + ((g4-g3) >> 1);
				u8 bbot = b3 + ((b4-b3) >> 1);
				
				u8 rfinal = rtop + ((rbot-rtop) >> 1);
				u8 gfinal = gtop + ((gbot-gtop) >> 1);
				u8 bfinal = btop + ((bbot-btop) >> 1);
				
				AGIDL_SetClr16(scale,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);

		return scale;
	}
}


void * AGIDL_ScaleImgDataTrilerp(void* data, u16* width, u16* height, float sx, float sy, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		u16 worg = *width;
		u16 horg = *height;
		
		u16 whalf = worg;
		u16 hhalf = horg;
		
		COLOR* clrs = (COLOR*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR* datacpy = (COLOR*)malloc(sizeof(COLOR)*worg*horg);
		AGIDL_ClrMemcpy(datacpy,clrs,worg*horg);
		
		COLOR* scale = (COLOR*)malloc(sizeof(COLOR)*newwidth*newheight);
		COLOR* scalehalf = (COLOR*)malloc(sizeof(COLOR)*whalf*hhalf);
		
		scalehalf = (COLOR*)AGIDL_ScaleImgDataBilerp(datacpy,&whalf,&hhalf,0.5f,0.5f,fmt);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		float xscalehalf = ((float)(whalf-1)/newwidth);
		float yscalehalf = ((float)(hhalf-1)/newheight);
		
		float x_diff, y_diff, x_half, y_half;
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				float x2 = (x*xscale);
				float y2 = (y*yscale);
				
				float x_floor = floor(x2);
				float y_floor = floor(y2);
				float x_ceil = ceil(x2);
				float y_ceil = ceil(y2);
				
				float x2half = (x*xscalehalf);
				float y2half = (y*yscalehalf);
				
				float x_floor2 = floor(x2half);
				float y_floor2 = floor(y2half);
				float x_ceil2 = ceil(x2half);
				float y_ceil2 = ceil(y2half);
				
				COLOR clr1 = AGIDL_GetClr(clrs,x_floor,y_floor,worg,horg);
				COLOR clr2 = AGIDL_GetClr(clrs,x_ceil,y_floor,worg,horg);
				COLOR clr3 = AGIDL_GetClr(clrs,x_floor,y_ceil,worg,horg);
				COLOR clr4 = AGIDL_GetClr(clrs,x_ceil,y_ceil,worg,horg);
				
				COLOR clr12 = AGIDL_GetClr(scalehalf,x_floor2,y_floor2,whalf,hhalf);
				COLOR clr22 = AGIDL_GetClr(scalehalf,x_ceil2,y_floor2,whalf,hhalf);
				COLOR clr32 = AGIDL_GetClr(scalehalf,x_floor2,y_ceil2,whalf,hhalf);
				COLOR clr42 = AGIDL_GetClr(scalehalf,x_ceil2,y_ceil2,whalf,hhalf);
				
				if(x2 == 0 || y2 == 0 || x2half == 0 || y2half == 0){
					clr1 = AGIDL_GetClr(clrs,x2,y2,worg,horg);
					clr2 = AGIDL_GetClr(clrs,x2+1,y2,worg,horg);
					clr3 = AGIDL_GetClr(clrs,x2,y2+1,worg,horg);
					clr4 = AGIDL_GetClr(clrs,x2+1,y2+1,worg,horg);
					
					clr12 = AGIDL_GetClr(scalehalf,x2half,y2half,whalf,hhalf);
					clr22 = AGIDL_GetClr(scalehalf,x2half+1,y2half,whalf,hhalf);
					clr32 = AGIDL_GetClr(scalehalf,x2half,y2half+1,whalf,hhalf);
					clr42 = AGIDL_GetClr(scalehalf,x2half+1,y2half+1,whalf,hhalf);
					
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 + ((r2-r1) >> 1);
					u8 gtop = g1 + ((g2-g1) >> 1);
					u8 btop = b1 + ((b2-b1) >> 1);
					
					u8 rbot = r3 + ((r4-r3) >> 1);
					u8 gbot = g3 + ((g4-g3) >> 1);
					u8 bbot = b3 + ((b4-b3) >> 1);
					
					u8 rbilerp1 = rtop + ((rbot-rtop) >> 1);
					u8 gbilerp1 = gtop + ((gbot-gtop) >> 1);
					u8 bbilerp1 = btop + ((bbot-btop) >> 1);
					
					r1 = AGIDL_GetR(clr12,fmt);
					g1 = AGIDL_GetG(clr12,fmt);
					b1 = AGIDL_GetB(clr12,fmt);
					
					r2 = AGIDL_GetR(clr22,fmt);
					g2 = AGIDL_GetG(clr22,fmt);
					b2 = AGIDL_GetB(clr22,fmt);
					
					r3 = AGIDL_GetR(clr32,fmt);
					g3 = AGIDL_GetG(clr32,fmt);
					b3 = AGIDL_GetB(clr32,fmt);
					
					r4 = AGIDL_GetR(clr42,fmt);
					g4 = AGIDL_GetG(clr42,fmt);
					b4 = AGIDL_GetB(clr42,fmt);
					
					rtop = r1 + ((r2-r1) >> 1);
					gtop = g1 + ((g2-g1) >> 1);
					btop = b1 + ((b2-b1) >> 1);
					
					rbot = r3 + ((r4-r3) >> 1);
					gbot = g3 + ((g4-g3) >> 1);
					bbot = b3 + ((b4-b3) >> 1);
					
					u8 rbilerp2 = rtop + ((rbot-rtop) >> 1);
					u8 gbilerp2 = gtop + ((gbot-gtop) >> 1);
					u8 bbilerp2 = btop + ((bbot-btop) >> 1);
					
					u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
					u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
					u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);
					
					AGIDL_SetClr(scale,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
				}
				else{
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 * (x_ceil - x2) + r2 * (x2 - x_floor);
					u8 gtop = g1 * (x_ceil - x2) + g2 * (x2 - x_floor);
					u8 btop = b1 * (x_ceil - x2) + b2 * (x2 - x_floor);
					
					u8 rbot = r3 * (x_ceil - x2) + r4 * (x2 - x_floor);
					u8 gbot = g3 * (x_ceil - x2) + g4 * (x2 - x_floor);
					u8 bbot = b3 * (x_ceil - x2) + b4 * (x2 - x_floor);
					
					u8 rbilerp1 = rtop * (y_ceil - y2) + rbot * (y2 - y_floor);
					u8 gbilerp1 = gtop * (y_ceil - y2) + gbot * (y2 - y_floor);
					u8 bbilerp1 = btop * (y_ceil - y2) + bbot * (y2 - y_floor);
					
					r1 = AGIDL_GetR(clr12,fmt);
					g1 = AGIDL_GetG(clr12,fmt);
					b1 = AGIDL_GetB(clr12,fmt);
					
					r2 = AGIDL_GetR(clr22,fmt);
					g2 = AGIDL_GetG(clr22,fmt);
					b2 = AGIDL_GetB(clr22,fmt);
					
					r3 = AGIDL_GetR(clr32,fmt);
					g3 = AGIDL_GetG(clr32,fmt);
					b3 = AGIDL_GetB(clr32,fmt);
					
					r4 = AGIDL_GetR(clr42,fmt);
					g4 = AGIDL_GetG(clr42,fmt);
					b4 = AGIDL_GetB(clr42,fmt);
					
					rtop = r1 * (x_ceil2 - x2half) + r2 * (x2half - x_floor2);
					gtop = g1 * (x_ceil2 - x2half) + g2 * (x2half - x_floor2);
					btop = b1 * (x_ceil2 - x2half) + b2 * (x2half - x_floor2);
					
					rbot = r3 * (x_ceil2 - x2half) + r4 * (x2half - x_floor2);
					gbot = g3 * (x_ceil2 - x2half) + g4 * (x2half - x_floor2);
					bbot = b3 * (x_ceil2 - x2half) + b4 * (x2half - x_floor2);
					
					u8 rbilerp2 = rtop * (y_ceil2 - y2half) + rbot * (y2half - y_floor2);
					u8 gbilerp2 = gtop * (y_ceil2 - y2half) + gbot * (y2half - y_floor2);
					u8 bbilerp2 = btop * (y_ceil2 - y2half) + bbot * (y2half - y_floor2);
					
					float diff = AGIDL_Abs(rbilerp2 - rbilerp1) / 255.0f;
					
					u8 rfinal = rbilerp1 + diff * (rbilerp2 - rbilerp1);
					u8 gfinal = gbilerp1 + diff * (gbilerp2 - gbilerp1);
					u8 bfinal = bbilerp1 + diff * (bbilerp2 - bbilerp1);
					
					AGIDL_SetClr(scale,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
				}
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);
		free(scalehalf);

		return scale;
	}
	else{
		u16 worg = *width;
		u16 horg = *height;
		
		u16 whalf = worg;
		u16 hhalf = horg;
		
		COLOR16* clrs = (COLOR16*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR16* datacpy = (COLOR16*)malloc(sizeof(COLOR16)*worg*horg);
		AGIDL_ClrMemcpy16(datacpy,clrs,worg*horg);
		
		COLOR16* scale = (COLOR16*)malloc(sizeof(COLOR16)*newwidth*newheight);
		COLOR16* scalehalf = (COLOR16*)malloc(sizeof(COLOR16)*whalf*hhalf);
		
		scalehalf = (COLOR16*)AGIDL_ScaleImgDataBilerp(datacpy,&whalf,&hhalf,0.5f,0.5f,fmt);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		float xscalehalf = ((float)(whalf-1)/newwidth);
		float yscalehalf = ((float)(hhalf-1)/newheight);
		
		float x_diff, y_diff, x_half, y_half;
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				float x2 = (x*xscale);
				float y2 = (y*yscale);
				
				float x_floor = floor(x2);
				float y_floor = floor(y2);
				float x_ceil = ceil(x2);
				float y_ceil = ceil(y2);
				
				float x2half = (x*xscalehalf);
				float y2half = (y*yscalehalf);
				
				float x_floor2 = floor(x2half);
				float y_floor2 = floor(y2half);
				float x_ceil2 = ceil(x2half);
				float y_ceil2 = ceil(y2half);
				
				COLOR16 clr1 = AGIDL_GetClr16(clrs,x2,y2,worg,horg);
				COLOR16 clr2 = AGIDL_GetClr16(clrs,x2+1,y2,worg,horg);
				COLOR16 clr3 = AGIDL_GetClr16(clrs,x2,y2+1,worg,horg);
				COLOR16 clr4 = AGIDL_GetClr16(clrs,x2+1,y2+1,worg,horg);
				
				COLOR16 clr12 = AGIDL_GetClr16(scalehalf,x2half,y2half,whalf,hhalf);
				COLOR16 clr22 = AGIDL_GetClr16(scalehalf,x2half+1,y2half,whalf,hhalf);
				COLOR16 clr32 = AGIDL_GetClr16(scalehalf,x2half,y2half+1,whalf,hhalf);
				COLOR16 clr42 = AGIDL_GetClr16(scalehalf,x2half+1,y2half+1,whalf,hhalf);
				
				if(x2 == 0 || y2 == 0 || x2half == 0 || y2half == 0){
					clr1 = AGIDL_GetClr16(clrs,x2,y2,worg,horg);
					clr2 = AGIDL_GetClr16(clrs,x2+1,y2,worg,horg);
					clr3 = AGIDL_GetClr16(clrs,x2,y2+1,worg,horg);
					clr4 = AGIDL_GetClr16(clrs,x2+1,y2+1,worg,horg);
					
					clr12 = AGIDL_GetClr16(scalehalf,x2half,y2half,whalf,hhalf);
					clr22 = AGIDL_GetClr16(scalehalf,x2half+1,y2half,whalf,hhalf);
					clr32 = AGIDL_GetClr16(scalehalf,x2half,y2half+1,whalf,hhalf);
					clr42 = AGIDL_GetClr16(scalehalf,x2half+1,y2half+1,whalf,hhalf);
					
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 + ((r2-r1) >> 1);
					u8 gtop = g1 + ((g2-g1) >> 1);
					u8 btop = b1 + ((b2-b1) >> 1);
					
					u8 rbot = r3 + ((r4-r3) >> 1);
					u8 gbot = g3 + ((g4-g3) >> 1);
					u8 bbot = b3 + ((b4-b3) >> 1);
					
					u8 rbilerp1 = rtop + ((rbot-rtop) >> 1);
					u8 gbilerp1 = gtop + ((gbot-gtop) >> 1);
					u8 bbilerp1 = btop + ((bbot-btop) >> 1);
					
					r1 = AGIDL_GetR(clr12,fmt);
					g1 = AGIDL_GetG(clr12,fmt);
					b1 = AGIDL_GetB(clr12,fmt);
					
					r2 = AGIDL_GetR(clr22,fmt);
					g2 = AGIDL_GetG(clr22,fmt);
					b2 = AGIDL_GetB(clr22,fmt);
					
					r3 = AGIDL_GetR(clr32,fmt);
					g3 = AGIDL_GetG(clr32,fmt);
					b3 = AGIDL_GetB(clr32,fmt);
					
					r4 = AGIDL_GetR(clr42,fmt);
					g4 = AGIDL_GetG(clr42,fmt);
					b4 = AGIDL_GetB(clr42,fmt);
					
					rtop = r1 + ((r2-r1) >> 1);
					gtop = g1 + ((g2-g1) >> 1);
					btop = b1 + ((b2-b1) >> 1);
					
					rbot = r3 + ((r4-r3) >> 1);
					gbot = g3 + ((g4-g3) >> 1);
					bbot = b3 + ((b4-b3) >> 1);
					
					u8 rbilerp2 = rtop + ((rbot-rtop) >> 1);
					u8 gbilerp2 = gtop + ((gbot-gtop) >> 1);
					u8 bbilerp2 = btop + ((bbot-btop) >> 1);
					
					u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
					u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
					u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);
					
					AGIDL_SetClr16(scale,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
				}
				else{
					u8 r1 = AGIDL_GetR(clr1,fmt);
					u8 g1 = AGIDL_GetG(clr1,fmt);
					u8 b1 = AGIDL_GetB(clr1,fmt);
					
					u8 r2 = AGIDL_GetR(clr2,fmt);
					u8 g2 = AGIDL_GetG(clr2,fmt);
					u8 b2 = AGIDL_GetB(clr2,fmt);
					
					u8 r3 = AGIDL_GetR(clr3,fmt);
					u8 g3 = AGIDL_GetG(clr3,fmt);
					u8 b3 = AGIDL_GetB(clr3,fmt);
					
					u8 r4 = AGIDL_GetR(clr4,fmt);
					u8 g4 = AGIDL_GetG(clr4,fmt);
					u8 b4 = AGIDL_GetB(clr4,fmt);
					
					u8 rtop = r1 * (x_ceil - x2) + r2 * (x2 - x_floor);
					u8 gtop = g1 * (x_ceil - x2) + g2 * (x2 - x_floor);
					u8 btop = b1 * (x_ceil - x2) + b2 * (x2 - x_floor);
					
					u8 rbot = r3 * (x_ceil - x2) + r4 * (x2 - x_floor);
					u8 gbot = g3 * (x_ceil - x2) + g4 * (x2 - x_floor);
					u8 bbot = b3 * (x_ceil - x2) + b4 * (x2 - x_floor);
					
					u8 rbilerp1 = rtop * (y_ceil - y2) + rbot * (y2 - y_floor);
					u8 gbilerp1 = gtop * (y_ceil - y2) + gbot * (y2 - y_floor);
					u8 bbilerp1 = btop * (y_ceil - y2) + bbot * (y2 - y_floor);
					
					r1 = AGIDL_GetR(clr12,fmt);
					g1 = AGIDL_GetG(clr12,fmt);
					b1 = AGIDL_GetB(clr12,fmt);
					
					r2 = AGIDL_GetR(clr22,fmt);
					g2 = AGIDL_GetG(clr22,fmt);
					b2 = AGIDL_GetB(clr22,fmt);
					
					r3 = AGIDL_GetR(clr32,fmt);
					g3 = AGIDL_GetG(clr32,fmt);
					b3 = AGIDL_GetB(clr32,fmt);
					
					r4 = AGIDL_GetR(clr42,fmt);
					g4 = AGIDL_GetG(clr42,fmt);
					b4 = AGIDL_GetB(clr42,fmt);
					
					rtop = r1 * (x_ceil2 - x2half) + r2 * (x2half - x_floor2);
					gtop = g1 * (x_ceil2 - x2half) + g2 * (x2half - x_floor2);
					btop = b1 * (x_ceil2 - x2half) + b2 * (x2half - x_floor2);
					
					rbot = r3 * (x_ceil2 - x2half) + r4 * (x2half - x_floor2);
					gbot = g3 * (x_ceil2 - x2half) + g4 * (x2half - x_floor2);
					bbot = b3 * (x_ceil2 - x2half) + b4 * (x2half - x_floor2);
					
					u8 rbilerp2 = rtop * (y_ceil2 - y2half) + rbot * (y2half - y_floor2);
					u8 gbilerp2 = gtop * (y_ceil2 - y2half) + gbot * (y2half - y_floor2);
					u8 bbilerp2 = btop * (y_ceil2 - y2half) + bbot * (y2half - y_floor2);
					
					float diff = AGIDL_Abs(rbilerp2 - rbilerp1) / 31.0f;
					
					u8 rfinal = rbilerp1 + diff * (rbilerp2 - rbilerp1);
					u8 gfinal = gbilerp1 + diff * (gbilerp2 - gbilerp1);
					u8 bfinal = bbilerp1 + diff * (bbilerp2 - bbilerp1);
					
					AGIDL_SetClr16(scale,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
				}	
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);
		free(scalehalf);

		return scale;
	}
}

void * AGIDL_FastScaleImgDataTrilerp(void* data, u16* width, u16* height, float sx, float sy, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		u16 worg = *width;
		u16 horg = *height;
		
		u16 whalf = worg;
		u16 hhalf = horg;
		
		COLOR* clrs = (COLOR*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR* datacpy = (COLOR*)malloc(sizeof(COLOR)*worg*horg);
		AGIDL_ClrMemcpy(datacpy,clrs,worg*horg);
		
		COLOR* scale = (COLOR*)malloc(sizeof(COLOR)*newwidth*newheight);
		COLOR* scalehalf = (COLOR*)malloc(sizeof(COLOR)*whalf*hhalf);
		
		scalehalf = (COLOR*)AGIDL_FastScaleImgDataBilerp(datacpy,&whalf,&hhalf,0.5f,0.5f,fmt);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		float xscalehalf = ((float)(whalf-1)/newwidth);
		float yscalehalf = ((float)(hhalf-1)/newheight);
		
		float x_diff, y_diff, x_half, y_half;
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				u16 x2 = (x*xscale);
				u16 y2 = (y*yscale);
				
				u16 x2half = (x*xscalehalf);
				u16 y2half = (y*yscalehalf);
				
				COLOR clr1 = AGIDL_GetClr(clrs,x2,y2,worg,horg);
				COLOR clr2 = AGIDL_GetClr(clrs,x2+1,y2,worg,horg);
				COLOR clr3 = AGIDL_GetClr(clrs,x2,y2+1,worg,horg);
				COLOR clr4 = AGIDL_GetClr(clrs,x2+1,y2+1,worg,horg);
				
				COLOR clr1half = AGIDL_GetClr(scalehalf,x2half,y2half,whalf,hhalf);
				COLOR clr2half = AGIDL_GetClr(scalehalf,x2half+1,y2half,whalf,hhalf);
				COLOR clr3half = AGIDL_GetClr(scalehalf,x2half,y2half+1,whalf,hhalf);
				COLOR clr4half = AGIDL_GetClr(scalehalf,x2half+1,y2half+1,whalf,hhalf);
				
				u8 r1 = AGIDL_GetR(clr1,fmt);
				u8 g1 = AGIDL_GetG(clr1,fmt);
				u8 b1 = AGIDL_GetB(clr1,fmt);
				
				u8 r2 = AGIDL_GetR(clr2,fmt);
				u8 g2 = AGIDL_GetG(clr2,fmt);
				u8 b2 = AGIDL_GetB(clr2,fmt);
				
				u8 r3 = AGIDL_GetR(clr3,fmt);
				u8 g3 = AGIDL_GetG(clr3,fmt);
				u8 b3 = AGIDL_GetB(clr3,fmt);
				
				u8 r4 = AGIDL_GetR(clr4,fmt);
				u8 g4 = AGIDL_GetG(clr4,fmt);
				u8 b4 = AGIDL_GetB(clr4,fmt);
				
				u8 rtop = r1 + ((r2-r1) >> 1);
				u8 gtop = g1 + ((g2-g1) >> 1);
				u8 btop = b1 + ((b2-b1) >> 1);
				
				u8 rbot = r3 + ((r4-r3) >> 1);
				u8 gbot = g3 + ((g4-g3) >> 1);
				u8 bbot = b3 + ((b4-b3) >> 1);
				
				u8 rbilerp1 = rtop + ((rbot-rtop) >> 1);
				u8 gbilerp1 = gtop + ((gbot-gtop) >> 1);
				u8 bbilerp1 = btop + ((bbot-btop) >> 1);
				
				r1 = AGIDL_GetR(clr1half,fmt);
			    g1 = AGIDL_GetG(clr1half,fmt);
				b1 = AGIDL_GetB(clr1half,fmt);
				
				r2 = AGIDL_GetR(clr2half,fmt);
				g2 = AGIDL_GetG(clr2half,fmt);
				b2 = AGIDL_GetB(clr2half,fmt);
				
				r3 = AGIDL_GetR(clr3half,fmt);
				g3 = AGIDL_GetG(clr3half,fmt);
				b3 = AGIDL_GetB(clr3half,fmt);
				
				r4 = AGIDL_GetR(clr4half,fmt);
				g4 = AGIDL_GetG(clr4half,fmt);
				b4 = AGIDL_GetB(clr4half,fmt);
				
				rtop = r1 + ((r2-r1) >> 1);
				gtop = g1 + ((g2-g1) >> 1);
				btop = b1 + ((b2-b1) >> 1);
				
				rbot = r3 + ((r4-r3) >> 1);
				gbot = g3 + ((g4-g3) >> 1);
				bbot = b3 + ((b4-b3) >> 1);
				
				u8 rbilerp2 = rtop + ((rbot-rtop) >> 1);
				u8 gbilerp2 = gtop + ((gbot-gtop) >> 1);
				u8 bbilerp2 = btop + ((bbot-btop) >> 1);
				
				u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
				u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
				u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);
				
				AGIDL_SetClr(scale,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);
		free(scalehalf);

		return scale;
	}
	else{
		u16 worg = *width;
		u16 horg = *height;
		
		u16 whalf = worg;
		u16 hhalf = horg;
		
		COLOR16* clrs = (COLOR16*)data;
		
		u16 newwidth = (u16)(worg*sx), newheight = (u16)(horg*sy);
		
		COLOR16* datacpy = (COLOR16*)malloc(sizeof(COLOR16)*worg*horg);
		AGIDL_ClrMemcpy16(datacpy,clrs,worg*horg);
		
		COLOR16* scale = (COLOR16*)malloc(sizeof(COLOR16)*newwidth*newheight);
		COLOR16* scalehalf = (COLOR16*)malloc(sizeof(COLOR16)*whalf*hhalf);
		
		scalehalf = (COLOR16*)AGIDL_ScaleImgDataBilerp(datacpy,&whalf,&hhalf,0.5f,0.5f,fmt);
		
		float xscale = ((float)(worg-1)/newwidth);
		float yscale = ((float)(horg-1)/newheight);
		
		float xscalehalf = ((float)(whalf-1)/newwidth);
		float yscalehalf = ((float)(hhalf-1)/newheight);
		
		float x_diff, y_diff, x_half, y_half;
		
		u16 x,y;
		for(y = 0; y < newheight; y++){
			for(x = 0; x < newwidth; x++){
				u16 x2 = (x*xscale);
				u16 y2 = (y*yscale);
				
				u16 x2half = (x*xscalehalf);
				u16 y2half = (y*yscalehalf);
				
				COLOR16 clr1 = AGIDL_GetClr16(clrs,x2,y2,worg,horg);
				COLOR16 clr2 = AGIDL_GetClr16(clrs,x2+1,y2,worg,horg);
				COLOR16 clr3 = AGIDL_GetClr16(clrs,x2,y2+1,worg,horg);
				COLOR16 clr4 = AGIDL_GetClr16(clrs,x2+1,y2+1,worg,horg);
				
				COLOR16 clr1half = AGIDL_GetClr16(scalehalf,x2half,y2half,whalf,hhalf);
				COLOR16 clr2half = AGIDL_GetClr16(scalehalf,x2half+1,y2half,whalf,hhalf);
				COLOR16 clr3half = AGIDL_GetClr16(scalehalf,x2half,y2half+1,whalf,hhalf);
				COLOR16 clr4half = AGIDL_GetClr16(scalehalf,x2half+1,y2half+1,whalf,hhalf);
				
				u8 r1 = AGIDL_GetR(clr1,fmt);
				u8 g1 = AGIDL_GetG(clr1,fmt);
				u8 b1 = AGIDL_GetB(clr1,fmt);
				
				u8 r2 = AGIDL_GetR(clr2,fmt);
				u8 g2 = AGIDL_GetG(clr2,fmt);
				u8 b2 = AGIDL_GetB(clr2,fmt);
				
				u8 r3 = AGIDL_GetR(clr3,fmt);
				u8 g3 = AGIDL_GetG(clr3,fmt);
				u8 b3 = AGIDL_GetB(clr3,fmt);
				
				u8 r4 = AGIDL_GetR(clr4,fmt);
				u8 g4 = AGIDL_GetG(clr4,fmt);
				u8 b4 = AGIDL_GetB(clr4,fmt);
				
				u8 rtop = r1 + ((r2-r1) >> 1);
				u8 gtop = g1 + ((g2-g1) >> 1);
				u8 btop = b1 + ((b2-b1) >> 1);
				
				u8 rbot = r3 + ((r4-r3) >> 1);
				u8 gbot = g3 + ((g4-g3) >> 1);
				u8 bbot = b3 + ((b4-b3) >> 1);
				
				u8 rbilerp1 = rtop + ((rbot-rtop) >> 1);
				u8 gbilerp1 = gtop + ((gbot-gtop) >> 1);
				u8 bbilerp1 = btop + ((bbot-btop) >> 1);
				
				r1 = AGIDL_GetR(clr1half,fmt);
			    g1 = AGIDL_GetG(clr1half,fmt);
				b1 = AGIDL_GetB(clr1half,fmt);
				
				r2 = AGIDL_GetR(clr2half,fmt);
				g2 = AGIDL_GetG(clr2half,fmt);
				b2 = AGIDL_GetB(clr2half,fmt);
				
				r3 = AGIDL_GetR(clr3half,fmt);
				g3 = AGIDL_GetG(clr3half,fmt);
				b3 = AGIDL_GetB(clr3half,fmt);
				
				r4 = AGIDL_GetR(clr4half,fmt);
				g4 = AGIDL_GetG(clr4half,fmt);
				b4 = AGIDL_GetB(clr4half,fmt);
				
				rtop = r1 + ((r2-r1) >> 1);
				gtop = g1 + ((g2-g1) >> 1);
				btop = b1 + ((b2-b1) >> 1);
				
				rbot = r3 + ((r4-r3) >> 1);
				gbot = g3 + ((g4-g3) >> 1);
				bbot = b3 + ((b4-b3) >> 1);
				
				u8 rbilerp2 = rtop + ((rbot-rtop) >> 1);
				u8 gbilerp2 = gtop + ((gbot-gtop) >> 1);
				u8 bbilerp2 = btop + ((bbot-btop) >> 1);
				
				u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
				u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
				u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);
				
				AGIDL_SetClr16(scale,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,newwidth,newheight);
			}
		}
		
		*width = newwidth;
		*height = newheight;
		
		free(clrs);
		free(scalehalf);

		return scale;
	}
}

void * AGIDL_ScaleImgData(void* data, u16* width, u16* height, float sx, float sy, AGIDL_SCALE scale, AGIDL_CLR_FMT fmt){
	switch(scale){
		case AGIDL_SCALE_NEAREST:{
			return AGIDL_ScaleImgDataNearest(data,width,height,sx,sy,fmt);
		}break;
		case AGIDL_SCALE_BILERP:{
			return AGIDL_ScaleImgDataBilerp(data,width,height,sx,sy,fmt);
		}break;
		case AGIDL_SCALE_TRILERP:{
			return AGIDL_ScaleImgDataTrilerp(data,width,height,sx,sy,fmt);
		}break;
	}
	return AGIDL_ScaleImgDataNearest(data,width,height,sx,sy,fmt);
}

void * AGIDL_FastScaleImgData(void* data, u16* width, u16* height, float sx, float sy, AGIDL_SCALE scale, AGIDL_CLR_FMT fmt){
	switch(scale){
		case AGIDL_SCALE_NEAREST:{
			return AGIDL_ScaleImgDataNearest(data,width,height,sx,sy,fmt);
		}break;
		case AGIDL_SCALE_BILERP:{
			return AGIDL_FastScaleImgDataBilerp(data,width,height,sx,sy,fmt);
		}break;
		case AGIDL_SCALE_TRILERP:{
			return AGIDL_FastScaleImgDataTrilerp(data,width,height,sx,sy,fmt);
		}break;
	}
	return AGIDL_ScaleImgDataNearest(data,width,height,sx,sy,fmt);
}

void * AGIDL_NearestPow2ScaleImgData(void* data, u16* width, u16* height, u16 max_pow, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clr_data = (COLOR16*)data;
		
		u16 worg = *width;
		u16 horg = *height;
		
		u16 w = AGIDL_NearestPow2((int)(worg)), h = AGIDL_NearestPow2((int)(horg));
		
		w = AGIDL_Clamp(1,w,max_pow); h = AGIDL_Clamp(1,h,max_pow);
		
		COLOR16* scale = (COLOR16*)malloc(sizeof(COLOR16)*w*h);
		
		float xscale = ((float)(worg-1)/w);
		float yscale = ((float)(horg-1)/h);
		
		int x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				
				u32 x2 = (x*xscale);
				u32 y2 = (y*yscale);
				
				COLOR16 clr = AGIDL_GetClr16(clr_data,x2,y2,worg,horg);
				
				AGIDL_SetClr16(scale,clr,x,y,w,h);
			}
		}
		
		*width = w;
		*height = h;
		
		free(clr_data);
		
		return scale;
	}
	else{
		COLOR* clr_data = (COLOR*)data;
		
		u16 worg = *width;
		u16 horg = *height;
		
		u16 w = AGIDL_NearestPow2((int)(worg)), h = AGIDL_NearestPow2((int)(horg));
		
		w = AGIDL_Clamp(1,w,max_pow); h = AGIDL_Clamp(1,h,max_pow);
		
		COLOR* scale = (COLOR*)malloc(sizeof(COLOR)*w*h);
		
		float xscale = ((float)(worg-1)/w);
		float yscale = ((float)(horg-1)/h);
		
		int x,y;
		for(y = 0; y < h; y++){
			for(x = 0; x < w; x++){
				
				u32 x2 = (x*xscale);
				u32 y2 = (y*yscale);
				
				COLOR clr = AGIDL_GetClr(clr_data,x2,y2,worg,horg);
				
				AGIDL_SetClr(scale,clr,x,y,w,h);
			}
		}
		
		*width = w;
		*height = h;
		
		free(clr_data);
		
		return scale;
	}
}