#ifndef AGIDL_IMGP_MIRROR_H
#define AGIDL_IMGP_MIRROR_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_mirror.h
*   Date: 12/13/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agidl_types.h>
#include <agidl_cc_types.h>
#include <agidl_img_types.h>

typedef enum AGIDL_MIRROR{
	MIRROR_LEFT_TO_RIGHT = 0x1,
	MIRROR_RIGHT_TO_LEFT = 0x2,
	MIRROR_TOP_TO_BOTTOM = 0x3,
	MIRROR_BOTTOM_TO_TOP = 0x4,
}AGIDL_MIRROR;

void * AGIDL_MirrorImgData(void* data, u32* width, u32* height, AGIDL_CLR_FMT fmt, AGIDL_MIRROR mirror);

#endif