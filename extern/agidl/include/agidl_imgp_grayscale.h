#ifndef AGIDL_IMGP_GRAYSCALE_H
#define AGIDL_IMGP_GRAYSCALE_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_grayscale.h
*   Date: 11/27/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_types.h>
#include <agidl_cc_types.h>

void AGIDL_GrayscaleImgData(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt);

#endif