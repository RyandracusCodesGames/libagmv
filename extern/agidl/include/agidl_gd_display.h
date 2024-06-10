#ifndef AGIDL_GD_DISPLAY_H
#define AGIDL_GD_DISPLAY_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_gd_display.h
*   Date: 2/1/2024
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_img_core.h>
#include <agidl_types.h>

typedef struct AGIDL_ATTR{
	int tx, ty;
	f32 sx, sy;
	AGIDL_Bool enableScale;
	AGIDL_Bool enableChroma;
	u32 chromaKey;
	AGIDL_Bool flipHorz;
	AGIDL_Bool flipVert;
}AGIDL_ATTR;

void AGIDL_DispImgData(void* vram, u32 width, u32 height, AGIDL_ATTR table, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt);
void AGIDL_DrawImgData(void* vram, u32 width, u32 height, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt);
void AGIDL_DispScaleImgData(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt);
void AGIDL_DispFlipHorzImgData(void* vram, u32 width, u32 height, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt);
void AGIDL_DispFlipVertImgData(void* vram, u32 width, u32 height, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt);
void AGIDL_DispFlipHorzAndVertImgData(void* vram, u32 width, u32 height, int tx, int ty, const void* spr, u32 widths, u32 heights, AGIDL_CLR_FMT fmt);

void AGIDL_DispBMP(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_BMP* bmp);
void AGIDL_DispTGA(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_TGA* tga);
void AGIDL_DispTIM(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_TIM* tim);
void AGIDL_DispPCX(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_PCX* pcx);
void AGIDL_DispLMP(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_LMP* lmp);
void AGIDL_DispPVR(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_PVR* pvr);
void AGIDL_DispGXT(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_GXT* gxt);
void AGIDL_DispBTI(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_BTI* bti);
void AGIDL_Disp3DF(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_3DF* glide);
void AGIDL_DispPPM(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_PPM* ppm);
void AGIDL_DispLBM(void* vram, u32 width, u32 height, AGIDL_ATTR table,  AGIDL_LBM* lbm);

void AGIDL_DrawBMP(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_BMP* bmp);
void AGIDL_DrawTGA(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_TGA* tga);
void AGIDL_DrawTIM(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_TIM* tim);
void AGIDL_DrawPCX(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_PCX* pcx);
void AGIDL_DrawLMP(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_LMP* lmp);
void AGIDL_DrawPVR(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_PVR* pvr);
void AGIDL_DrawGXT(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_GXT* gxt);
void AGIDL_DrawBTI(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_BTI* bti);
void AGIDL_Draw3DF(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_3DF* glide);
void AGIDL_DrawPPM(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_PPM* ppm);
void AGIDL_DrawLBM(void* vram, u32 width, u32 height, int tx, int ty,  AGIDL_LBM* lbm);

void AGIDL_DispScaleBMP(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_BMP* bmp);
void AGIDL_DispScaleTGA(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_TGA* tga);
void AGIDL_DispScaleTIM(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_TIM* tim);
void AGIDL_DispScalePCX(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_PCX* pcx);
void AGIDL_DispScaleLMP(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_LMP* lmp);
void AGIDL_DispScalePVR(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_PVR* pvr);
void AGIDL_DispScaleGXT(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_GXT* gxt);
void AGIDL_DispScaleBTI(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_BTI* bti);
void AGIDL_DispScale3DF(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_3DF* glide);
void AGIDL_DispScalePPM(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_PPM* ppm);
void AGIDL_DispScaleLBM(void* vram, u32 width, u32 height, f32 sx, f32 sy, int tx, int ty,  AGIDL_LBM* lbm);

#endif