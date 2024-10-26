#ifndef AGMV_CONVERT_H
#define AGMV_CONVERT_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_convert.h
*   Date: 8/11/2024
*   Version: 2.0
*   Updated: 8/21/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_defines.h>

int AGMV_ConvertAGMVtoAVI(const char* filename);
int AGMV_ConvertAVItoAGMV(const char* filename, AGMV_OPT opt, AGMV_QUALITY quality, AGMV_COMPRESSION compression);
u16 AGMV_ToRGB16(u32 color);

#endif