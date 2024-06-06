#ifndef AGIDL_IMGP_BLEND_H
#define AGIDL_IMGP_BLEND_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_blend.h
*   Date: 12/13/2023
*   Version: 0.2b
*   Updated: 1/19/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include "agidl_types.h"
#include "agidl_cc_types.h"
#include "agidl_img_types.h"

void AGIDL_BlendChromaImg(void* data, void* chromadata, u32 width, u32 height, u32 width2, u32 height2, AGIDL_CLR_FMT fmt1, AGIDL_CLR_FMT fmt2, u16 x_off, u16 y_off, COLOR chromaclr);
void AGIDL_BlendImgAlpha(void* spr1, u32 width, u32 height, int tx, int ty, void* spr2, u32 width2, u32 height2, AGIDL_CLR_FMT fmt);

#endif