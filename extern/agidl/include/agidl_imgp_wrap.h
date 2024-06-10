#ifndef AGIDL_IMGP_WRAP_H
#define AGIDL_IMGP_WRAP_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_wrap.h
*   Date: 12/13/2023
*   Version: 0.2b
*   Updated: 1/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_imgp_mirror.h>
#include <agidl_types.h>

void * AGIDL_WrapImgData(void* data, u16* width, u16 height, AGIDL_CLR_FMT fmt, u8 num_of_wraps);
void * AGIDL_WrapAndMirrorImgData(void* data, u16* width, u16* height, AGIDL_CLR_FMT fmt, u8 num_of_wraps, AGIDL_MIRROR mirror);

#endif
