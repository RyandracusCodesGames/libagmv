#ifndef AGIDL_IMGP_FILTER_H
#define AGIDL_IMGP_FILTER_H
/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_filter.h
*   Date: 12/13/2023
*   Version: 0.2b
*   Updated: 1/29/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_types.h>

COLOR AGIDL_SamplePointNearest(const void* data, float u, float v, u32 width, u32 height, AGIDL_CLR_FMT fmt);
COLOR AGIDL_SamplePointBilerp(void* data, float u, float v, u32 width, u32 height, AGIDL_CLR_FMT fmt);
COLOR AGIDL_SamplePointTrilerp(void* data, float u, float v, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_FilterImgDataBilerp(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_FastFilterImgDataBilerp(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_FilterImgDataTrilerp(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_FastFilterImgDataTrilerp(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt);

#endif
