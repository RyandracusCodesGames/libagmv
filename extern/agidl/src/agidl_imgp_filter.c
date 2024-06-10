/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_filter.c
*   Date: 12/13/2023
*   Version: 0.2b
*   Updated: 1/31/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_imgp_filter.h>

#include <stdlib.h>
#include <tgmath.h>

#include <agidl_imgp_scale.h>
#include <agidl_img_bmp.h>
#include <agidl_math_utils.h>

COLOR AGIDL_SamplePointNearest(const void* data, float u, float v, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt){
	u = AGIDL_Clampf(0.0f,u,1.0f);
	v = AGIDL_Clampf(0.0f,v,1.0f);
	
	float x = u * width - 0.5f;
	float y = v * height - 0.5f;
	
	x = AGIDL_Clampf(0,x,width);
	y = AGIDL_Clampf(0,y,height);
	
	if(AGIDL_GetBitCount(fmt) == 16){
		const COLOR16* clrdata = data;
		return AGIDL_GetClr16(clrdata,x,y,width,height);
	}
	const COLOR* clrdata = data;
	return AGIDL_GetClr(clrdata,x,y,width,height);
}

COLOR AGIDL_SamplePointBilerp(void* data, float u, float v, u32 width, u32 height, AGIDL_CLR_FMT fmt){
	u = AGIDL_Clampf(0.0f,u,1.0f);
	v = AGIDL_Clampf(0.0f,v,1.0f);
	
	float x = u * width - 0.5f;
	float y = v * height - 0.5f;
	
	x = AGIDL_Clampf(0,x,width);
	y = AGIDL_Clampf(0,y,height);
	
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clrdata = data;
		
		float x_floor = floor(x);
		float y_floor = floor(y);
		float x_ceil  = ceil(x);
		float y_ceil  = ceil(y);
		
		if(x > 0 && y > 0){
			COLOR16 clr1 = AGIDL_GetClr16(clrdata,x_floor,y_floor,width,height);
			COLOR16 clr2 = AGIDL_GetClr16(clrdata,x_ceil,y_floor,width,height);
			COLOR16 clr3 = AGIDL_GetClr16(clrdata,x_floor,y_ceil,width,height);
			COLOR16 clr4 = AGIDL_GetClr16(clrdata,x_ceil,y_ceil,width,height);
			
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
			
			u8 rtop = r1 * (x_ceil - x) + r2 * (x - x_floor);
			u8 gtop = g1 * (x_ceil - x) + g2 * (x - x_floor);
			u8 btop = b1 * (x_ceil - x) + b2 * (x - x_floor);
			
			u8 rbot = r3 * (x_ceil - x) + r4 * (x - x_floor);
			u8 gbot = g3 * (x_ceil - x) + g4 * (x - x_floor);
			u8 bbot = b3 * (x_ceil - x) + b4 * (x - x_floor);
			
			u8 rfinal = rtop * (y_ceil - y) + rbot * (y - y_floor);
			u8 gfinal = gtop * (y_ceil - y) + gbot * (y - y_floor);
			u8 bfinal = btop * (y_ceil - y) + bbot * (y - y_floor);
			
			return AGIDL_RGB16(rfinal,gfinal,bfinal,fmt);
		}
		COLOR16 clr1 = AGIDL_GetClr16(clrdata,x,y,width,height);
		COLOR16 clr2 = AGIDL_GetClr16(clrdata,x+1,y,width,height);
		COLOR16 clr3 = AGIDL_GetClr16(clrdata,x,y+1,width,height);
		COLOR16 clr4 = AGIDL_GetClr16(clrdata,x+1,y+1,width,height);

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

		return AGIDL_RGB16(rfinal,gfinal,bfinal,fmt);
	}
	COLOR* clrdata = data;

	float x_floor = floor(x);
	float y_floor = floor(y);
	float x_ceil  = ceil(x);
	float y_ceil  = ceil(y);

	if(x > 0 && y > 0){
		COLOR clr1 = AGIDL_GetClr(clrdata,x_floor,y_floor,width,height);
		COLOR clr2 = AGIDL_GetClr(clrdata,x_ceil,y_floor,width,height);
		COLOR clr3 = AGIDL_GetClr(clrdata,x_floor,y_ceil,width,height);
		COLOR clr4 = AGIDL_GetClr(clrdata,x_ceil,y_ceil,width,height);

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

		u8 rtop = r1 * (x_ceil - x) + r2 * (x - x_floor);
		u8 gtop = g1 * (x_ceil - x) + g2 * (x - x_floor);
		u8 btop = b1 * (x_ceil - x) + b2 * (x - x_floor);

		u8 rbot = r3 * (x_ceil - x) + r4 * (x - x_floor);
		u8 gbot = g3 * (x_ceil - x) + g4 * (x - x_floor);
		u8 bbot = b3 * (x_ceil - x) + b4 * (x - x_floor);

		u8 rfinal = rtop * (y_ceil - y) + rbot * (y - y_floor);
		u8 gfinal = gtop * (y_ceil - y) + gbot * (y - y_floor);
		u8 bfinal = btop * (y_ceil - y) + bbot * (y - y_floor);

		return AGIDL_RGB(rfinal,gfinal,bfinal,fmt);
	}
	COLOR clr1 = AGIDL_GetClr(clrdata,x,y,width,height);
	COLOR clr2 = AGIDL_GetClr(clrdata,x+1,y,width,height);
	COLOR clr3 = AGIDL_GetClr(clrdata,x,y+1,width,height);
	COLOR clr4 = AGIDL_GetClr(clrdata,x+1,y+1,width,height);

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

	return AGIDL_RGB(rfinal,gfinal,bfinal,fmt);
}

