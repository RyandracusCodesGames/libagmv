#ifndef AGIDL_IMG_BMP_H
#define AGIDL_IMG_BMP_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_bmp.h
*   Date: 9/11/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agidl_types.h>
#include <agidl_cc_types.h>
#include <agidl_cc_manager.h>
#include <agidl_img_types.h>

#define BI_RGB 0
#define BI_RLE8 1
#define BI_RLE4 2

typedef enum BMP_IMG_TYPE{
	BMP_IMG_TYPE_HIGH_CLR = 16,
	BMP_IMG_TYPE_TRUE_CLR = 24,
	BMP_IMG_TYPE_DEEP_CLR = 32,
	BMP_IMG_TYPE_ICP_256 = 8,
	BMP_IMG_TYPE_ICP_16 = 4,
	BMP_IMG_TYPE_RLE = 1,
	BMP_IMG_TYPE_1BPP_ICP = 1,
	BMP_IMG_TYPE_2BPP_ICP = 2,
}BMP_IMG_TYPE;

typedef struct BMP_HEADER{
	u16 magic;
	u32 file_size;
	u16 reserved1;
	u16 reserved2;
	u32 offset;
	u32 header_size;
	u32 width;
	u32 height;
	u16 num_of_planes;
	u16 bits;
	u32 compress;
	u32 img_size;
	u32 x_resolution;
	u32 y_resolution;
	u32 num_of_colors;
	u32 important_colors;
}BMP_HEADER;

typedef struct AGIDL_BMP{
	BMP_HEADER header;
	int icp;
	int compression;
	u16 max_diff;
	AGIDL_ICP palette;
	AGIDL_CLR_FMT fmt;
	Pixels pixels;
	AGIDL_Bool IsOS2;
	AGIDL_ICP_ENCODE encode;
	char *filename;
}AGIDL_BMP;

void AGIDL_SetBMPFilename(AGIDL_BMP *bmp, const char* filename);
AGIDL_BMP * AGIDL_LoadBMP(char *filename); 
AGIDL_BMP * AGIDL_CreateBMP(const char *filename, int width, int height, AGIDL_CLR_FMT fmt);
void AGIDL_ExportBMP(AGIDL_BMP *bmp);
COLOR AGIDL_BMPGetClr(AGIDL_BMP *bmp, int x, int y);
COLOR16 AGIDL_BMPGetClr16(AGIDL_BMP *bmp, int x, int y);
void AGIDL_BMPSyncPix(AGIDL_BMP *bmp, COLOR *clrs);
void AGIDL_BMPSyncPix16(AGIDL_BMP *bmp, COLOR16 *clrs);
void AGIDL_BMPCopyPix(AGIDL_BMP* bmp, COLOR* clrs, u32 count);
void AGIDL_BMPCopyPix16(AGIDL_BMP* bmp, COLOR16* clrs, u32 count);
void AGIDL_BMPSetClr(AGIDL_BMP *bmp, int x, int y, COLOR clr);
void AGIDL_BMPSetClr16(AGIDL_BMP *bmp, int x, int y, COLOR16 clr);
void AGIDL_BMPSetRGB(AGIDL_BMP *bmp, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_BMPSetRGBA(AGIDL_BMP *bmp, int x, int y, u8 r, u8 g, u8 b, u8 a);
void AGIDL_BMPSetCompression(AGIDL_BMP *bmp, int compress);
void AGIDL_BMPSetICPMode(AGIDL_BMP *bmp, int mode);
void AGIDL_BMPSetICPEncoding(AGIDL_BMP* bmp, AGIDL_ICP_ENCODE encode);
void AGIDL_ClearBMP(AGIDL_BMP *bmp, COLOR clr);
void AGIDL_ClearBMP16(AGIDL_BMP *bmp, COLOR16 clr);
void AGIDL_ClearColorBMP(AGIDL_BMP* bmp, float r, float g, float b);
void AGIDL_FlushBMP(AGIDL_BMP* bmp);
void AGIDL_FreeBMP(AGIDL_BMP *bmp);
void AGIDL_BMPSetWidth(AGIDL_BMP *bmp, int width);
void AGIDL_BMPSetHeight(AGIDL_BMP *bmp, int height);
void AGIDL_BMPSetClrFmt(AGIDL_BMP *bmp, AGIDL_CLR_FMT fmt);
void AGIDL_BMPSetPalette(AGIDL_BMP *bmp, AGIDL_ICP palette);
void AGIDL_BMPSetMaxDiff(AGIDL_BMP *bmp, u16 max_diff);
void AGIDL_BMPRGB2BGR(AGIDL_BMP *bmp);
void AGIDL_BMPBGR2RGB(AGIDL_BMP *bmp);
void AGIDL_BMPConvert16BPPTO24BPP(AGIDL_BMP *bmp);
void AGIDL_BMPConvert24BPPTO16BPP(AGIDL_BMP *bmp);
void AGIDL_BMP555TO565(AGIDL_BMP* bmp);
void AGIDL_BMP565TO555(AGIDL_BMP* bmp);
void AGIDL_ColorConvertBMP(AGIDL_BMP* bmp, AGIDL_CLR_FMT dest);
int AGIDL_BMPGetWidth(AGIDL_BMP *bmp);
int AGIDL_BMPGetHeight(AGIDL_BMP *bmp);
u32 AGIDL_BMPGetSize(AGIDL_BMP* bmp);
AGIDL_CLR_FMT AGIDL_BMPGetClrFmt(AGIDL_BMP* bmp);
BMP_IMG_TYPE AGIDL_BMPGetImgType(int bits);
u16 AGIDL_BMPGetMaxDiff(AGIDL_BMP *bmp);
void AGIDL_BMPEncodeHeader(AGIDL_BMP* bmp, FILE* file);
void AGIDL_BMPEncodeICP(AGIDL_BMP* bmp,FILE* file);
void AGIDL_BMPEncodeNearestICP(AGIDL_BMP* bmp, AGIDL_ICP palette, FILE* file);
void AGIDL_BMPEncodeRLE(AGIDL_BMP* bmp,FILE* file);
void AGIDL_BMPEncodeIMG(AGIDL_BMP* bmp,FILE* file);
void AGIDL_BMPEncodeIMG0(AGIDL_BMP* bmp, FILE* file);
int AGIDL_BMPDecodeHeader(AGIDL_BMP* bmp, FILE* file);
void AGIDL_BMPDecodeIMG(AGIDL_BMP* bmp, FILE* file, BMP_IMG_TYPE img_type);
void AGIDL_BMPDecodeRLE(AGIDL_BMP* bmp, FILE* file, BMP_IMG_TYPE img_type);
AGIDL_BMP* AGIDL_BMPCpyImg(AGIDL_BMP* bmp);
#endif