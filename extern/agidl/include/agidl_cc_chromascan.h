#ifndef AGIDL_CC_CHROMASCAN_H
#define AGIDL_CC_CHROMASCAN_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_chromascan.h
*   Date: 2/4/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_types.h>
#include <agidl_cc_types.h>

void AGIDL_ChromaScan1555(u16* clr_data, u32 width, u32 height, COLOR16 chromakey, AGIDL_Bool on);
void AGIDL_SetAlpha1555(u16* clr_data, u32 width, u32 height, AGIDL_Bool on);
void AGIDL_SetCoordAlpha1555(u16* clr_data, u32 x, u32 y, u32 width, u32 height, AGIDL_Bool on);
void AGIDL_SetClrAlpha1555(u16* clr_data, u32 width, u32 height, COLOR16 clr, AGIDL_Bool on);

void AGIDL_ChromaScan8888(u32* clr_data, u32 width, u32 height, AGIDL_CLR_FMT fmt, COLOR chromakey, AGIDL_Bool on);
void AGIDL_SetAlpha8888(u32* clr_data, u32 width, u32 height, AGIDL_CLR_FMT fmt, AGIDL_Bool on);
void AGIDL_SetCoordAlpha8888(u32* clr_data, u32 x, u32 y, u32 width, u32 height, AGIDL_CLR_FMT fmt, AGIDL_Bool on);
void AGIDL_SetClrAlpha8888(u32* clr_data, u32 width, u32 height, COLOR clr, AGIDL_CLR_FMT fmt, AGIDL_Bool on);

#endif