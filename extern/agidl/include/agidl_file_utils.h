#ifndef AGIDL_FILE_UTILS_H
#define AGIDL_FILE_UTILS_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_file_utils.h
*   Date: 1/25/2024
*   Version: 0.2b
*   Updated: 3/15/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_types.h>

#include <stdio.h>

void AGIDL_InitBigEndArch();
void AGIDL_DisableBigEndArch();

u8 AGIDL_ReadByte(FILE* file);
u16 AGIDL_ReadShort(FILE* file);
u32 AGIDL_ReadLong(FILE* file);
u32 AGIDL_ReadRGB(FILE* file, AGIDL_CLR_FMT fmt);

void AGIDL_WriteByte(FILE* file, u8 byte);
void AGIDL_WriteShort(FILE* file, u16 word);
void AGIDL_WriteLong(FILE* file, u32 dword);
void AGIDL_WriteRGB(FILE* file, COLOR clr, AGIDL_CLR_FMT fmt, AGIDL_CLR_FMT write);

void AGIDL_ReadBufClr16(FILE* file, COLOR16* buf, u32 width, u32 height);
void AGIDL_ReadBufRGB16(FILE* file, COLOR16* buf, u32 width, u32 height);
void AGIDL_ReadBufRGB(FILE* file, COLOR* clr, u32 width, u32 height);
void AGIDL_ReadBufBGR(FILE* file, COLOR* clr, u32 width, u32 height);
void AGIDL_ReadBufRGBA(FILE* file, COLOR* clr, u32 width, u32 height);
void AGIDL_ReadBufBGRA(FILE* file, COLOR* clr, u32 width, u32 height);

void AGIDL_WriteBufRGB16(FILE* file, const COLOR16* buf, u32 width, u32 height);
void AGIDL_WriteBufClr16(FILE* file, const COLOR16* buf, u32 width, u32 height);
void AGIDL_WriteBufRGB(FILE* file, const COLOR* clr, u32 width, u32 height);
void AGIDL_WriteBufBGR(FILE* file, const COLOR* clr, u32 width, u32 height);
void AGIDL_WriteBufRGBA(FILE* file, const COLOR* clr, u32 width, u32 height);
void AGIDL_WriteBufBGRA(FILE* file, const COLOR* clr, u32 width, u32 height, AGIDL_CLR_FMT fmt);

void AGIDL_PrintFourCC(FILE* file, char f, char o, char u, char r);
void AGIDL_ReadNBytes(FILE* file, u8* buf, int n);

#endif
