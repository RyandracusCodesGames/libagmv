#ifndef AGIDL_MMU_UTILS_H
#define AGIDL_MMU_UTILS_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_mmu_utils.h
*   Date: 2/6/2024
*   Version: 0.3b
*   Updated: 2/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include "agidl_types.h"
#include "agidl_cc_types.h"

void* AGIDL_AllocImgDataMMU(u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_ConvertRGB2RGBA(void* src, void* dest, u32 width, u32 height, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt);
void AGIDL_ConvertRGBA2RGB(void* src, void* dest, u32 width, u32 height, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt);
void AGIDL_ColorConvertImgData(void* src, void* dest, u32 width, u32 height, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt);

#endif