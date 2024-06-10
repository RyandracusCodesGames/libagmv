#ifndef AGIDL_IMG_PVR_H
#define AGIDL_IMG_PVR_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_pvr.h
*   Date: 10/28/2023
*   Version: 0.1b
*   Updated: 3/14/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_imgp_mipmap.h>
#include <agidl_img_types.h>
#include <agidl_types.h>

#include <stdio.h>

typedef enum PVR_TYPE{
	DREAMCAST_PVR = 0x0,
	MODERN_PVR = 0x1,
}PVR_TYPE;

typedef enum PVRClrFmt{
	PVR_RGB_555 = 0x00,
	PVR_RGB_565 = 0x01,
	PVR_ARGB_4444 = 0x02,
	PVR_YUV_442 = 0x03,
	PVR_ICP_16 = 0x05,
	PVR_ICP_256 = 0x06,
}PVRClrFmt;

typedef enum PVRPxlFmt{
	PVRTC_2BPP_RGB = 0x0,
	PVRTC_2BPP_RGBA = 0x1,
	PVRTC_4BPP_RGB = 0x2,
	PVRTC_4BPP_RGBA = 0x3,
	PVRTC_II_2BPP_RGB = 0x4,
	PVRTC_II_4BPP_RGB = 0x5,
	PVRTC_RGB_555 =  0x6,
	PVRTC_RGB_565 = 0x7,
	PVRTC_RGB_888 = 0x8,
	PVRTC_RGBA_8888 = 0x9,
	PVRTC_BGRA_8888 = 0x10,
}PVRPxlFmt;

typedef enum PVRImgType{
	PVR_IMG_SQUARE_TWIDDLED = 0x01,
	PVR_IMG_SQUARE_TWIDDLED_AND_MIPMAPPED = 0x02,
	PVR_IMG_ICP_256_TWIDDLED = 0x05,
	PVR_IMG_ICP_16_TWIDDLED = 0x06,
	PVR_IMG_ICP_256 = 0x07,
	PVR_IMG_ICP_16 = 0x08,
	PVR_IMG_RECTANGLE = 0x09,
	PVR_IMG_RECTANGLE_TWIDDLED = 0x0D,
}PVRImgType;

typedef struct PVR_DREAMCAST_HEADER{
	u32 id1;
	u32 offset;
	u32 global_index_1;
	u32 global_index_2;
	u32 id2;
	u32 file_size;
	u8 pvr_clr_fmt;
	u8 pvr_img_type;
	u16 width;
	u16 height;
}PVR_DREAMCAST_HEADER;

typedef struct PVR_MODERN_HEADER{
	u32 version;
	u32 flags;
	u8 ulong2[8];
	u32 clr_fmt;
	u32 channel_type;
	u32 height;
	u32 width;
	u32 depth;
	u32 num_of_surfaces;
	u32 num_of_faces;
	u32 num_of_mipmaps;
	u32 meta_data_size;
	u8 fourcc[4];
	u32 key;
	u32 data_size;
}PVR_MODERN_HEADER;

typedef struct AGIDL_PVR{
	PVR_DREAMCAST_HEADER header;
	PVR_MODERN_HEADER mheader;
	int icp;
	int max_diff;
	AGIDL_ICP palette;
	AGIDL_CLR_FMT fmt;
	Pixels pixels;
	PVR_TYPE pvr_type;
	PVRPxlFmt pxl_fmt;
	AGIDL_Bool compression;
	AGIDL_MIPMAP* mipmap;
	AGIDL_Bool mipped;
	u32 mip_lvl;
	char* filename;
}AGIDL_PVR;

