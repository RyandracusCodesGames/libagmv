#ifndef AGIDL_IMG_TGA_H
#define AGIDL_IMG_TGA_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_tga.h
*   Date: 9/13/2023
*   Version: 0.1b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_manager.h>
#include <agidl_cc_types.h>
#include <agidl_img_types.h>
#include <agidl_types.h>

#include <stdio.h>

typedef enum TGA_ICP_TYPE{
	TGA_IMG_TYPE_ICP = 1,
	TGA_IMG_TYPE_NO_ICP = 2,
	TGA_IMG_TYPE_RLE_ICP = 9,
	TGA_IMG_TYPE_RLE_NO_ICP = 10,
}TGA_ICP_TYPE;

typedef enum TGA_IMG_TYPE{
	TGA_IMG_TYPE_ICP_CLR = 8,
	TGA_IMG_TYPE_HIGH_CLR = 16,
	TGA_IMG_TYPE_TRUE_CLR = 24,
	TGA_IMG_TYPE_DEEP_CLR = 32,
}TGA_IMG_TYPE;

typedef struct TGA_HEADER{
	u8 id;
	u8 clrmaptype;
	u8 imgtype;
	u16 clrmapstart;
	u16 clrmaplength;
	u8 icpbits;
	u16 xstart;
	u16 ystart;
	u16 width;
	u16 height;
	u8 bits;
	u8 flip;
}TGA_HEADER;

typedef struct AGIDL_TGA{
	TGA_HEADER header;
	int icp;
	int compression;
	int max_diff;
	AGIDL_ICP palette;
	AGIDL_CLR_FMT fmt;
	Pixels pixels;
	AGIDL_ICP_ENCODE encode;
	char *filename;
}AGIDL_TGA;

void AGIDL_SetTGAFilename(AGIDL_TGA *tga, const char *filename);
void AGIDL_TGASetWidth(AGIDL_TGA *tga, int width);
void AGIDL_TGASetHeight(AGIDL_TGA *tga, int height);
void AGIDL_TGASetClrFmt(AGIDL_TGA *tga, AGIDL_CLR_FMT fmt);
void AGIDL_TGASetClr(const AGIDL_TGA *tga, int x, int y, COLOR clr);
void AGIDL_TGASetClr16(const AGIDL_TGA *tga, int x, int y, COLOR16 clr);
void AGIDL_TGASetRGB(const AGIDL_TGA *tga, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_TGASetRGBA(const AGIDL_TGA* tga, int x, int y, u8 r, u8 g, u8 b, u8 a);
void AGIDL_TGASetMaxDiff(AGIDL_TGA* tga, int max_diff);
void AGIDL_TGASetICPMode(AGIDL_TGA *tga, int mode);
void AGIDL_TGASetICPEncoding(AGIDL_TGA* tga, AGIDL_ICP_ENCODE encode);
void AGIDL_TGASetCompression(AGIDL_TGA* tga, int compress);
void AGIDL_ClearTGA(const AGIDL_TGA *tga, COLOR clr);
void AGIDL_ClearTGA16(const AGIDL_TGA *tga, COLOR16 clr);
void AGIDL_ClearColorTGA(const AGIDL_TGA* tga, float r, float g, float b);
void AGIDL_FlushTGA(const AGIDL_TGA* tga);
int AGIDL_TGAGetWidth(const AGIDL_TGA *tga);
int AGIDL_TGAGetHeight(const AGIDL_TGA *tga);
u32 AGIDL_TGAGetSize(const AGIDL_TGA* tga);
int AGIDL_TGAGetMaxDiff(const AGIDL_TGA *tga);
AGIDL_CLR_FMT AGIDL_TGAGetClrFmt(const AGIDL_TGA* tga);
COLOR AGIDL_TGAGetClr(const AGIDL_TGA *tga, int x, int y);
COLOR16 AGIDL_TGAGetClr16(const AGIDL_TGA *tga, int x, int y);
void AGIDL_FreeTGA(AGIDL_TGA *tga);
void AGIDL_TGARGB2BGR(AGIDL_TGA *tga);
void AGIDL_TGABGR2RGB(AGIDL_TGA *tga);
void AGIDL_TGAConvert16BPPTO24BPP(AGIDL_TGA *tga);
void AGIDL_TGAConvert24BPPTO16BPP(AGIDL_TGA *tga);
void AGIDL_TGAConvert555TO565(AGIDL_TGA *tga);
void AGIDL_TGAConvert565TO555(AGIDL_TGA *tga);
void AGIDL_ColorConvertTGA(AGIDL_TGA* tga, AGIDL_CLR_FMT dest);
void AGIDL_TGASyncPix(const AGIDL_TGA *tga, const COLOR *clrs);
void AGIDL_TGASyncPix16(const AGIDL_TGA *tga, const COLOR16 *clrs);
void AGIDL_TGACopyPix(const AGIDL_TGA* tga, const COLOR* clrs, u32 count);
void AGIDL_TGACopyPix16(const AGIDL_TGA* tga, const COLOR16* clrs, u32 count);
AGIDL_TGA * AGIDL_LoadTGA(char *filename);
AGIDL_TGA * AGIDL_CreateTGA(const char *filename, int width, int height, AGIDL_CLR_FMT fmt);
void AGIDL_ExportTGA(AGIDL_TGA *tga);
TGA_ICP_TYPE AGIDL_TGAGetICPType(int num);
TGA_IMG_TYPE AGIDL_TGAGetIMGType(int bits);
int AGIDL_IsTGA(const AGIDL_TGA* tga);
int AGIDL_TGADecodeHeader(AGIDL_TGA* tga, FILE* file);
void AGIDL_TGAEncodeNearestICP(const AGIDL_TGA* tga, AGIDL_ICP palette, FILE* file);
void AGIDL_TGADecodeIMG(AGIDL_TGA *tga, FILE* file, TGA_ICP_TYPE icp, TGA_IMG_TYPE img_type);
void AGIDL_TGADecodeRLE(AGIDL_TGA *tga, FILE* file, TGA_ICP_TYPE icp, TGA_IMG_TYPE img_type);
void AGIDL_TGAEncodeHeader(AGIDL_TGA* tga, FILE* file);
void AGIDL_TGAEncodeIMG(const AGIDL_TGA* tga, FILE* file);
void AGIDL_TGAEncodeICP(AGIDL_TGA* tga, FILE* file);
void AGIDL_TGAEncodeRLE(const AGIDL_TGA* tga, FILE* file);
AGIDL_TGA* AGIDL_TGACpyImg(const AGIDL_TGA* tga);
#endif
