#ifndef AGIDL_IMGP_ROTATE_H
#define AGIDL_IMGP_ROTATE_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_rotate.h
*   Date: 12/9/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_types.h>
#include <agidl_cc_types.h>
#include <agidl_img_types.h>

typedef enum AGIDL_ANGLE{
	ANGLE_90_DEG = 90,
	ANGLE_180_DEG = 180,
	ANGLE_270_DEG = 270,
}AGIDL_ANGLE;

void AGIDL_FlipImgDataHorz(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_FlipImgDataVert(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_FlipImgDataHorzAndVert(void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt);
void AGIDL_RotateImgData(void* data, u32* width, u32* height, AGIDL_CLR_FMT fmt, AGIDL_ANGLE angle);
#endif