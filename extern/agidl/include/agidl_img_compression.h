#ifndef AGIDL_IMG_COMPRESSION_H
#define AGIDL_IMG_COMPRESSION_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_compression.h
*   Date: 11/25/2023
*   Version: 0.1b
*   Updated: 3/7/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include "agidl_types.h"

u32 AGIDL_EncodeRLE(void* data, u32 bit_count, u32 x, u32 y, u32 width, u32 height, u16 max_rle);
u32 AGIDL_EncodeByteRLE(u8* data, u32 i, u32 max_rle);
void AGIDL_PackBits(u8* src, u8 *dest, u32 max_rle);
void AGIDL_EncodePackBits(u8* src, u8* dest, u32 max_rle);

#endif