#ifndef AGIDL_IMG_TIM_H
#define AGIDL_IMG_TIM_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_tim.h
*   Date: 9/19/2023
*   Version: 0.1b
*   Updated: 2/26/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include "agidl_types.h"
#include "agidl_cc_types.h"
#include "agidl_img_types.h"
#include "agidl_cc_manager.h"

#define TIM_MAGIC 0x10

typedef enum TIM_TYPE{
	TIM_4BPP = 0x08,
	TIM_8BPP = 0x09,
	TIM_16BPP = 0x02,
	TIM_24BPP = 0x03,
}TIM_TYPE;

typedef struct TIM_HEADER{
	u32 magic;
	u32 version;
}TIM_HEADER;

typedef struct CLUT_HEADER{
	u32 clut_size;
	u16 pal_mem_add_x;
	u16 pal_mem_add_y;
	u16 num_clrs;
	u16 num_icps;
}CLUT_HEADER;

typedef struct IMG_HEADER{
	u32 img_size;
	u16 img_mem_add_x;
	u16 img_mem_add_y;
	u16 width;
	u16 height;
}IMG_HEADER;

typedef struct AGIDL_TIM{
	TIM_HEADER header;
	CLUT_HEADER clut_header;
	int icp;
	int max_diff;
	AGIDL_ICP palette;
	AGIDL_CLR_FMT fmt;
	IMG_HEADER img_header;
	Pixels pixels;
	AGIDL_ICP_ENCODE encode;
	AGIDL_Bool force4bpp;
	char *filename;
}AGIDL_TIM;

void AGIDL_SetTIMFilename(AGIDL_TIM *tim, const char *filename);
void AGIDL_TIMSetWidth(AGIDL_TIM *tim, int width);
void AGIDL_TIMSetHeight(AGIDL_TIM *tim, int height);
void AGIDL_TIMSetClrFmt(AGIDL_TIM *tim, AGIDL_CLR_FMT fmt);
void AGIDL_TIMSetClr(AGIDL_TIM *tim, int x, int y, COLOR clr);
void AGIDL_TIMSetClr16(AGIDL_TIM *tim, int x, int y, COLOR16 clr);
void AGIDL_TIMSetRGB(AGIDL_TIM *tim, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_TIMSetICPMode(AGIDL_TIM *tim, int mode);
void AGIDL_TIMSetICPEncoding(AGIDL_TIM* tim, AGIDL_ICP_ENCODE encode);
void AGIDL_TIMForce4BPPEncoding(AGIDL_TIM* tim, AGIDL_Bool force4bpp);
void AGIDL_TIMInitAttributes(AGIDL_TIM* tim, u16 pal_mem_add_x, u16 pal_mem_add_y, u16 img_mem_add_x, u16 img_mem_add_y);
void AGIDL_TIMSetMaxDiff(AGIDL_TIM *tim, int max_diff);
void AGIDL_ClearTIM(AGIDL_TIM *tim, COLOR clr);
void AGIDL_ClearTIM16(AGIDL_TIM *tim, COLOR16 clr);
void AGIDL_ClearColorTIM(AGIDL_TIM* tim, float r, float g, float b);
void AGIDL_FlushTIM(AGIDL_TIM* tim);
int AGIDL_TIMGetWidth(AGIDL_TIM *tim);
int AGIDL_TIMGetHeight(AGIDL_TIM *tim);
u32 AGIDL_TIMGetSize(AGIDL_TIM* tim);
int AGIDL_TIMGetMaxDiff(AGIDL_TIM *tim);
AGIDL_CLR_FMT AGIDL_TIMGetClrFmt(AGIDL_TIM *tim);
COLOR AGIDL_TIMGetClr(AGIDL_TIM *tim, int x, int y);
COLOR16 AGIDL_TIMGetClr16(AGIDL_TIM *tim, int x, int y);
void AGIDL_FreeTIM(AGIDL_TIM *tim);
void AGIDL_TIMRGB2BGR(AGIDL_TIM *tim);
void AGIDL_TIMBGR2RGB(AGIDL_TIM *tim);
void AGIDL_TIMConvert16BPPTO24BPP(AGIDL_TIM *tim);
void AGIDL_TIMConvert24BPPTO16BPP(AGIDL_TIM *tim);
void AGIDL_TIMConvert555TO565(AGIDL_TIM *tim);
void AGIDL_TIMConvert565TO555(AGIDL_TIM *tim);
void AGIDL_ColorConvertTIM(AGIDL_TIM* tim, AGIDL_CLR_FMT dest);
void AGIDL_TIMSyncPix(AGIDL_TIM *tim, COLOR *clrs);
void AGIDL_TIMSyncPix16(AGIDL_TIM *tim, COLOR16 *clrs);
void AGIDL_TIMCopyPix(AGIDL_TIM* tim, COLOR* clrs, u32 count);
void AGIDL_TIMCopyPix16(AGIDL_TIM* tim, COLOR16* clrs, u32 count);
AGIDL_TIM * AGIDL_LoadTIM(char *filename);
AGIDL_TIM * AGIDL_CreateTIM(const char *filename, int width, int height, AGIDL_CLR_FMT fmt);
void AGIDL_ExportTIM(AGIDL_TIM *tim);
AGIDL_TIM* AGIDL_TIMCpyImg(AGIDL_TIM* tim);
void AGIDL_TIMEncodeHeader(AGIDL_TIM* tim, FILE* file);
void AGIDL_TIMEncodeICP(AGIDL_TIM* tim);
void AGIDL_TIMEncodeIMG(AGIDL_TIM* tim, FILE* file);
int AGIDL_TIMDecodeHeader(AGIDL_TIM* tim, FILE* file);
int AGIDL_TIMDecodeIMG(AGIDL_TIM* tim, FILE* file);
void AGIDL_TIMEncodeNearestICP(AGIDL_TIM* tim, AGIDL_ICP palette, FILE* file);
int AGIDL_IsTIM(AGIDL_TIM* tim);
int AGIDL_IsTIMHeader(AGIDL_TIM* tim);
#endif