COLOR AGIDL_SamplePointTrilerp(void* data, float u, float v, u32 width, u32 height, AGIDL_CLR_FMT fmt){
	u = AGIDL_Clampf(0.0f,u,1.0f);
	v = AGIDL_Clampf(0.0f,v,1.0f);

	float x = u * width - 0.5f;
	float y = v * height - 0.5f;

	x = AGIDL_Clampf(0,x,width);
	y = AGIDL_Clampf(0,y,height);

	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clrdata = data;
		COLOR16* clrscpy = malloc(sizeof(COLOR16)*width*height);
		AGIDL_ClrMemcpy16(clrscpy,clrdata,width*height);

		u16 w = width, h = height;

		COLOR16* clrscale = malloc(sizeof(COLOR16)*w*h);
		clrscale = (COLOR16*)AGIDL_ScaleImgDataBilerp(clrscpy,&w,&h,0.5f,0.5f,fmt);

		float x_floor = floor(x);
		float y_floor = floor(y);
		float x_ceil  = ceil(x);
		float y_ceil  = ceil(y);

		float x_scale = ((float)(w-1)/width);
		float y_scale = ((float)(h-1)/height);
		int xx = x * x_scale;
		int yy = y * y_scale;

		float x_floor2 = floor(xx);
		float y_floor2 = floor(yy);
		float x_ceil2  = ceil(xx);
		float y_ceil2  = ceil(yy);

		COLOR16 clr1 = AGIDL_GetClr16(clrdata,x_floor,y_floor,width,height);
		COLOR16 clr2 = AGIDL_GetClr16(clrdata,x_ceil,y_floor,width,height);
		COLOR16 clr3 = AGIDL_GetClr16(clrdata,x_floor,y_ceil,width,height);
		COLOR16 clr4 = AGIDL_GetClr16(clrdata,x_ceil,y_ceil,width,height);

		COLOR16 clr1s = AGIDL_GetClr16(clrscale,x_floor2,y_floor2,w,h);
		COLOR16 clr2s = AGIDL_GetClr16(clrscale,x_ceil2,y_floor2,w,h);
		COLOR16 clr3s = AGIDL_GetClr16(clrscale,x_floor2,y_ceil2,w,h);
		COLOR16 clr4s = AGIDL_GetClr16(clrscale,x_ceil2,y_ceil2,w,h);

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

		u8 rtop = r1 * (x_ceil - x) + r2 * (x - x_floor);
		u8 gtop = g1 * (x_ceil - x) + g2 * (x - x_floor);
		u8 btop = b1 * (x_ceil - x) + b2 * (x - x_floor);

		u8 rbot = r3 * (x_ceil - x) + r4 * (x - x_floor);
		u8 gbot = g3 * (x_ceil - x) + g4 * (x - x_floor);
		u8 bbot = b3 * (x_ceil - x) + b4 * (x - x_floor);

		u8 rbilerp1 = rtop * (y_ceil - y) + rbot * (y - y_floor);
		u8 gbilerp1 = gtop * (y_ceil - y) + gbot * (y - y_floor);
		u8 bbilerp1 = btop * (y_ceil - y) + bbot * (y - y_floor);

		r1 = AGIDL_GetR(clr1s,fmt);
		g1 = AGIDL_GetG(clr1s,fmt);
		b1 = AGIDL_GetB(clr1s,fmt);

		r2 = AGIDL_GetR(clr2s,fmt);
		g2 = AGIDL_GetG(clr2s,fmt);
		b2 = AGIDL_GetB(clr2s,fmt);

		r3 = AGIDL_GetR(clr3s,fmt);
		g3 = AGIDL_GetG(clr3s,fmt);
		b3 = AGIDL_GetB(clr3s,fmt);

		r4 = AGIDL_GetR(clr4s,fmt);
		g4 = AGIDL_GetG(clr4s,fmt);
		b4 = AGIDL_GetB(clr4s,fmt);

		rtop = r1 * (x_ceil2 - xx) + r2 * (xx - x_floor2);
		gtop = g1 * (x_ceil2 - xx) + g2 * (xx - x_floor2);
		btop = b1 * (x_ceil2 - xx) + b2 * (xx - x_floor2);

		rbot = r3 * (x_ceil2 - xx) + r4 * (xx - x_floor2);
		gbot = g3 * (x_ceil2 - xx) + g4 * (xx - x_floor2);
		bbot = b3 * (x_ceil2 - xx) + b4 * (xx - x_floor2);

		u8 rbilerp2 = rtop * (y_ceil2 - yy) + rbot * (yy - y_floor2);
		u8 gbilerp2 = gtop * (y_ceil2 - yy) + gbot * (yy - y_floor2);
		u8 bbilerp2 = btop * (y_ceil2 - yy) + bbot * (yy - y_floor2);

		free(clrscale);

		u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
		u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
		u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);
		return AGIDL_RGB16(rfinal,gfinal,bfinal,fmt);
	}
	COLOR* clrdata = data;
	COLOR* clrscpy = malloc(sizeof(COLOR)*width*height);
	AGIDL_ClrMemcpy(clrscpy,clrdata,width*height);

	u16 w = width, h = height;

	COLOR* clrscale = malloc(sizeof(COLOR16)*w*h);
	clrscale = (COLOR*)AGIDL_ScaleImgDataBilerp(clrscpy,&w,&h,0.5f,0.5f,fmt);

	float x_floor = floor(x);
	float y_floor = floor(y);
	float x_ceil  = ceil(x);
	float y_ceil  = ceil(y);

	float x_scale = ((float)(w-1)/width);
	float y_scale = ((float)(h-1)/height);

	int xx = x * x_scale;
	int yy = y * y_scale;

	float x_floor2 = floor(xx);
	float y_floor2 = floor(yy);
	float x_ceil2  = ceil(xx);
	float y_ceil2  = ceil(yy);

	COLOR clr1 = AGIDL_GetClr(clrdata,x_floor,y_floor,width,height);
	COLOR clr2 = AGIDL_GetClr(clrdata,x_ceil,y_floor,width,height);
	COLOR clr3 = AGIDL_GetClr(clrdata,x_floor,y_ceil,width,height);
	COLOR clr4 = AGIDL_GetClr(clrdata,x_ceil,y_ceil,width,height);

	COLOR clr1s = AGIDL_GetClr(clrscale,x_floor2,y_floor2,w,h);
	COLOR clr2s = AGIDL_GetClr(clrscale,x_ceil2,y_floor2,w,h);
	COLOR clr3s = AGIDL_GetClr(clrscale,x_floor2,y_ceil2,w,h);
	COLOR clr4s = AGIDL_GetClr(clrscale,x_ceil2,y_ceil2,w,h);

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

	u8 rtop = r1 * (x_ceil - x) + r2 * (x - x_floor);
	u8 gtop = g1 * (x_ceil - x) + g2 * (x - x_floor);
	u8 btop = b1 * (x_ceil - x) + b2 * (x - x_floor);

	u8 rbot = r3 * (x_ceil - x) + r4 * (x - x_floor);
	u8 gbot = g3 * (x_ceil - x) + g4 * (x - x_floor);
	u8 bbot = b3 * (x_ceil - x) + b4 * (x - x_floor);

	u8 rbilerp1 = rtop * (y_ceil - y) + rbot * (y - y_floor);
	u8 gbilerp1 = gtop * (y_ceil - y) + gbot * (y - y_floor);
	u8 bbilerp1 = btop * (y_ceil - y) + bbot * (y - y_floor);

	r1 = AGIDL_GetR(clr1s,fmt);
	g1 = AGIDL_GetG(clr1s,fmt);
	b1 = AGIDL_GetB(clr1s,fmt);

	r2 = AGIDL_GetR(clr2s,fmt);
	g2 = AGIDL_GetG(clr2s,fmt);
	b2 = AGIDL_GetB(clr2s,fmt);

	r3 = AGIDL_GetR(clr3s,fmt);
	g3 = AGIDL_GetG(clr3s,fmt);
	b3 = AGIDL_GetB(clr3s,fmt);

	r4 = AGIDL_GetR(clr4s,fmt);
	g4 = AGIDL_GetG(clr4s,fmt);
	b4 = AGIDL_GetB(clr4s,fmt);

	rtop = r1 * (x_ceil2 - xx) + r2 * (xx - x_floor2);
	gtop = g1 * (x_ceil2 - xx) + g2 * (xx - x_floor2);
	btop = b1 * (x_ceil2 - xx) + b2 * (xx - x_floor2);

	rbot = r3 * (x_ceil2 - xx) + r4 * (xx - x_floor2);
	gbot = g3 * (x_ceil2 - xx) + g4 * (xx - x_floor2);
	bbot = b3 * (x_ceil2 - xx) + b4 * (xx - x_floor2);

	u8 rbilerp2 = rtop * (y_ceil2 - yy) + rbot * (yy - y_floor2);
	u8 gbilerp2 = gtop * (y_ceil2 - yy) + gbot * (yy - y_floor2);
	u8 bbilerp2 = btop * (y_ceil2 - yy) + bbot * (yy - y_floor2);

	free(clrscale);

	u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
	u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
	u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);

	return AGIDL_RGB(rfinal,gfinal,bfinal,fmt);
}

