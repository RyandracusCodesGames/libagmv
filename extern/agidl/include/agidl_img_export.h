#ifndef AGIDL_IMG_EXPORT_H
#define AGIDL_IMG_EXPORT_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_export.h
*   Date: 3/28/2024
*   Version: 0.4b
*   Updated: 3/28/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_img_types.h>
#include <agidl_types.h>

void AGIDL_QuickExport(const void* data, u32 width, u32 height, AGIDL_CLR_FMT fmt, AGIDL_IMG_TYPE img_type);

#endif
