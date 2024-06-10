#ifndef AGIDL_IMGP_SCALE_H
#define AGIDL_IMGP_SCALE_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_scale.h
*   Date: 12/9/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_types.h>
#include <agidl_cc_types.h>
#include <agidl_img_types.h>

typedef enum AGIDL_SCALE{
	AGIDL_SCALE_NEAREST = 0x1,
	AGIDL_SCALE_BILERP = 0x2,
	AGIDL_SCALE_TRILERP = 0x3,
}AGIDL_SCALE;

void * AGIDL_HalfImgDataNearest(void* data, u32* width, u32* height, AGIDL_CLR_FMT fmt);
void * AGIDL_HalfImgDataBilerp(void* data, u32* width, u32* height, AGIDL_CLR_FMT fmt);
void * AGIDL_DoubleImgDataNearest(void* data, u32* width, u32* height, AGIDL_CLR_FMT fmt);
void * AGIDL_ScaleImgDataNearest(void* data, u32* width, u32* height, float sx, float sy, AGIDL_CLR_FMT fmt);
void * AGIDL_ScaleImgDataBilerp(void* data, u32* width, u32* height, float sx, float sy, AGIDL_CLR_FMT fmt);
void * AGIDL_FastScaleImgDataBilerp(void* data, u32* width, u32* height, float sx, float sy, AGIDL_CLR_FMT fmt);
void * AGIDL_FastScaleImgDataTrilerp(void* data, u32* width, u32* height, float sx, float sy, AGIDL_CLR_FMT fmt);
void * AGIDL_ScaleImgDataTrilerp(void* data, u32* width, u32* height, float sx, float sy, AGIDL_CLR_FMT fmt);
void * AGIDL_ScaleImgData(void* data, u32* width, u32* height, float sx, float sy, AGIDL_SCALE scale, AGIDL_CLR_FMT fmt);
void * AGIDL_FastScaleImgData(void* data, u32* width, u32* height, float sx, float sy, AGIDL_SCALE scale, AGIDL_CLR_FMT fmt);
void * AGIDL_NearestPow2ScaleImgData(void* data, u32* width, u32* height, u32 max_pow, AGIDL_CLR_FMT fmt);

#endif