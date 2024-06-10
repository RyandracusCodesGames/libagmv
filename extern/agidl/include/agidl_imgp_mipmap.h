#ifndef AGIDL_IMGP_MIPMAP_H
#define AGIDL_IMGP_MIPMAP_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_mipmap.h
*   Date: 1/23/2024
*   Version: 0.3b
*   Updated: 2/7/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_img_types.h>
#include <agidl_types.h>

#include <stdio.h>

#define MAX_MIP_LVL 16

typedef struct AGIDL_MIPMAP_LEVEL{
	AGIDL_CLR_FMT fmt;
	u16 width;
	u16 height;
	u8 mip_lvl;
	void* img_data;
}AGIDL_MIPMAP_LEVEL;

typedef struct AGIDL_MIPMAPP{
	AGIDL_CLR_FMT fmt;
	u16 width;
	u16 height;
	AGIDL_Bool IsLinear;
	u8 mipcount;
	AGIDL_MIPMAP_LEVEL mipmap[MAX_MIP_LVL];
}AGIDL_MIPMAP;

AGIDL_MIPMAP* AGIDL_CreateMipmapMMU(u16 width, u16 height, AGIDL_CLR_FMT fmt, AGIDL_Bool IsLinear);
AGIDL_MIPMAP* AGIDL_GenerateMipmapFromImgData(const void* data, u16 width, u16 height, AGIDL_CLR_FMT fmt);
AGIDL_MIPMAP* AGIDL_LoadMipmapImgData(FILE* file, u16 width, u16 height, AGIDL_CLR_FMT fmt, u8 count, AGIDL_Bool IsLinear);
void AGIDL_DestroyMipmapMMU(AGIDL_MIPMAP* mipmap);
void AGIDL_ExportMipmap(const AGIDL_MIPMAP* mipmap, AGIDL_IMG_TYPE img_type, AGIDL_Bool flip);
void AGIDL_FilterBilerpMipmapLevel(const AGIDL_MIPMAP* mipmap, u8 mip_lvl);
void AGIDL_FilterBilerpMipmap(const AGIDL_MIPMAP* mipmap);
void AGIDL_ClearMipmapLevel(const AGIDL_MIPMAP* mipmap, u8 mip_lvl);
void AGIDL_RebuildMipmapLevel(const AGIDL_MIPMAP* mipmap, u8 mip_lvl);
void AGIDL_BlendMipmapLevel(AGIDL_MIPMAP* mipmap, u8 mip_lvl1, u8 mip_lvl2);

#endif
