#ifndef AGIDL_IMGP_LIGHTMAP_H
#define AGIDL_IMGP_LIGHTMAP_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_lightmap.h
*   Date: 12/10/2023
*   Version: 0.2b
*   Updated: 1/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_types.h>
#include <agidl_cc_types.h>
#include <agidl_img_types.h>

#define MAX_POINTS 512

typedef enum AGIDL_LIGHT{
	LIGHTMAP = 0x1,
	INVLIGHTMAP = 0x2,
}AGIDL_LIGHT;

typedef enum AGIDL_LIGHT_DIR{
	LIGHT_DIR_NE = 0x1,
	LIGHT_DIR_SE = 0x2,
	LIGHT_DIR_NW_TO_SE = 0x3,
	LIGHT_DIR_SE_TO_NW = 0x4,
}AGIDL_LIGHT_DIR;

typedef struct AGIDL_Point{
	u32 x,y;
	u8 size;
	float bias;
	int inv;
	AGIDL_LIGHT_DIR dir;
	COLOR clr;
}AGIDL_Point;

typedef struct AGIDL_LIGHTMAP{
	AGIDL_LIGHT light;
	u32 width, height;
	AGIDL_CLR_FMT fmt;
	AGIDL_Point points[MAX_POINTS];
	u32 num_of_points;
}AGIDL_LIGHTMAP;

void AGIDL_BindLightmapAndImg(void* data, void* lightdata, u32 width, u32 height, u32 widthl, u32 heightl, AGIDL_CLR_FMT imgfmt, AGIDL_CLR_FMT lightfmt, AGIDL_LIGHT light, COLOR blend);
void * AGIDL_GenerateLightmapImgData(AGIDL_LIGHTMAP lightmap);
AGIDL_Point AGIDL_CreateLightPoint(u32 x, u32 y, u8 size, float bias, int inv, AGIDL_LIGHT_DIR dir, COLOR clr);
void AGIDL_SetLight(AGIDL_LIGHTMAP* lightmap, AGIDL_LIGHT light);
void AGIDL_SetWidth(AGIDL_LIGHTMAP* lightmap, u32 width);
void AGIDL_SetHeight(AGIDL_LIGHTMAP* lightmap, u32 height);
void AGIDL_SetClrFmt(AGIDL_LIGHTMAP* lightmap, AGIDL_CLR_FMT fmt);
int IsInXRange(float bias, u32 x, u8 size, u32 width);
int IsInYRange(float bias, u32 y, u8 size, u32 height);

void AGIDL_InitLightmap(AGIDL_LIGHTMAP* lightmap, AGIDL_LIGHT light, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_AddLightPoint(AGIDL_LIGHTMAP* lightmap, AGIDL_Point point);

void AGIDL_FloodLightRectNE(COLOR* clrs, COLOR clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodLightRectSE(COLOR* clrs, COLOR clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodLightRectNWTOSE(COLOR* clrs, COLOR clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodLightRectSETONW(COLOR* clrs, COLOR clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);

void AGIDL_FloodInvLightRectNE(COLOR* clrs, COLOR clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodInvLightRectSE(COLOR* clrs, COLOR clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodInvLightRectNWTOSE(COLOR* clrs, COLOR clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodInvLightRectSETONW(COLOR* clrs, COLOR clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);

void AGIDL_FloodLightRectNE16(COLOR16* clrs, COLOR16 clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodLightRectSE16(COLOR16* clrs, COLOR16 clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodLightRectNWTOSE16(COLOR16* clrs, COLOR16 clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodLightRectSETONW16(COLOR16* clrs, COLOR16 clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);

void AGIDL_FloodInvLightRectNE16(COLOR16* clrs, COLOR16 clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodInvLightRectSE16(COLOR16* clrs, COLOR16 clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodInvLightRectNWTOSE16(COLOR16* clrs, COLOR16 clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);
void AGIDL_FloodInvLightRectSETONW16(COLOR16* clrs, COLOR16 clr, float clrfactor, AGIDL_CLR_FMT fmt, u32 x, u32 y, u32 width, u32 height, float bias, u8 size);

void AGIDL_RemoveLightPoint(AGIDL_LIGHTMAP* lightmap, u32 index);

#endif