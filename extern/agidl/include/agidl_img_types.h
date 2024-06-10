#ifndef AGIDL_IMG_TYPES_H
#define AGIDL_IMG_TYPES_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_types.h
*   Date: 9/15/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agidl_cc_types.h>

#define NO_ICP 0
#define YES_ICP 1

typedef struct Pixels{
	COLOR *pix32;
	COLOR16 *pix16;
}Pixels;

typedef enum AGIDL_IMG_TYPE{
	AGIDL_IMG_BMP = 0x1,
	AGIDL_IMG_TGA = 0x2,
	AGIDL_IMG_TIM = 0x3,
	AGIDL_IMG_PCX = 0x4,
	AGIDL_IMG_LMP = 0x5,
	AGIDL_IMG_PVR = 0x6,
	AGIDL_IMG_GXT = 0x7,
	AGIDL_IMG_BTI = 0x8,
	AGIDL_IMG_3DF = 0x9,
	AGIDL_IMG_PPM = 0x0A,
	AGIDL_IMG_LBM = 0x0B,
}AGIDL_IMG_TYPE;

typedef enum AGIDL_FILE_TYPE{
	F_SOURCE = 0x1,
	F_HEADER = 0x2,
	F_BIN = 0x4,
}AGIDL_FILE_TYPE;

typedef enum AGIDL_ARR_TYPE{
	ARR = 0x1,
	ARR2D = 0x2,
}AGIDL_ARR_TYPE;

void AGIDL_FilenameCpy(char *filedest, const char *filesrc);
char* AGIDL_StrCpy(char* a, char*b);
void AGIDL_StrCpy2(char* dest, char *a, char *b);
char* AGIDL_GetImgExtension(AGIDL_IMG_TYPE img);
char* AGIDL_GetImgName(char* filename);
void AGIDL_ClrMemcpy(COLOR *dest, COLOR *src, u32 count);
void AGIDL_ClrMemcpy16(COLOR16 *dest, COLOR16 *src, u32 count);
int AGIDL_CheckCoordBounds(u32 x, u32 y, u32 width, u32 height);
void AGIDL_CopyTile(void* dest, void* src, u32 destw, u32 desth, u32 srcw, u32 srch, AGIDL_CLR_FMT destfmt, AGIDL_CLR_FMT srcfmt, u16 xstart, u16 xend, u16 ystart, u16 yend, u16 dx, u16 dy);
void AGIDL_CopyScanline(void* dest, void* src, u32 destw, u32 desth, u32 srcw, u32 srch, AGIDL_CLR_FMT destfmt, AGIDL_CLR_FMT srcfmt, u16 destscanline, u16 srcscanline);
void AGIDL_ClrMemset(COLOR *dest, COLOR clr, u32 count);
void AGIDL_ClrMemset16(COLOR16 *dest, COLOR16 clr, u32 count);
void AGIDL_ClearColorBuffer(void* data, float r, float g, float b, AGIDL_CLR_FMT fmt, u32 count);
u8 AGIDL_GetBitCount(AGIDL_CLR_FMT fmt);
void AGIDL_ExtractAndPrintBGR(FILE* file, COLOR clr, AGIDL_CLR_FMT fmt);
void AGIDL_ExtractAndPrintRGB(FILE* file, COLOR clr, AGIDL_CLR_FMT fmt);
void AGIDL_ExtractAndPrintRGBA(FILE* file, COLOR clr, AGIDL_CLR_FMT fmt);
void AGIDL_ExportRawColors(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt, AGIDL_FILE_TYPE ftype, AGIDL_ARR_TYPE arrtype, u8 rgb);
u8 * AGIDL_GenerateImgDataFromICP(void* data, u32 width, u32 height, AGIDL_ICP icp, int max_diff);
COLOR AGIDL_GetClr(COLOR* clrs, int x, int y, int width, int height);
COLOR16 AGIDL_GetClr16(COLOR16* clrs, int x, int y, int width, int height);
void AGIDL_SetClr(COLOR* clrs, COLOR clr, int x, int y, int width, int height);
void AGIDL_SetClr16(COLOR16* clrs, COLOR16 clr, int x, int y, int width, int height);
void AGIDL_RGB2BGR(COLOR* clrs, int width, int height, AGIDL_CLR_FMT *fmt);
void AGIDL_BGR2RGB(COLOR* clrs, int width, int height, AGIDL_CLR_FMT *fmt);
void AGIDL_RGB2BGR16(COLOR16* clrs, int width, int height, AGIDL_CLR_FMT *fmt);
void AGIDL_BGR2RGB16(COLOR16* clrs, int width, int height, AGIDL_CLR_FMT *fmt);
void AGIDL_24BPPTO16BPP(COLOR* src, COLOR16* dest, int width, int height, AGIDL_CLR_FMT *fmt);
void AGIDL_16BPPTO24BPP(COLOR16* src, COLOR* dest, int width, int height, AGIDL_CLR_FMT *fmt);
void AGIDL_RGBA2RGB(COLOR* rgba, int width, int height, AGIDL_CLR_FMT* fmt);
void AGIDL_555TO565(COLOR16* src, int width, int height, AGIDL_CLR_FMT *fmt);
void AGIDL_565TO555(COLOR16* src, int width, int height, AGIDL_CLR_FMT *fmt);
u8* AGIDL_GenerateRGBBuffer(void* data, int width, int height, AGIDL_CLR_FMT fmt);
u8* AGIDL_GenerateRGBABuffer(COLOR* clrs, int width, int height, AGIDL_CLR_FMT fmt);
u8* AGIDL_GenerateBGRBuffer(COLOR* clrs, int width, int height, AGIDL_CLR_FMT fmt);
COLOR* AGIDL_RGBSyncClrs(u8* rgbbuff, int width, int height, AGIDL_CLR_FMT fmt);
COLOR* AGIDL_RGBASyncClrs(u8* rgbbuf, int width, int height, AGIDL_CLR_FMT fmt);
void AGIDL_FreeClrs(COLOR* clrs);
void AGIDL_FreeClrs16(COLOR16* clrs);
void AGIDL_FreeRGBBuffer(u8* rgbbuf);
void AGIDL_PrintRGBBuffer(u8* rgbbuf, int width, int height, AGIDL_CLR_FMT fmt);
void AGIDL_PrintRGBABuffer(u8* rgbbuf, int width, int height, AGIDL_CLR_FMT fmt);
#endif