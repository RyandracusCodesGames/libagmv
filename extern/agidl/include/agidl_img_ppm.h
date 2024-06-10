#ifndef AGIDL_IMG_PPM_H
#define AGIDL_IMG_PPM_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_ppm.h
*   Date: 2/19/2024
*   Version: 0.3b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_img_types.h>
#include <agidl_types.h>

#include <stdio.h>

typedef enum AGIDL_PPM_TYPE{
	PPM_BIN = 0x1,
	PPM_ASCII = 0x2,
	PPM_UNKNOWN = 0x3,
}AGIDL_PPM_TYPE;

typedef struct AGIDL_PPM{
	AGIDL_PPM_TYPE type;
	u32 width;
	u32 height;
	u8 bpp;
	u16 max_diff;
	AGIDL_CLR_FMT fmt;
	Pixels pixels;
	char* filename;
}AGIDL_PPM;

void AGIDL_SetPPMFilename(AGIDL_PPM* ppm, const char* filename);
void AGIDL_PPMSetWidth(AGIDL_PPM* ppm, int width);
void AGIDL_PPMSetHeight(AGIDL_PPM* ppm, int height);
void AGILD_PPMSetClrFmt(AGIDL_PPM* ppm, AGIDL_CLR_FMT fmt);
void AGIDL_PPMSetType(AGIDL_PPM* ppm, AGIDL_PPM_TYPE type);
void AGIDL_PPMSetMaxDiff(AGIDL_PPM* ppm, int max_diff);
void AGIDL_PPMSetClr(const AGIDL_PPM* ppm, int x, int y, COLOR clr);
void AGIDL_PPMSetClr16(const AGIDL_PPM* ppm, int x, int y, COLOR16 clr);
void AGIDL_PPMSetRGB(const AGIDL_PPM* ppm, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_PPMSetColor3f(const AGIDL_PPM* ppm, int x, int y, float r, float g, float b);
void AGIDL_PPMSetColor4f(const AGIDL_PPM* ppm, int x, int y, float r, float g, float b, float a);
void AGIDL_ClearPPM(const AGIDL_PPM* ppm, COLOR clr);
void AGIDL_ClearPPM16(const AGIDL_PPM* ppm, COLOR16 clr);
void AGIDL_ClearColorPPM(const AGIDL_PPM* ppm, float r, float g, float b);
void AGIDL_FlushPPM(const AGIDL_PPM* ppm);
u32 AGIDL_PPMGetWidth(const AGIDL_PPM* ppm);
u32 AGIDL_PPMGetHeight(const AGIDL_PPM* ppm);
u32 AGIDL_PPMGetSize(const AGIDL_PPM* ppm);
AGIDL_CLR_FMT AGIDL_PPMGetClrFmt(const AGIDL_PPM* ppm);
int AGIDL_PPMGetMaxDiff(const AGIDL_PPM* ppm);
COLOR AGIDL_PPMGetClr(const AGIDL_PPM* ppm, int x, int y);
COLOR16 AGIDL_PPMGetClr16(const AGIDL_PPM* ppm, int, int y);
void AGIDL_PPMSyncPix(const AGIDL_PPM *ppm, const COLOR *clrs);
void AGIDL_PPMSyncPix16(const AGIDL_PPM *ppm, const COLOR16 *clrs);
void AGIDL_PPMCopyPix(const AGIDL_PPM* ppm, const COLOR* clrs, u32 count);
void AGIDL_PPMCopyPix16(const AGIDL_PPM* ppm, const COLOR16* clrs, u32 count);
void AGIDL_FreePPM(AGIDL_PPM* ppm);
void AGIDL_PPMRGB2BGR(AGIDL_PPM* ppm);
void AGIDL_PPMBGR2RGB(AGIDL_PPM* ppm);
void AGIDL_PPMConvert24BPPTO16BPP(AGIDL_PPM* ppm);
void AGIDL_PPMConvert16BPPTO24BPP(AGIDL_PPM* ppm);
void AGIDL_PPMConvertRGBA2RGB(AGIDL_PPM* ppm);
void AGIDL_PPM555TO565(AGIDL_PPM* ppm);
void AGIDL_PPM565TO555(AGIDL_PPM* ppm);
void AGIDL_PPMConvertRGBA2RGB(AGIDL_PPM* ppm);
void AGIDL_PPMConvertRGB2RGBA(AGIDL_PPM* ppm);
void AGIDL_ColorConvertPPM(AGIDL_PPM* ppm, AGIDL_CLR_FMT dest);

int AGIDL_PPMDecodeHeader(AGIDL_PPM* ppm, FILE* file);
void AGIDL_PPMDecodeIMG(AGIDL_PPM* ppm, FILE* file);
AGIDL_PPM * AGIDL_LoadPPM(const char* filename);
AGIDL_PPM * AGIDL_CreatePPM(const char* filename, int width, int height, AGIDL_CLR_FMT fmt);
AGIDL_PPM * AGIDL_PPMCpyImg(const AGIDL_PPM* ppm);
void AGIDL_PPMEncodeHeader(const AGIDL_PPM* ppm, FILE* file);
void AGIDL_PPMEncodeIMG(const AGIDL_PPM* ppm, FILE* file);
void AGIDL_ExportPPM(AGIDL_PPM* ppm);

AGIDL_PPM_TYPE AGIDL_PPMGetType(u8 byte1, u8 byte2);
u8 AGIDL_PPMGetNum(char c);

#endif