void AGIDL_SetPVRFilename(AGIDL_PVR* pvr, const char* filename);
void AGIDL_PVRSetWidth(AGIDL_PVR* pvr, int width);
void AGIDL_PVRSetHeight(AGIDL_PVR* pvr, int height);
void AGIDL_PVRSetClrFmt(AGIDL_PVR* pvr, AGIDL_CLR_FMT fmt);
void AGIDL_PVRSetMaxDiff(AGIDL_PVR* pvr, int max_diff);
void AGIDL_PVRSetICPMode(AGIDL_PVR* pvr, int mode);
void AGIDL_PVRSetCompression(AGIDL_PVR* pvr, AGIDL_Bool compression);
void AGIDL_PVRBuildMipmap(AGIDL_PVR* pvr, AGIDL_Bool mipped);
void AGIDL_PVRSetMipmapLevel(AGIDL_PVR* pvr, u8 mip_lvl);
void AGIDL_PVRSetType(AGIDL_PVR* pvr, PVR_TYPE pvr_type);
void AGIDL_PVRSetClr(const AGIDL_PVR* pvr, int x, int y, COLOR clr);
void AGIDL_PVRSetClr16(const AGIDL_PVR* pvr, int x, int y, COLOR16 clr);
void AGIDL_PVRSetRGB(const AGIDL_PVR* pvr, int x, int y, u8 r, u8 g, u8 b);
void AGIDL_ClearPVR(const AGIDL_PVR* pvr, COLOR clr);
void AGIDL_ClearPVR16(const AGIDL_PVR* pvr, COLOR16 clr);
void AGIDL_ClearColorPVR(const AGIDL_PVR* pvr, float r, float g, float b);
void AGIDL_FlushPVR(const AGIDL_PVR* pvr);
int AGIDL_PVRGetWidth(const AGIDL_PVR* pvr);
int AGIDL_PVRGetHeight(const AGIDL_PVR* pvr);
u32 AGIDL_PVRGetSize(const AGIDL_PVR* pvr);
AGIDL_CLR_FMT AGIDL_PVRGetClrFmt(const AGIDL_PVR* pvr);
PVR_TYPE AGIDL_PVRGetType(const AGIDL_PVR* pvr);
int AGIDL_PVRGetMaxDiff(const AGIDL_PVR* pvr);
COLOR AGIDL_PVRGetClr(const AGIDL_PVR* pvr, int x, int y);
COLOR16 AGIDL_PVRGetClr16(const AGIDL_PVR* pvr, int, int y);
void AGIDL_PVRSyncPix(const AGIDL_PVR *pvr, const COLOR *clrs);
void AGIDL_PVRSyncPix16(const AGIDL_PVR *pvr, const COLOR16 *clrs);
void AGIDL_PVRCopyPix(const AGIDL_PVR* pvr, const COLOR* clrs, u32 count);
void AGIDL_PVRCopyPix16(const AGIDL_PVR* pvr, const COLOR16* clrs, u32 count);
void AGIDL_FreePVR(AGIDL_PVR* pvr);
void AGIDL_PVRRGB2BGR(AGIDL_PVR* pvr);
void AGIDL_PVRBGR2RGB(AGIDL_PVR* pvr);
void AGIDL_PVRConvert24BPPTO16BPP(AGIDL_PVR* pvr);
void AGIDL_PVRConvert16BPPTO24BPP(AGIDL_PVR* pvr);
void AGIDL_PVRConvertRGBA2RGB(AGIDL_PVR* pvr);
void AGIDL_PVR555TO565(AGIDL_PVR* pvr);
void AGIDL_PVR565TO555(AGIDL_PVR* pvr);
void AGIDL_ColorConvertPVR(AGIDL_PVR* pvr, AGIDL_CLR_FMT dest);
AGIDL_PVR * AGIDL_LoadPVR(char *filename);
AGIDL_PVR * AGIDL_CreatePVR(const char *filename, int width, int height, AGIDL_CLR_FMT fmt);
AGIDL_PVR* AGIDL_PVRCpyImg(const AGIDL_PVR* pvr);
void AGIDL_ExportPVR(AGIDL_PVR *pvr);

int isImgPVR(u32 gbix, u32 pvrt);
int AGIDL_IsModernPVR(PVRPxlFmt fmt);
int AGIDL_PVRDecodeHeader(AGIDL_PVR* pvr, FILE* file);
void AGIDL_PVRDecodeImg(AGIDL_PVR* pvr, PVRClrFmt fmt, PVRImgType img, FILE* file);
void AGIDL_PVRDecodeTwiddledImg(AGIDL_PVR* pvr, PVRClrFmt fmt, PVRImgType img, FILE* file);
void AGIDL_PVRDecodeICP(AGIDL_PVR* pvr, FILE* file);
PVRClrFmt AGIDL_GetPVRClrFmt(u8 byte);
PVRImgType AGIDL_PVRGetImgType(u8 byte);
PVRPxlFmt AGIDL_GetPVRPxlFmt(u8 long2[8]);
u32 AGIDL_GetTwiddleValue(u32 i);
u32 AGIDL_GetDetwiddledIndex(u32 x, u32 y);
void AGIDL_PVREncodeHeader(AGIDL_PVR* pvr, FILE* file);
void AGIDL_PVREncodeIMG(AGIDL_PVR* pvr, FILE* file);

#endif
