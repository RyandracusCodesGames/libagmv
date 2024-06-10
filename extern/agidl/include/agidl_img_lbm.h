#ifndef AGIDL_IMG_LBM_H
#define AGIDL_IMG_LBM_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_lbm.h
*   Date: 2/24/2024
*   Version: 0.3b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include <agidl_types.h>
#include <agidl_cc_types.h>
#include <agidl_img_types.h>
#include <agidl_cc_manager.h>

typedef enum AGIDL_ILBM_TYPE{
	TYPE_ILBM = 0x1,
	TYPE_PBM = 0x2,
}AGIDL_ILBM_TYPE;

typedef struct AGIDL_FORM{
	char formid[4];
	u32 size;
	char ilbmid[4];
}AGIDL_FORM;

typedef struct AGIDL_BMHD{
	char bmhdid[4];
	u32 size;
	u32 width;
	u32 height;
	u16 x_off;
	u16 y_off;
	u8 bpp;
	u8 mask;
	u8 compress;
	u8 pad;
	u16 chromakey;
	u8 x_aspect;
	u8 y_aspect;
	u16 horz_res;
	u16 vert_res;
}AGIDL_BMHD;

typedef struct AGIDL_CMAP{
	char cmapid[4];
	u32 size;
	AGIDL_ICP palette;
}AGIDL_CMAP;

typedef struct AGIDL_CLUT{
	char clutid[4];
	u32 size;
	u8 rlut[256];
	u8 glut[256];
	u8 blut[256];
	u8 ilut[256];
}AGIDL_CLUT;

typedef struct AGIDL_CRNG{
	char crngid[4];
	u32 size;
	u16 pad;
	u16 rate;
	u16 flags;
	u8 low;
	u8 high;
}AGIDL_CRNG;

typedef struct AGIDL_CMAG{
	char cmagid[4];
	u32 size;
	u32 viewport_mode;
}AGIDL_CMAG;

typedef struct AGIDL_ANNO{
	char annoid[4];
	u32 size;
}AGIDL_ANNO;

typedef struct AGIDL_BODY{
	char bodyid[4];
	u32 size;
	u8* src;
	u8* interleave;
	u8* dest;
}AGIDL_BODY;

typedef struct LBM_HEADER{
	AGIDL_FORM form;
	AGIDL_BMHD bmhd;
	AGIDL_CMAP cmap;
	AGIDL_CLUT clut;
	AGIDL_CRNG crng;
	AGIDL_CMAG cmag;
	AGIDL_BODY body;
	AGIDL_ANNO anno;
}LBM_HEADER;

typedef struct AGIDL_LBM{
	LBM_HEADER header;
	AGIDL_CLR_FMT fmt;
	AGIDL_Bool icp;
	AGIDL_Bool compress;
	AGIDL_ICP_ENCODE encode;
	AGIDL_ILBM_TYPE type;
	AGIDL_Bool clut;
	u16 max_diff;
	Pixels pixels;
	char* filename;
}AGIDL_LBM;

