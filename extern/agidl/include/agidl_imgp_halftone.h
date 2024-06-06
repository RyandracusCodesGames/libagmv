#ifndef AGIDL_IMGP_HALFTONE_H
#define AGIDL_IMGP_HALFTONE_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_halftone.h
*   Date: 1/27/2024
*   Version: 0.2b
*   Updated: 1/27/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include "agidl_types.h"
#include "agidl_cc_types.h"
#include "agidl_img_types.h"

void AGIDL_HalftoneImgData(void* data, u16 width, u16 height, AGIDL_CLR_FMT fmt, u8 threshold);

#endif