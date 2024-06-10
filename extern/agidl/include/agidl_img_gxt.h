#ifndef AGIDL_IMG_GXT_H
#define AGIDL_IMG_GXT_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_gxt.h
*   Date: 11/19/2023
*   Version: 0.1b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agidl_types.h>
#include <agidl_cc_types.h>
#include <agidl_img_types.h>

#define GXT_TEXTURE_BASE_FORMAT 0x0010

#define UNSUPPORTED_PVRT 0x78

typedef enum GXT_TEXTURE_TYPE{
	SWIZZLED = 0x00,
	CUBE = 0x40,
	LINEAR = 0x60,
	TILED = 0x80,
	LINEAR_STRIDDED = 0x0C,
}GXT_TEXTURE_TYPE;

typedef enum GXT_CLR_FMT{
	GXT_RGB_888 = 0x98,
	GXT_ICP_256 = 0x95,
	GXT_RGB_565 = 0x05,
	GXT_RGB_555 = 0x04,
	GXT_RGB_4444 = 0x02,
	GXT_ARGB_8888 = 0x0C,
	GXT_ARGB_4444 = 0x10,
	GXT_PVRT2BPP = 0x80,
	GXT_PVRT4BPP = 0x81,
}GXT_CLR_FMT;

typedef struct GXT_TEXTURE_HEADER{
	u32 offset;
	u32 size;
	u32 index;
	u32 tex_flags;
	GXT_TEXTURE_TYPE type;
	GXT_CLR_FMT fmt;
	u32 width;
	u32 height;
	u16 mipmaps;
	u16 blank;
}GXT_TEXTURE_HEADER;	

typedef struct GXT_HEADER{
	u32 magic;
	u32 version;
	u32 num_of_tex;
	u32 offset;
	u32 size_of_tex;
	u32 num_of_icp_16;
	u32 num_of_icp_256;
	u32 blank;
	GXT_TEXTURE_HEADER header;
}GXT_HEADER;

typedef struct AGIDL_GXT{
	GXT_HEADER header;
	int icp;
	int max_diff;
	AGIDL_ICP palette;
	AGIDL_CLR_FMT fmt;
	Pixels pixels;
	char* filename;
}AGIDL_GXT;

void AGIDL_SetGXTFilename(AGIDL_GXT* gtx, const char* filename);
void AGIDL_GXTSetWidth(AGIDL_GXT* gxt, u32 width);
void AGIDL_GXTSetHeight(AGIDL_GXT* gxt, u32 height);
void AGIDL_GXTSetClrFmt(AGIDL_GXT* gxt, AGIDL_CLR_FMT fmt);
void AGIDL_GXTSetICPMode(AGIDL_GXT* gxt, int mode);
void AGIDL_GXTSetMaxDiff(AGIDL_GXT* gxt, int max_diff);
void AGIDL_GXTSetClr(AGIDL_GXT* gxt, int x, int y, COLOR clr);
void AGIDL_GXTSetClr16(AGIDL_GXT* gxt, int x, int y, COLOR16 clr);
void AGIDL_GXTSetRGB(AGIDL_GXT* gxt, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_ClearGXT(AGIDL_GXT* gxt, COLOR clr);
void AGIDL_ClearGXT16(AGIDL_GXT* gxt, COLOR16 clr);
void AGIDL_ClearColorGXT(AGIDL_GXT* gxt, float r, float g, float b);
void AGIDL_FlushGXT(AGIDL_GXT* gxt);
u32 AGIDL_GXTGetWidth(AGIDL_GXT* gxt);
u32 AGIDL_GXTGetHeight(AGIDL_GXT* gxt);
u32 AGIDL_GXTGetSize(AGIDL_GXT* gxt);
AGIDL_CLR_FMT AGIDL_GXTGetClrFmt(AGIDL_GXT* gxt);
int AGIDL_GXTGetMaxDiff(AGIDL_GXT* gxt);
COLOR AGIDL_GXTGetClr(AGIDL_GXT* gxt, int x, int y);
COLOR16 AGIDL_GXTGetClr16(AGIDL_GXT* gxt, int x, int y);
void AGIDL_GXTRGB2BGR(AGIDL_GXT* gxt);
void AGIDL_GXTBGR2RGB(AGIDL_GXT* gxt);
void AGIDL_GXTConvert24BPPTO16BPP(AGIDL_GXT* gxt);
void AGIDL_GXTConvert16BPPTO24BPP(AGIDL_GXT* gxt);
void AGIDL_GXT555TO565(AGIDL_GXT* gxt);
void AGIDL_GXT565TO555(AGIDL_GXT* gxt);
void AGIDL_ColorConvertGXT(AGIDL_GXT* gxt, AGIDL_CLR_FMT dest);
void AGIDL_GXTSyncPix(AGIDL_GXT *gxt, COLOR *clrs);
void AGIDL_GXTSyncPix16(AGIDL_GXT *gxt, COLOR16 *clrs);
void AGIDL_GXTCopyPix(AGIDL_GXT* gxt, COLOR* clrs, u32 count);
void AGIDL_GXTCopyPix16(AGIDL_GXT* gxt, COLOR16* clrs, u32 count);
AGIDL_GXT* AGIDL_LoadGXT(char* filename);
AGIDL_GXT* AGIDL_CreateGXT(const char* filename, int width, int height, AGIDL_CLR_FMT fmt);
AGIDL_GXT* AGIDL_GXTCpyImg(AGIDL_GXT* gxt);
void AGIDL_ExportGXT(AGIDL_GXT* gxt);
void AGIDL_FreeGXT(AGIDL_GXT* gxt);
GXT_CLR_FMT AGIDL_GetGXTClrFmt(u8 type);
GXT_TEXTURE_TYPE AGIDL_GXTGetTextureType(u8 type);
int AGIDL_GXTDecodeHeader(AGIDL_GXT* gxt, FILE* file);
int AGIDL_GXTDecodeTextureHeader(AGIDL_GXT* gxt, FILE* file);
void AGIDL_GXTDecodeIMG(AGIDL_GXT* gxt, FILE* file);
void AGIDL_GXTDecodeSwizzledIMG(AGIDL_GXT* gxt, FILE* file);
void AGIDL_GXTEncodeHeader(AGIDL_GXT* gxt, FILE* file);
void AGIDL_GXTEncodeTextureHeader(AGIDL_GXT* gxt, FILE* file);
void AGIDL_GXTEncodeICP(AGIDL_GXT* gxt, FILE* file);
void AGIDL_GXTEncodeIMG(AGIDL_GXT* gxt, FILE* file);
#endif