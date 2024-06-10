#ifndef AGIDL_IMG_PCX_H
#define AGIDL_IMG_PCX_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_pcx.h
*   Date: 9/25/2023
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

typedef struct PCXHEADER{
	u8 id;
	u8 version;
	u8 encoding;
	u8 bits;
	u16 x_start;
	u16 y_start;
	u16 x_end;
	u16 y_end;
	u16 width;
	u16 height;
	u8 ega[48];
	u8 reserved;
	u8 numbitplanes;
	u16 bytesperline;
	u16 pal_type;
	u16 screen_horz;
	u16 screen_vert;
	u8 blanks[54];
}PCXHEADER;

typedef struct AGIDL_PCX{
	PCXHEADER header;
	int icp;
	int max_diff;
	AGIDL_ICP palette;
	AGIDL_CLR_FMT fmt;
	Pixels pixels;
	AGIDL_ICP_ENCODE encode;
	char *filename;
}AGIDL_PCX;

void AGIDL_SetPCXFilename(AGIDL_PCX *pcx, const char* filename);
void AGIDL_PCXSetWidth(AGIDL_PCX *pcx, int width);
void AGIDL_PCXSetHeight(AGIDL_PCX *pcx, int height);
void AGIDL_PCXSetClrFmt(AGIDL_PCX *pcx, AGIDL_CLR_FMT fmt);
void AGIDL_PCXSetICPMode(AGIDL_PCX *pcx, int mode);
void AGIDL_PCXSetICPEncoding(AGIDL_PCX* pcx, AGIDL_ICP_ENCODE encode);
void AGIDL_PCXSetMaxDiff(AGIDL_PCX* pcx, int max_diff);
void AGIDL_PCXSetClr(const AGIDL_PCX *pcx, int x, int y, COLOR clr);
void AGIDL_PCXSetClr16(const AGIDL_PCX *pcx, int x, int y, COLOR16 clr);
void AGIDL_PCXSetRGB(const AGIDL_PCX *pcx, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_ClearPCX(const AGIDL_PCX *pcx, COLOR clr);
void AGIDL_ClearPCX16(const AGIDL_PCX *pcx, COLOR16 clr);
void AGIDL_ClearColorPCX(const AGIDL_PCX* pcx, float r, float g, float b);
void AGIDL_FlushPCX(const AGIDL_PCX* pcx);
void AGIDL_PCXSyncPix(const AGIDL_PCX *pcx, const COLOR *clrs);
void AGIDL_PCXSyncPix16(const AGIDL_PCX *pcx, const COLOR16 *clrs);
void AGIDL_PCXCopyPix(const AGIDL_PCX* pcx, const COLOR* clrs, u32 count);
void AGIDL_PCXCopyPix16(const AGIDL_PCX* pcx, const COLOR16* clrs, u32 count);
int AGIDL_PCXGetWidth(const AGIDL_PCX *pcx);
int AGIDL_PCXGetHeight(const AGIDL_PCX *pcx);
u32 AGIDL_PCXGetSize(const AGIDL_PCX* pcx);
int AGIDL_PCXGetMaxDiff(const AGIDL_PCX* pcx);
AGIDL_CLR_FMT AGIDL_PCXGetClrFmt(const AGIDL_PCX* pcx);
COLOR AGIDL_PCXGetClr(const AGIDL_PCX *pcx, int x, int y);
COLOR16 AGIDL_PCXGetClr16(const AGIDL_PCX *pcx, int x, int y);
void AGIDL_FreePCX(AGIDL_PCX *pcx);
void AGIDL_PCXRGB2BGR(AGIDL_PCX *pcx);
void AGIDL_PCXBGR2RGB(AGIDL_PCX *pcx);
void AGIDL_PCXConvert16BPPTO24BPP(AGIDL_PCX *pcx);
void AGIDL_PCXConvert24BPPTO16BPP(AGIDL_PCX *pcx);
void AGIDL_PCXConvert555TO565(AGIDL_PCX *pcx);
void AGIDL_PCXConvert565TO555(AGIDL_PCX *pcx);
void AGIDL_ColorConvertPCX(AGIDL_PCX* pcx, AGIDL_CLR_FMT dest);
AGIDL_PCX * AGIDL_LoadPCX(char *filename);
AGIDL_PCX * AGIDL_CreatePCX(const char *filename, int width, int height, AGIDL_CLR_FMT fmt);
void AGIDL_ExportPCX(AGIDL_PCX *pcx);
AGIDL_PCX* AGIDL_PCXCpyImg(const AGIDL_PCX* pcx);
void AGIDL_PCXEncodeHeader(AGIDL_PCX* pcx, FILE* file);
void AGIDL_PCXEncodeICP(AGIDL_PCX* pcx);
void AGIDL_PCXEncodeImg(AGIDL_PCX* pcx, FILE* file);
int AGIDL_PCXDecodeHeader(AGIDL_PCX* pcx, FILE* file);
void AGIDL_PCXDecodeIMG(AGIDL_PCX* pcx, FILE* file);
/*HELPER FUNCTIONS THAT ARE FINE-TUNED EXPLICITLY FOR USAGE WITH PCX IMAGES*/
u16 bin2dec(const char *binary);
char* dec2bin(u16 number);
u16 little_endianify(u16 number);
u16 big_endianify(u16 number);
char* pcxrlebits(const char *binary);

#endif
