#ifndef AGIDL_CC_CONVERTER_H
#define AGIDL_CC_CONVERTER_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_converter.h
*   Date: 9/9/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_types.h>
#include <agidl_types.h>

COLOR AGIDL_CLR16_TO_CLR(COLOR16 clr, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt);
COLOR16 AGIDL_CLR_TO_CLR16(COLOR clr, AGIDL_CLR_FMT srcfmt, AGIDL_CLR_FMT destfmt);

COLOR AGIDL_RGB_TO_BGR(COLOR rgb, AGIDL_CLR_FMT fmt);
COLOR AGIDL_BGR_TO_RGB(COLOR bgr, AGIDL_CLR_FMT fmt);

COLOR16 AGIDL_555_TO_565(COLOR16 clr, AGIDL_CLR_FMT fmt);
COLOR16 AGIDL_565_TO_555(COLOR16 clr, AGIDL_CLR_FMT fmt);

COLOR AGIDL_RGBA_TO_RGB(COLOR rgba, AGIDL_CLR_FMT src, AGIDL_CLR_FMT dest);
COLOR AGIDL_RGB_TO_RGBA(COLOR rgb, AGIDL_CLR_FMT src, AGIDL_CLR_FMT dest);

COLOR AGIDL_ColorConvert(COLOR src, AGIDL_CLR_FMT sfmt, AGIDL_CLR_FMT dfmt);

#endif