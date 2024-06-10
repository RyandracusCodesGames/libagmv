#ifndef AGIDL_IMG_QUAKE_H
#define AGIDL_IMG_QUAKE_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_quake.h
*   Date: 10/3/2023
*   Version: 0.1b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_img_types.h>
#include <agidl_types.h>

#include <stdio.h>

#define MAX_PICS 300

typedef struct AGIDL_LMP{
	u32 width;
	u32 height;
	AGIDL_ICP palette;
	AGIDL_CLR_FMT fmt;
	int max_diff;
	Pixels pixels;
	char *filename;
}AGIDL_LMP;

typedef struct SPRHEADER{
	char name[4];
	u32 version;
	u32 type;
	f32 radius;
	u32 maxwidth;
	u32 maxheight;
	u32 num_frames;
	f32 beamlength;
	u32 synchtype;
}SPRHEADER;

typedef struct PICTURE{
	u32 x_offset;
	u32 y_offset;
	u32 width;
	u32 height;
	u8* pix_indices;
	Pixels pixels;
}PICTURE;

typedef struct AGIDL_SPR{
	SPRHEADER header;
	AGIDL_ICP palette;
	AGIDL_CLR_FMT fmt;
	PICTURE pics[MAX_PICS];
	char *filename;
}AGIDL_SPR;

void AGIDL_SetLMPFilename(AGIDL_LMP *lmp, const char *filename);
void AGDIL_LMPSetWidth(AGIDL_LMP *lmp, int width);
void AGIDL_LMPSetHeight(AGIDL_LMP *lmp, int height);
void AGIDL_LMPSetClrFmt(AGIDL_LMP *lmp, AGIDL_CLR_FMT fmt);
void AGIDL_LMPSetClr(const AGIDL_LMP *lmp, int x, int y, COLOR clr);
void AGIDL_LMPSetClr16(const AGIDL_LMP *lmp, int x, int y, COLOR16 clr);
int AGIDL_LMPGetMaxDiff(const AGIDL_LMP* lmp);
void AGIDL_LMPSetRGB(const AGIDL_LMP *lmp, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_ClearLMP(const AGIDL_LMP *lmp, COLOR clr);
void AGIDL_ClearLMP16(const AGIDL_LMP *lmp, COLOR16 clr);
void AGIDL_ClearColorLMP(const AGIDL_LMP* lmp, float r, float g, float b);
void AGIDL_FlushLMP(const AGIDL_LMP* lmp);
int AGIDL_LMPGetWidth(const AGIDL_LMP *lmp);
int AGIDL_LMPGetHeight(const AGIDL_LMP *lmp);
u32 AGIDL_LMPGetSize(const AGIDL_LMP* lmp);
AGIDL_CLR_FMT AGIDL_LMPGetClrFmt(const AGIDL_LMP* lmp);
COLOR AGIDL_LMPGetClr(const AGIDL_LMP *lmp, int x, int y);
COLOR16 AGIDL_LMPGetClr16(const AGIDL_LMP *lmp, int x, int y);
void AGIDL_FreeLMP(AGIDL_LMP *lmp);
void AGIDL_LMPRGB2BGR(AGIDL_LMP *lmp);
void AGIDL_LMPBGR2RGB(AGIDL_LMP *lmp);
void AGIDL_LMPConvert16BPPTO24BPP(AGIDL_LMP *lmp);
void AGIDL_LMPConvert24BPPTO16BPP(AGIDL_LMP *lmp);
void AGIDL_LMPConvert555TO565(AGIDL_LMP *lmp);
void AGIDL_LMPConvert565TO555(AGIDL_LMP *lmp);
void AGIDL_ColorConvertLMP(AGIDL_LMP* lmp, AGIDL_CLR_FMT dest);
void AGIDL_LMPSyncPix(const AGIDL_LMP *lmp, const COLOR *clrs);
void AGIDL_LMPSyncPix16(const AGIDL_LMP *lmp, const COLOR16 *clrs);
void AGIDL_LMPCopyPix(const AGIDL_LMP* lmp, const COLOR* clrs, u32 count);
void AGIDL_LMPCopyPix16(const AGIDL_LMP* lmp, const COLOR16* clrs, u32 count);
AGIDL_LMP * AGIDL_LoadLMP(char *filename);
AGIDL_LMP * AGIDL_CreateLMP(const char *filename, int width, int height, AGIDL_CLR_FMT fmt);
void AGIDL_ExportLMP(AGIDL_LMP *lmp);
void AGIDL_LMPEncodeICP(const AGIDL_LMP* lmp, FILE* file);
AGIDL_ICP AGIDL_GenerateQuakeICP();
AGIDL_ICP AGIDL_LoadQuakeICP(const char *filename);
void AGIDL_LMPSetMaxDiff(AGIDL_LMP *lmp, int max_diff);
AGIDL_LMP* AGIDL_LMPCpyImg(const AGIDL_LMP* lmp);
void AGIDL_LMPEncodeHeader(const AGIDL_LMP* lmp, FILE* file);
void AGIDL_LMPDecodeHeader(AGIDL_LMP* lmp, FILE* file);
void AGIDL_LMPDecodeIMG(const AGIDL_LMP* lmp, FILE* file);
#endif
