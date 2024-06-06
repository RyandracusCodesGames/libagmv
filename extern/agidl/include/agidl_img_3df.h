#ifndef AGIDL_IMG_3DF_H
#define AGIDL_IMG_3DF_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_3df.h
*   Date: 2/12/2024
*   Version: 0.3b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdio.h>
#include "agidl_types.h"
#include "agidl_cc_types.h"
#include "agidl_img_types.h"
#include "agidl_cc_manager.h"
#include "agidl_imgp_mipmap.h"

typedef enum GLIDE_3DF_CLR_FMT{
	GLIDE_I8 = 0x1,
	GLIDE_A8 = 0x2,
	GLIDE_AI44 = 0x3,
	GLIDE_YIQ = 0x4,
	GLIDE_RGB_332 = 0x5,
	GLIDE_RGB_565 = 0x7,
	GLIDE_ARGB_8332 = 0x8,
	GLIDE_ARGB_1555 = 0x9,
	GLIDE_AYIQ8_422 = 0x0A,
	GLIDE_ARGB_4444 = 0x0B,
	GLIDE_AI_88 = 0x0C,
	GLIDE_ARGB_8888 = 0x0D,
	GLIDE_P8 = 0x0F,
	GLIDE_UNKNOWN_FMT = 44,
}GLIDE_3DF_CLR_FMT;

typedef enum GLIDE_ASPECT_TABLE{
	GLIDE_ASPECT_1x1 = 0x1,
	GLIDE_ASPECT_1x2 = 0x2,
	GLIDE_ASPECT_1x4 = 0x3,
	GLIDE_ASPECT_1x8 = 0x4,
	GLIDE_ASPECT_2x1 = 0x5,
	GLIDE_ASPECT_4x1 = 0x6,
	GLIDE_ASPECT_8x1 = 0x7,
	GLIDE_UNKNOWN_ASPECT = 0x8,
}GLIDE_ASPECT_TABLE;

typedef struct GLIDE_3DF_HEADER{
	char magic[4];
	char version[5];
	GLIDE_3DF_CLR_FMT fmt;
	u16 little_lod;
	u16 large_lod;
	GLIDE_ASPECT_TABLE aspect;
}GLIDE_3DF_HEADER;

typedef struct AGIDL_3DF{
	GLIDE_3DF_HEADER header;
	u16 width;
	u16 height;
	AGIDL_CLR_FMT fmt;
	AGIDL_Bool icp;
	AGIDL_ICP_ENCODE encode;
	AGIDL_ICP palette;
	u16 max_diff;
	Pixels pixels;
	AGIDL_MIPMAP* mipmap;
	AGIDL_Bool mipped;
	char* filename;
}AGIDL_3DF;