void AGIDL_SetLBMFilename(AGIDL_LBM* lbm, const char* filename);
void AGIDL_LBMSetWidth(AGIDL_LBM* lbm, u32 width);
void AGIDL_LBMSetHeight(AGIDL_LBM* lbm, u32 height);
void AGIDL_LBMSetClrFmt(AGIDL_LBM* lbm, AGIDL_CLR_FMT fmt);
void AGIDL_LBMSetICPMode(AGIDL_LBM* lbm, AGIDL_Bool icp);
void AGIDL_LBMSetICPEncoding(AGIDL_LBM* lbm, AGIDL_ICP_ENCODE encode);
void AGIDL_LBMSetMaxDiff(AGIDL_LBM* lbm, int max_diff);
void AGIDL_LBMSetCompression(AGIDL_LBM* lbm, AGIDL_Bool compress);
void AGIDL_LBMSetType(AGIDL_LBM* lbm, AGIDL_ILBM_TYPE type);
void AGIDL_LBMSetClr(AGIDL_LBM* lbm, int x, int y, COLOR clr);
void AGIDL_LBMSetClr16(AGIDL_LBM* lbm, int x, int y, COLOR16 clr);
void AGIDL_LBMSetRGB(AGIDL_LBM* lbm, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_LBMSetRGBA(AGIDL_LBM* lbm, int x, int y, u8 r, u8 g, u8 b, u8 a);
void AGIDL_LBMSetColor3f(AGIDL_LBM* lbm, int x, int y, f32 r, f32 g, f32 b);
void AGIDL_LBMSetColor4f(AGIDL_LBM* lbm, int x, int y, f32 r, f32 g, f32 b, f32 a);
void AGIDL_ClearLBM(AGIDL_LBM* lbm, COLOR clr);
void AGIDL_ClearLBM16(AGIDL_LBM* lbm, COLOR16 clr);
void AGIDL_ClearColorLBM(AGIDL_LBM* lbm, float r, float g, float b);
void AGIDL_FlushLBM(AGIDL_LBM* lbm);
u32 AGIDL_LBMGetWidth(AGIDL_LBM* lbm);
u32 AGIDL_LBMGetHeight(AGIDL_LBM* lbm);
u32 AGIDL_LBMGetSize(AGIDL_LBM* lbm);
AGIDL_CLR_FMT AGIDL_LBMGetClrFmt(AGIDL_LBM* lbm);
int AGIDL_LBMGetMaxDiff(AGIDL_LBM* lbm);
AGIDL_ILBM_TYPE AGIDL_LBMGetType(AGIDL_LBM* lbm);
COLOR AGIDL_LBMGetClr(AGIDL_LBM* lbm, int x, int y);
COLOR16 AGIDL_LBMGetClr16(AGIDL_LBM* lbm, int, int y);
void AGIDL_LBMSyncPix(AGIDL_LBM *lbm, COLOR *clrs);
void AGIDL_LBMSyncPix16(AGIDL_LBM *lbm, COLOR16 *clrs);
void AGIDL_LBMCopyPix(AGIDL_LBM* lbm, COLOR* clrs, u32 count);
void AGIDL_LBMCopyPix16(AGIDL_LBM* lbm, COLOR16* clrs, u32 count);
void AGIDL_FreeLBM(AGIDL_LBM* lbm);
void AGIDL_LBMRGB2BGR(AGIDL_LBM* lbm);
void AGIDL_LBMBGR2RGB(AGIDL_LBM* lbm);
void AGIDL_LBMConvert24BPPTO16BPP(AGIDL_LBM* lbm);
void AGIDL_LBMConvert16BPPTO24BPP(AGIDL_LBM* lbm);
void AGIDL_LBMConvertRGBA2RGB(AGIDL_LBM* lbm);
void AGIDL_LBM555TO565(AGIDL_LBM* lbm);
void AGIDL_LBM565TO555(AGIDL_LBM* lbm);
void AGIDL_LBMConvertRGBA2RGB(AGIDL_LBM* lbm);
void AGIDL_LBMConvertRGB2RGBA(AGIDL_LBM* lbm);
void AGIDL_ColorConvertLBM(AGIDL_LBM* lbm, AGIDL_CLR_FMT dest);

int AGIDL_LBMDecodeHeader(AGIDL_LBM* lbm, FILE* file);
void AGIDL_LBMDecompressData(AGIDL_LBM* lbm, u8* src, u8* dest);
void AGIDL_LBMDeinterleaveData(AGIDL_LBM* lbm, u8* src, u8* dest);
void AGIDL_LBMDecodeIMG(AGIDL_LBM* lbm);
void AGIDL_LBMEncodeHeader(AGIDL_LBM* lbm, FILE* file);
void AGIDL_LBMEncodeICP(AGIDL_LBM* lbm);
void AGIDL_LMPEncodeNearestICP(AGIDL_LBM* lbm, AGIDL_ICP palette);
void AGIDL_LBMEncodeIMG(AGIDL_LBM* lbm, FILE* file);

AGIDL_LBM * AGIDL_CreateLBM(const char* filename, int width, int height, AGIDL_CLR_FMT fmt);
AGIDL_LBM * AGIDL_LBMCopyImg(AGIDL_LBM* lbm);
AGIDL_LBM * AGIDL_LoadLBM(char* filename);
void AGIDL_ExportLBM(AGIDL_LBM* lbm);
void AGIDL_FreeLBM(AGIDL_LBM* lbm);

#endif