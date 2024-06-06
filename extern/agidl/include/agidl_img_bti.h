#ifndef AGIDL_IMG_BTI_H
#define AGIDL_IMG_BIT_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_bti.h
*   Date: 11/22/2023
*   Version: 0.1b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <stdio.h>
#include "agidl_types.h"
#include "agidl_cc_types.h"
#include "agidl_img_types.h"
#include "agidl_cc_manager.h"

typedef enum BTI_CLR_FMT{
	BTI_I4 = 0x00,
	BTI_I8 = 0x01,
	BTI_IA4 = 0x02,
	BTI_IA8 = 0x03,
	BTI_RGB_565 = 0x04,
	BTI_RGB5A3 = 0x05,
	BTI_RGBA32 = 0x06,
	BTI_CI4 = 0x08,
	BTI_CI8 = 0x09,
	BTI_C14X2 = 0x0A,
	BTI_CMPR = 0x0E,
}BTI_CLR_FMT;

typedef enum BTI_ICP_FMT{
	BTI_ICP_IA8 = 0x00,
	BTI_ICP_565 = 0x01,
	BTI_ICP_5A3 = 0x02,
}BTI_ICP_FMT;

typedef struct BTI_HEADER{
	BTI_CLR_FMT fmt;
	u16 alpha;
	u16 width;
	u16 height;
	u8 wraps;
	u8 wrapt;
	BTI_ICP_FMT icp_fmt;
	u16 num_of_icps;
	u32 offset_icp;
	u32 blank;
	u8 filter_type;
	u8 filter_type2;
	u16 blank2;
	u8 mipmapcount;
	u32 blank3;
	u16 blank4;
	u32 offset;
}BTI_HEADER;

typedef struct AGIDL_BTI{
	BTI_HEADER header;
	int icp;
	int compression;
	int max_diff;
	AGIDL_ICP palette;
	AGIDL_CLR_FMT fmt;
	AGIDL_ICP_ENCODE encode;
	Pixels pixels;
	char* filename;
}AGIDL_BTI;

void AGIDL_SetBTIFilename(AGIDL_BTI* bti, const char* filename);
void AGIDL_BTISetWidth(AGIDL_BTI* bti, u16 width);
void AGIDL_BTISetHeight(AGIDL_BTI* bti, u16 height);
void AGIDL_BTISetClrFmt(AGIDL_BTI* bti, AGIDL_CLR_FMT fmt);
void AGIDL_BTISetMaxDiff(AGIDL_BTI* bti, int max_diff);
void AGIDL_BTISetICPMode(AGIDL_BTI* bti, int mode);
void AGIDL_BTISetICPEncoding(AGIDL_BTI* bti, AGIDL_ICP_ENCODE encode);
void AGIDL_BTISetCompression(AGIDL_BTI* bti, int compress);
void AGIDL_BTISetClr(AGIDL_BTI* bti, int x, int y, COLOR clr);
void AGIDL_BTISetClr16(AGIDL_BTI* bti, int x, int y, COLOR16 clr);
void AGIDL_BTISetRGB(AGIDL_BTI* bti, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_BTISetRGBA(AGIDL_BTI* bti, int x, int y, u8 r, u8 g, u8 b, u8 a);
void AGIDL_ClearBTI(AGIDL_BTI* bti, COLOR clr);
void AGIDL_ClearBTI16(AGIDL_BTI* bti, COLOR16 clr);
void AGIDL_ClearColorBTI(AGIDL_BTI* bti, float r, float g, float b);
void AGIDL_FlushBTI(AGIDL_BTI* bti);
int AGIDL_BTIGetWidth(AGIDL_BTI* bti);
int AGIDL_BTIGetHeight(AGIDL_BTI* bti);
u32 AGIDL_BTIGetSize(AGIDL_BTI* bti);
AGIDL_CLR_FMT AGIDL_BTIGetClrFmt(AGIDL_BTI* bti);
int AGIDL_BTIGetMaxDiff(AGIDL_BTI* bti);
COLOR AGIDL_BTIGetClr(AGIDL_BTI* bti, int x, int y);
COLOR16 AGIDL_BTIGetClr16(AGIDL_BTI* bti, int x, int y);
void AGIDL_BTIRGB2BGR(AGIDL_BTI* bti);
void AGIDL_BTIBGR2RGB(AGIDL_BTI* bti);
void AGIDL_BTIConvert24BPPTO16BPP(AGIDL_BTI* bti);
void AGIDL_BTIConvert16BPPTO24BPP(AGIDL_BTI* bti);
void AGIDL_BTIRGBATORGB(AGIDL_BTI* bti);
void AGIDL_BTI555TO565(AGIDL_BTI* bti);
void AGIDL_BTI565TO555(AGIDL_BTI* bti);
void AGIDL_ColorConvertBTI(AGIDL_BTI* bti, AGIDL_CLR_FMT dest);
void AGIDL_BTISyncPix(AGIDL_BTI *bti, COLOR *clrs);
void AGIDL_BTISyncPix16(AGIDL_BTI *bti, COLOR16 *clrs);
void AGIDL_BTICopyPix(AGIDL_BTI* bti, COLOR* clrs, u32 count);
void AGIDL_BTICopyPix16(AGIDL_BTI* bti, COLOR16* clrs, u32 count);
AGIDL_BTI* AGIDL_LoadBTI(char* filename);
AGIDL_BTI* AGIDL_CreateBTI(const char* filename, int width, int height, AGIDL_CLR_FMT fmt);
AGIDL_BTI* AGIDL_BTICpyImg(AGIDL_BTI* bti);
BTI_CLR_FMT AGIDL_GetBTIClrFmt(u8 type);
BTI_ICP_FMT AGIDL_BTIGetICPFmt(u8 type);
void AGIDL_ExportBTI(AGIDL_BTI* bti);
void AGIDL_FreeBTI(AGIDL_BTI* bti);
int AGIDL_BTIDecodeHeader(AGIDL_BTI* bti, FILE* file);
void AGIDL_BTIDecodeIMG(AGIDL_BTI* bti, FILE* file);
void AGIDL_BTIEncodeHeader(AGIDL_BTI* bti, FILE* file);
void AGIDL_BTIEncodeICP(AGIDL_BTI* bti);
void AGIDL_BTIEncodeIMG(AGIDL_BTI* bti, FILE* file);

#endif