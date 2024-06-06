#ifndef AGIDL_IMG_SEARCH_H
#define AGIDL_IMG_SEARCH_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_search.h
*   Date: 11/11/2023
*   Version: 0.1b
*   Updated: 2/15/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include "agidl_img_types.h"

void AGIDL_TIMSearchFileOnDisk(const char* filename, AGIDL_IMG_TYPE img_type, int flip);
void AGIDL_TGASearchFileOnDisk(const char* filename, AGIDL_IMG_TYPE img_type);
int AGIDL_3DFSearchFileOnDisk(const char* filename, AGIDL_IMG_TYPE img_type, int flip, u32 jump);

#endif