void AGIDL_FilterImgDataBilerp(void* data, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* clrs = data;
		COLOR* temp = malloc(sizeof(COLOR)*width*height);

		for(u16 y = 0; y < height; y++){
			for(u16 x = 0; x < width; x++){
				const COLOR pix = AGIDL_SamplePointBilerp(clrs,x/(float)width,y/(float)height,width,height,fmt);
				AGIDL_SetClr(temp,pix,x,y,width,height);
			}
		}

		AGIDL_ClrMemcpy(clrs,temp,width*height);
		free(temp);
	}
	else{
		COLOR16* clrs = data;
		COLOR16* temp = malloc(sizeof(COLOR16)*width*height);

		for(u16 y = 0; y < height; y++){
			for(u16 x = 0; x < width; x++){
				const COLOR16 pix = AGIDL_SamplePointBilerp(clrs,x/(float)width,y/(float)height,width,height,fmt);
				AGIDL_SetClr16(temp,pix,x,y,width,height);
			}
		}

		AGIDL_ClrMemcpy16(clrs,temp,width*height);
		free(temp);
	}
}

void AGIDL_FastFilterImgDataBilerp(void* data, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 16){
		COLOR16* clr_data = data;

		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR16 clr1 = AGIDL_GetClr16(clr_data,x,y,width,height);
				COLOR16 clr2 = AGIDL_GetClr16(clr_data,x+1,y,width,height);
				COLOR16 clr3 = AGIDL_GetClr16(clr_data,x,y+1,width,height);
				COLOR16 clr4 = AGIDL_GetClr16(clr_data,x+1,y+1,width,height);

				if(x == width-1){
					clr1 = AGIDL_GetClr16(clr_data,x,y,width,height);
					clr2 = AGIDL_GetClr16(clr_data,x-1,y,width,height);
					clr3 = AGIDL_GetClr16(clr_data,x,y+1,width,height);
					clr4 = AGIDL_GetClr16(clr_data,x-1,y+1,width,height);
				}

				if(y == height-1){
					clr1 = AGIDL_GetClr16(clr_data,x,y,width,height);
					clr2 = AGIDL_GetClr16(clr_data,x+1,y,width,height);
					clr3 = AGIDL_GetClr16(clr_data,x,y-1,width,height);
					clr4 = AGIDL_GetClr16(clr_data,x+1,y-1,width,height);
				}

				const u8 r1 = AGIDL_GetR(clr1,fmt);
				const u8 g1 = AGIDL_GetG(clr1,fmt);
				const u8 b1 = AGIDL_GetB(clr1,fmt);

				const u8 r2 = AGIDL_GetR(clr2,fmt);
				const u8 g2 = AGIDL_GetG(clr2,fmt);
				const u8 b2 = AGIDL_GetB(clr2,fmt);

				const u8 r3 = AGIDL_GetR(clr3,fmt);
				const u8 g3 = AGIDL_GetG(clr3,fmt);
				const u8 b3 = AGIDL_GetB(clr3,fmt);

				const u8 r4 = AGIDL_GetR(clr4,fmt);
				const u8 g4 = AGIDL_GetG(clr4,fmt);
				const u8 b4 = AGIDL_GetB(clr4,fmt);

				const u8 rtop = r1 + ((r2-r1) >> 1);
				const u8 gtop = g1 + ((g2-g1) >> 1);
				const u8 btop = b1 + ((b2-b1) >> 1);

				const u8 rbot = r3 + ((r4-r3) >> 1);
				const u8 gbot = g3 + ((g4-g3) >> 1);
				const u8 bbot = b3 + ((b4-b3) >> 1);

				const u8 rfinal = rtop + ((rbot-rtop) >> 1);
				const u8 gfinal = gtop + ((gbot-gtop) >> 1);
				const u8 bfinal = btop + ((bbot-btop) >> 1);
				AGIDL_SetClr16(clr_data,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,width,height);
			}
		}
	}
	else{
		COLOR* clr_data = data;

		for(int y = 0; y < height; y++){
			for(int x = 0; x < width; x++){
				COLOR clr1 = AGIDL_GetClr(clr_data,x,y,width,height);
				COLOR clr2 = AGIDL_GetClr(clr_data,x+1,y,width,height);
				COLOR clr3 = AGIDL_GetClr(clr_data,x,y+1,width,height);
				COLOR clr4 = AGIDL_GetClr(clr_data,x+1,y+1,width,height);

				if(x == width-1){
					clr1 = AGIDL_GetClr(clr_data,x,y,width,height);
					clr2 = AGIDL_GetClr(clr_data,x-1,y,width,height);
					clr3 = AGIDL_GetClr(clr_data,x,y+1,width,height);
					clr4 = AGIDL_GetClr(clr_data,x-1,y+1,width,height);
				}

				if(y == height-1){
					clr1 = AGIDL_GetClr(clr_data,x,y,width,height);
					clr2 = AGIDL_GetClr(clr_data,x+1,y,width,height);
					clr3 = AGIDL_GetClr(clr_data,x,y-1,width,height);
					clr4 = AGIDL_GetClr(clr_data,x+1,y-1,width,height);
				}

				const u8 r1 = AGIDL_GetR(clr1,fmt);
				const u8 g1 = AGIDL_GetG(clr1,fmt);
				const u8 b1 = AGIDL_GetB(clr1,fmt);

				const u8 r2 = AGIDL_GetR(clr2,fmt);
				const u8 g2 = AGIDL_GetG(clr2,fmt);
				const u8 b2 = AGIDL_GetB(clr2,fmt);

				const u8 r3 = AGIDL_GetR(clr3,fmt);
				const u8 g3 = AGIDL_GetG(clr3,fmt);
				const u8 b3 = AGIDL_GetB(clr3,fmt);

				const u8 r4 = AGIDL_GetR(clr4,fmt);
				const u8 g4 = AGIDL_GetG(clr4,fmt);
				const u8 b4 = AGIDL_GetB(clr4,fmt);

				const u8 rtop = r1 + ((r2-r1) >> 1);
				const u8 gtop = g1 + ((g2-g1) >> 1);
				const u8 btop = b1 + ((b2-b1) >> 1);

				const u8 rbot = r3 + ((r4-r3) >> 1);
				const u8 gbot = g3 + ((g4-g3) >> 1);
				const u8 bbot = b3 + ((b4-b3) >> 1);

				const u8 rfinal = rtop + ((rbot-rtop) >> 1);
				const u8 gfinal = gtop + ((gbot-gtop) >> 1);
				const u8 bfinal = btop + ((bbot-btop) >> 1);
				AGIDL_SetClr(clr_data,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,width,height);
			}
		}
	}
}