void AGIDL_Set3DFFilename(AGIDL_3DF* glide, const char* filename);
void AGIDL_3DFSetWidth(AGIDL_3DF* glide, int width);
void AGIDL_3DFSetHeight(AGIDL_3DF* glide, int height);
void AGIDL_3DFSetClrFmt(AGIDL_3DF* glide, AGIDL_CLR_FMT fmt);
void AGIDL_3DFSetICPMode(AGIDL_3DF* glide, AGIDL_Bool icp);
void AGIDL_3DFSetICPEncoding(AGIDL_3DF* glide, AGIDL_ICP_ENCODE encode);
void AGIDL_3DFSetMaxDiff(AGIDL_3DF* glide, int max_diff);
void AGIDL_3DFBuildMipmap(AGIDL_3DF* glide, AGIDL_Bool mipped);
void AGIDL_3DFSetClr(AGIDL_3DF* glide, int x, int y, COLOR clr);
void AGIDL_3DFSetClr16(AGIDL_3DF* glide, int x, int y, COLOR16 clr);
void AGIDL_3DFSetRGB(AGIDL_3DF* glide, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_3DFSetRGBA(AGIDL_3DF* glide, int x, int y, u8 r, u8 g, u8 b, u8 a);
void AGIDL_3DFSetColor3f(AGIDL_3DF* glide, int x, int y, f32 r, f32 g, f32 b);
void AGIDL_3DFSetColor4f(AGIDL_3DF* glide, int x, int y, f32 r, f32 g, f32 b, f32 a);
void AGIDL_Clear3DF(AGIDL_3DF* glide, COLOR clr);
void AGIDL_Clear3DF16(AGIDL_3DF* glide, COLOR16 clr);
void AGIDL_ClearColor3DF(AGIDL_3DF* glide, float r, float g, float b);
void AGIDL_Flush3DF(AGIDL_3DF* glide);
u32 AGIDL_3DFGetWidth(AGIDL_3DF* glide);
u32 AGIDL_3DFGetHeight(AGIDL_3DF* glide);
u32 AGIDL_3DFGetSize(AGIDL_3DF* glide);
AGIDL_CLR_FMT AGIDL_3DFGetClrFmt(AGIDL_3DF* glide);
int AGIDL_3DFGetMaxDiff(AGIDL_3DF* glide);
COLOR AGIDL_3DFGetClr(AGIDL_3DF* glide, int x, int y);
COLOR16 AGIDL_3DFGetClr16(AGIDL_3DF* glide, int, int y);
void AGIDL_3DFSyncPix(AGIDL_3DF *glide, COLOR *clrs);
void AGIDL_3DFSyncPix16(AGIDL_3DF *glide, COLOR16 *clrs);
void AGIDL_3DFCopyPix(AGIDL_3DF* glide, COLOR* clrs, u32 count);
void AGIDL_3DFCopyPix16(AGIDL_3DF* glide, COLOR16* clrs, u32 count);
void AGIDL_Free3DF(AGIDL_3DF* glide);
void AGIDL_3DFRGB2BGR(AGIDL_3DF* glide);
void AGIDL_3DFBGR2RGB(AGIDL_3DF* glide);
void AGIDL_3DFConvert24BPPTO16BPP(AGIDL_3DF* glide);
void AGIDL_3DFConvert16BPPTO24BPP(AGIDL_3DF* glide);
void AGIDL_3DFConvertRGBA2RGB(AGIDL_3DF* glide);
void AGIDL_3DF555TO565(AGIDL_3DF* glide);
void AGIDL_3DF565TO555(AGIDL_3DF* glide);
void AGIDL_3DFConvertRGBA2RGB(AGIDL_3DF* glide);
void AGIDL_3DFConvertRGB2RGBA(AGIDL_3DF* glide);
void AGIDL_ColorConvert3DF(AGIDL_3DF* glide, AGIDL_CLR_FMT dest);
int AGIDL_3DFDecodeHeader(AGIDL_3DF* glide, FILE* file);
int AGIDL_3DFDecodePartialHeader(AGIDL_3DF* glide, FILE* file);
void AGIDL_3DFDecodeIMG(AGIDL_3DF* glide, FILE* file);
void AGIDL_3DFEncodeHeader(AGIDL_3DF* glide, FILE* file);
void AGIDL_3DFEncodeICP(AGIDL_3DF* glide, FILE* file);
void AGIDL_3DFEncodeNearestICP(AGIDL_3DF* glide, AGIDL_ICP palette, FILE* file);
void AGIDL_3DFEncodeIMG(AGIDL_3DF* glide, FILE* file);

AGIDL_3DF * AGIDL_Load3DF(char *filename);
AGIDL_3DF * AGIDL_Create3DF(const char *filename, int width, int height, AGIDL_CLR_FMT fmt);
AGIDL_3DF * AGIDL_3DFCpyImg(AGIDL_3DF* glide);
void AGIDL_Export3DF(AGIDL_3DF *glide);

GLIDE_3DF_CLR_FMT AGIDL_3DFGetGlideClrFmt(char format[10]);
GLIDE_ASPECT_TABLE AGIDL_3DFGetAspect(char aspect[3]);
u8 AGIDL_GetCharNum(char num);

#endif