void AGIDL_FilterImgDataTrilerp(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* clr_data = data;
		COLOR* clr_cpy = malloc(sizeof(COLOR)*width*height);
		COLOR* temp = malloc(sizeof(COLOR)*width*height);

		AGIDL_ClrMemcpy(clr_cpy,clr_data,width*height);

		u16 w = width, h = height;

		COLOR* scale = AGIDL_ScaleImgDataBilerp(clr_cpy,&w,&h,2.0f,2.0f,fmt);

		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){

				float tempx = x/(float)width;
				float tempy = y/(float)height;

				float tempx2 = (x << 1)/(float)w;
				float tempy2 = (y << 1)/(float)h;

				float i = tempx * width - 0.5f;
				float j = tempy * height - 0.5f;
				float ii = tempx2 * w - 0.5f;
				float jj = tempy2 * h - 0.5f;

				i = AGIDL_Clampf(0,i,width);
				j = AGIDL_Clampf(0,j,height);
				ii = AGIDL_Clampf(0,ii,w);
				jj = AGIDL_Clampf(0,jj,h);

				float x_floor = floor(i);
				float x_floor2 = floor(ii);
				float x_ceil = ceil(i);
				float x_ceil2 = ceil(ii);
				float y_floor = floor(j);
				float y_floor2 = floor(jj);
				float y_ceil = ceil(j);
				float y_ceil2 = ceil(jj);

				if(ii > 0 && jj > 0){
					COLOR clr1 = AGIDL_GetClr(clr_data,x_floor,y_floor,width,height);
					COLOR clr2 = AGIDL_GetClr(clr_data,x_ceil,y_floor,width,height);
					COLOR clr3 = AGIDL_GetClr(clr_data,x_floor,y_ceil,width,height);
					COLOR clr4 = AGIDL_GetClr(clr_data,x_ceil,y_ceil,width,height);

					COLOR clr1s = AGIDL_GetClr(scale,x_floor2,y_floor2,w,h);
					COLOR clr2s = AGIDL_GetClr(scale,x_ceil2,y_floor2,w,h);
					COLOR clr3s = AGIDL_GetClr(scale,x_floor2,y_ceil2,w,h);
					COLOR clr4s = AGIDL_GetClr(scale,x_ceil2,y_ceil2,w,h);

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

					u8 rtop = r1 * (x_ceil - i) + r2 * (i - x_floor);
					u8 gtop = g1 * (x_ceil - i) + g2 * (i - x_floor);
					u8 btop = b1 * (x_ceil - i) + b2 * (i - x_floor);

					u8 rbot = r3 * (x_ceil - i) + r4 * (i - x_floor);
					u8 gbot = g3 * (x_ceil - i) + g4 * (i - x_floor);
					u8 bbot = b3 * (x_ceil - i) + b4 * (i - x_floor);

					u8 rbilerp1 = rtop * (y_ceil - j) + rbot * (j - y_floor);
					u8 gbilerp1 = gtop * (y_ceil - j) + gbot * (j - y_floor);
					u8 bbilerp1 = btop * (y_ceil - j) + bbot * (j - y_floor);

					r1 = AGIDL_GetR(clr1s,fmt);
					g1 = AGIDL_GetG(clr1s,fmt);
					b1 = AGIDL_GetB(clr1s,fmt);

					r2 = AGIDL_GetR(clr2s,fmt);
					g2 = AGIDL_GetG(clr2s,fmt);
					b2 = AGIDL_GetB(clr2s,fmt);

					r3 = AGIDL_GetR(clr3s,fmt);
					g3 = AGIDL_GetG(clr3s,fmt);
					b3 = AGIDL_GetB(clr3s,fmt);

					r4 = AGIDL_GetR(clr4s,fmt);
					g4 = AGIDL_GetG(clr4s,fmt);
					b4 = AGIDL_GetB(clr4s,fmt);

					rtop = r1 * (x_ceil2 - ii) + r2 * (ii - x_floor2);
					gtop = g1 * (x_ceil2 - ii) + g2 * (ii - x_floor2);
					btop = b1 * (x_ceil2 - ii) + b2 * (ii - x_floor2);

					rbot = r3 * (x_ceil2 - ii) + r4 * (ii - x_floor2);
					gbot = g3 * (x_ceil2 - ii) + g4 * (ii - x_floor2);
					bbot = b3 * (x_ceil2 - ii) + b4 * (ii - x_floor2);

					u8 rbilerp2 = rtop * (y_ceil2 - jj) + rbot * (jj - y_floor2);
					u8 gbilerp2 = gtop * (y_ceil2 - jj) + gbot * (jj - y_floor2);
					u8 bbilerp2 = btop * (y_ceil2 - jj) + bbot * (jj - y_floor2);

					u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
					u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
					u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);
					AGIDL_SetClr(temp,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,width,height);
				}
				else{
					COLOR clr1 = AGIDL_GetClr(clr_data,x,y,width,height);
					COLOR clr2 = AGIDL_GetClr(clr_data,x+1,y,width,height);
					COLOR clr3 = AGIDL_GetClr(clr_data,x,y+1,width,height);
					COLOR clr4 = AGIDL_GetClr(clr_data,x+1,y+1,width,height);

					COLOR clr1s = AGIDL_GetClr(scale,(x<<1),(y<<1),w,h);
					COLOR clr2s = AGIDL_GetClr(scale,(x<<1)+1,(y<<1),w,h);
					COLOR clr3s = AGIDL_GetClr(scale,(x<<1),(y<<1)+1,w,h);
					COLOR clr4s = AGIDL_GetClr(scale,(x<<1)+1,(y<<1)+1,w,h);

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
					r1 = AGIDL_GetR(clr1s,fmt);
					g1 = AGIDL_GetG(clr1s,fmt);
					b1 = AGIDL_GetB(clr1s,fmt);

					r2 = AGIDL_GetR(clr2s,fmt);
					g2 = AGIDL_GetG(clr2s,fmt);
					b2 = AGIDL_GetB(clr2s,fmt);

					r3 = AGIDL_GetR(clr3s,fmt);
					g3 = AGIDL_GetG(clr3s,fmt);
					b3 = AGIDL_GetB(clr3s,fmt);

					r4 = AGIDL_GetR(clr4s,fmt);
					g4 = AGIDL_GetG(clr4s,fmt);
					b4 = AGIDL_GetB(clr4s,fmt);

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
					AGIDL_SetClr(temp,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,width,height);
				}
			}
		}

		AGIDL_ClrMemcpy(clr_data,temp,width*height);

		free(temp);
		free(scale);
	}
	else{
		COLOR16* clr_data = data;
		COLOR16* clr_cpy = malloc(sizeof(COLOR16)*width*height);
		COLOR16* temp = malloc(sizeof(COLOR16)*width*height);

		AGIDL_ClrMemcpy16(clr_cpy,clr_data,width*height);

		u16 w = width, h = height;

		COLOR16* scale = AGIDL_ScaleImgDataBilerp(clr_cpy,&w,&h,2.0f,2.0f,fmt);

		int x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){

				float tempx = x/(float)width;
				float tempy = y/(float)height;

				float tempx2 = (x << 1)/(float)w;
				float tempy2 = (y << 1)/(float)h;

				float i = tempx * width - 0.5f;
				float j = tempy * height - 0.5f;
				float ii = tempx2 * w - 0.5f;
				float jj = tempy2 * h - 0.5f;

				i = AGIDL_Clampf(0,i,width);
				j = AGIDL_Clampf(0,j,height);
				ii = AGIDL_Clampf(0,ii,w);
				jj = AGIDL_Clampf(0,jj,h);

				float x_floor = floor(i);
				float x_floor2 = floor(ii);
				float x_ceil = ceil(i);
				float x_ceil2 = ceil(ii);
				float y_floor = floor(j);
				float y_floor2 = floor(jj);
				float y_ceil = ceil(j);
				float y_ceil2 = ceil(jj);

				if(ii > 0 && jj > 0){
					COLOR16 clr1 = AGIDL_GetClr16(clr_data,x_floor,y_floor,width,height);
					COLOR16 clr2 = AGIDL_GetClr16(clr_data,x_ceil,y_floor,width,height);
					COLOR16 clr3 = AGIDL_GetClr16(clr_data,x_floor,y_ceil,width,height);
					COLOR16 clr4 = AGIDL_GetClr16(clr_data,x_ceil,y_ceil,width,height);

					COLOR16 clr1s = AGIDL_GetClr16(scale,x_floor2,y_floor2,w,h);
					COLOR16 clr2s = AGIDL_GetClr16(scale,x_ceil2,y_floor2,w,h);
					COLOR16 clr3s = AGIDL_GetClr16(scale,x_floor2,y_ceil2,w,h);
					COLOR16 clr4s = AGIDL_GetClr16(scale,x_ceil2,y_ceil2,w,h);

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

					u8 rtop = r1 * (x_ceil - i) + r2 * (i - x_floor);
					u8 gtop = g1 * (x_ceil - i) + g2 * (i - x_floor);
					u8 btop = b1 * (x_ceil - i) + b2 * (i - x_floor);

					u8 rbot = r3 * (x_ceil - i) + r4 * (i - x_floor);
					u8 gbot = g3 * (x_ceil - i) + g4 * (i - x_floor);
					u8 bbot = b3 * (x_ceil - i) + b4 * (i - x_floor);

					u8 rbilerp1 = rtop * (y_ceil - j) + rbot * (j - y_floor);
					u8 gbilerp1 = gtop * (y_ceil - j) + gbot * (j - y_floor);
					u8 bbilerp1 = btop * (y_ceil - j) + bbot * (j - y_floor);

					r1 = AGIDL_GetR(clr1s,fmt);
					g1 = AGIDL_GetG(clr1s,fmt);
					b1 = AGIDL_GetB(clr1s,fmt);

					r2 = AGIDL_GetR(clr2s,fmt);
					g2 = AGIDL_GetG(clr2s,fmt);
					b2 = AGIDL_GetB(clr2s,fmt);

					r3 = AGIDL_GetR(clr3s,fmt);
					g3 = AGIDL_GetG(clr3s,fmt);
					b3 = AGIDL_GetB(clr3s,fmt);

					r4 = AGIDL_GetR(clr4s,fmt);
					g4 = AGIDL_GetG(clr4s,fmt);
					b4 = AGIDL_GetB(clr4s,fmt);

					rtop = r1 * (x_ceil2 - ii) + r2 * (ii - x_floor2);
					gtop = g1 * (x_ceil2 - ii) + g2 * (ii - x_floor2);
					btop = b1 * (x_ceil2 - ii) + b2 * (ii - x_floor2);

					rbot = r3 * (x_ceil2 - ii) + r4 * (ii - x_floor2);
					gbot = g3 * (x_ceil2 - ii) + g4 * (ii - x_floor2);
					bbot = b3 * (x_ceil2 - ii) + b4 * (ii - x_floor2);

					u8 rbilerp2 = rtop * (y_ceil2 - jj) + rbot * (jj - y_floor2);
					u8 gbilerp2 = gtop * (y_ceil2 - jj) + gbot * (jj - y_floor2);
					u8 bbilerp2 = btop * (y_ceil2 - jj) + bbot * (jj - y_floor2);

					u8 rfinal = rbilerp1 + ((rbilerp2-rbilerp1) >> 1);
					u8 gfinal = gbilerp1 + ((gbilerp2-gbilerp1) >> 1);
					u8 bfinal = bbilerp1 + ((bbilerp2-bbilerp1) >> 1);
					AGIDL_SetClr16(temp,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,width,height);
				}
				else{
					COLOR16 clr1 = AGIDL_GetClr16(clr_data,x,y,width,height);
					COLOR16 clr2 = AGIDL_GetClr16(clr_data,x+1,y,width,height);
					COLOR16 clr3 = AGIDL_GetClr16(clr_data,x,y+1,width,height);
					COLOR16 clr4 = AGIDL_GetClr16(clr_data,x+1,y+1,width,height);

					COLOR16 clr1s = AGIDL_GetClr16(scale,(x<<1),(y<<1),w,h);
					COLOR16 clr2s = AGIDL_GetClr16(scale,(x<<1)+1,(y<<1),w,h);
					COLOR16 clr3s = AGIDL_GetClr16(scale,(x<<1),(y<<1)+1,w,h);
					COLOR16 clr4s = AGIDL_GetClr16(scale,(x<<1)+1,(y<<1)+1,w,h);

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
					r1 = AGIDL_GetR(clr1s,fmt);
					g1 = AGIDL_GetG(clr1s,fmt);
					b1 = AGIDL_GetB(clr1s,fmt);

					r2 = AGIDL_GetR(clr2s,fmt);
					g2 = AGIDL_GetG(clr2s,fmt);
					b2 = AGIDL_GetB(clr2s,fmt);

					r3 = AGIDL_GetR(clr3s,fmt);
					g3 = AGIDL_GetG(clr3s,fmt);
					b3 = AGIDL_GetB(clr3s,fmt);

					r4 = AGIDL_GetR(clr4s,fmt);
					g4 = AGIDL_GetG(clr4s,fmt);
					b4 = AGIDL_GetB(clr4s,fmt);

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
					AGIDL_SetClr16(temp,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,width,height);
				}
			}
		}

		AGIDL_ClrMemcpy16(clr_data,temp,width*height);

		free(temp);
		free(scale);

	}
}

void AGIDL_FastFilterImgDataTrilerp(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* clrs = data;
		COLOR* clrscpy = malloc(sizeof(COLOR)*width*height);

		AGIDL_ClrMemcpy(clrscpy,clrs,width*height);

		u16 w = width, h = height;

		COLOR* clrscale = malloc(sizeof(COLOR)*w*h);
		clrscale = (COLOR*)AGIDL_ScaleImgDataNearest(clrscpy,&w,&h,2.0f,2.0f,fmt);

		AGIDL_FastFilterImgDataBilerp(clrscale,w,h,fmt);

		float x_scale = ((float)(w-1)/width);
		float y_scale = ((float)(h-1)/height);
		u32 x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){

				u16 xx = (x * x_scale);
				u16 yy = (y * y_scale);
				COLOR clr1 = AGIDL_GetClr(clrs,x,y,width,height);
				COLOR clr2 = AGIDL_GetClr(clrs,x+1,y,width,height);
				COLOR clr3 = AGIDL_GetClr(clrs,x,y+1,width,height);
				COLOR clr4 = AGIDL_GetClr(clrs,x+1,y+1,width,height);

				COLOR clr1s = AGIDL_GetClr(clrscale,xx,yy,w,h);
				COLOR clr2s = AGIDL_GetClr(clrscale,xx+1,yy,w,h);
				COLOR clr3s = AGIDL_GetClr(clrscale,xx,yy+1,w,h);
				COLOR clr4s = AGIDL_GetClr(clrscale,xx+1,yy+1,w,h);

				if(x == width-1){
					clr1 = AGIDL_GetClr(clrs,x,y,width,height);
					clr2 = AGIDL_GetClr(clrs,x-1,y,width,height);
					clr3 = AGIDL_GetClr(clrs,x,y+1,width,height);
					clr4 = AGIDL_GetClr(clrs,x-1,y+1,width,height);

					clr1s = AGIDL_GetClr(clrscale,xx,yy,w,h);
					clr2s = AGIDL_GetClr(clrscale,xx-1,yy,w,h);
					clr3s = AGIDL_GetClr(clrscale,xx,yy+1,w,h);
					clr4s = AGIDL_GetClr(clrscale,xx-1,yy+1,w,h);
				}

				if(y == height-1){
					clr1 = AGIDL_GetClr(clrs,x,y,width,height);
					clr2 = AGIDL_GetClr(clrs,x+1,y,width,height);
					clr3 = AGIDL_GetClr(clrs,x,y-1,width,height);
					clr4 = AGIDL_GetClr(clrs,x+1,y-1,width,height);

					clr1s = AGIDL_GetClr(clrscale,xx,yy,w,h);
					clr2s = AGIDL_GetClr(clrscale,xx+1,yy,w,h);
					clr3s = AGIDL_GetClr(clrscale,xx,yy-1,w,h);
					clr4s = AGIDL_GetClr(clrscale,xx+1,yy-1,w,h);
				}

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
				r1 = AGIDL_GetR(clr1s,fmt);
			    g1 = AGIDL_GetG(clr1s,fmt);
				b1 = AGIDL_GetB(clr1s,fmt);

				r2 = AGIDL_GetR(clr2s,fmt);
				g2 = AGIDL_GetG(clr2s,fmt);
				b2 = AGIDL_GetB(clr2s,fmt);

				r3 = AGIDL_GetR(clr3s,fmt);
				g3 = AGIDL_GetG(clr3s,fmt);
				b3 = AGIDL_GetB(clr3s,fmt);

				r4 = AGIDL_GetR(clr4s,fmt);
				g4 = AGIDL_GetG(clr4s,fmt);
				b4 = AGIDL_GetB(clr4s,fmt);

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
				AGIDL_SetClr(clrs,AGIDL_RGB(rfinal,gfinal,bfinal,fmt),x,y,width,height);
			}
		}

		free(clrscale);
	}
	else{
		COLOR16* clrs = data;
		COLOR16* clrscpy = malloc(sizeof(COLOR16)*width*height);

		AGIDL_ClrMemcpy16(clrscpy,clrs,width*height);

		u16 w = width, h = height;

		COLOR16* clrscale = malloc(sizeof(COLOR16)*w*h);
		clrscale = (COLOR16*)AGIDL_ScaleImgDataNearest(clrscpy,&w,&h,2.0f,2.0f,fmt);

		AGIDL_FastFilterImgDataBilerp(clrscale,w,h,fmt);

		float x_scale = ((float)(w-1)/width);
		float y_scale = ((float)(h-1)/height);
		u32 x,y;
		for(y = 0; y < height; y++){
			for(x = 0; x < width; x++){

				u16 xx = (x * x_scale);
				u16 yy = (y * y_scale);
				COLOR16 clr1 = AGIDL_GetClr16(clrs,x,y,width,height);
				COLOR16 clr2 = AGIDL_GetClr16(clrs,x+1,y,width,height);
				COLOR16 clr3 = AGIDL_GetClr16(clrs,x,y+1,width,height);
				COLOR16 clr4 = AGIDL_GetClr16(clrs,x+1,y+1,width,height);

				COLOR16 clr1s = AGIDL_GetClr16(clrscale,xx,yy,w,h);
				COLOR16 clr2s = AGIDL_GetClr16(clrscale,xx+1,yy,w,h);
				COLOR16 clr3s = AGIDL_GetClr16(clrscale,xx,yy+1,w,h);
				COLOR16 clr4s = AGIDL_GetClr16(clrscale,xx+1,yy+1,w,h);

				if(x == width-1){
					clr1 = AGIDL_GetClr16(clrs,x,y,width,height);
					clr2 = AGIDL_GetClr16(clrs,x-1,y,width,height);
					clr3 = AGIDL_GetClr16(clrs,x,y+1,width,height);
					clr4 = AGIDL_GetClr16(clrs,x-1,y+1,width,height);

					clr1s = AGIDL_GetClr16(clrscale,xx,yy,w,h);
					clr2s = AGIDL_GetClr16(clrscale,xx-1,yy,w,h);
					clr3s = AGIDL_GetClr16(clrscale,xx,yy+1,w,h);
					clr4s = AGIDL_GetClr16(clrscale,xx-1,yy+1,w,h);
				}

				if(y == height-1){
					clr1 = AGIDL_GetClr16(clrs,x,y,width,height);
					clr2 = AGIDL_GetClr16(clrs,x+1,y,width,height);
					clr3 = AGIDL_GetClr16(clrs,x,y-1,width,height);
					clr4 = AGIDL_GetClr16(clrs,x+1,y-1,width,height);

					clr1s = AGIDL_GetClr16(clrscale,xx,yy,w,h);
					clr2s = AGIDL_GetClr16(clrscale,xx+1,yy,w,h);
					clr3s = AGIDL_GetClr16(clrscale,xx,yy-1,w,h);
					clr4s = AGIDL_GetClr16(clrscale,xx+1,yy-1,w,h);
				}

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
				r1 = AGIDL_GetR(clr1s,fmt);
			    g1 = AGIDL_GetG(clr1s,fmt);
				b1 = AGIDL_GetB(clr1s,fmt);

				r2 = AGIDL_GetR(clr2s,fmt);
				g2 = AGIDL_GetG(clr2s,fmt);
				b2 = AGIDL_GetB(clr2s,fmt);

				r3 = AGIDL_GetR(clr3s,fmt);
				g3 = AGIDL_GetG(clr3s,fmt);
				b3 = AGIDL_GetB(clr3s,fmt);

				r4 = AGIDL_GetR(clr4s,fmt);
				g4 = AGIDL_GetG(clr4s,fmt);
				b4 = AGIDL_GetB(clr4s,fmt);

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
				
				AGIDL_SetClr16(clrs,AGIDL_RGB16(rfinal,gfinal,bfinal,fmt),x,y,width,height);
			}
		}
		
		free(clrscale);
	}
}
