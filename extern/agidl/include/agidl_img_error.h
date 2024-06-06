#ifndef AGIDL_IMG_ERROR_H
#define AGIDL_IMG_ERROR_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_error.h
*   Date: 1/19/2023
*   Version: 0.2b
*   Updated: 1/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include "agidl_types.h"

typedef enum AGIDL_IMG_ERROR{
	NO_IMG_ERROR = 0x0,
	FILE_NOT_LOCATED_IMG_ERROR = 0x1,
	MEMORY_IMG_ERROR = 0x2,
	INVALID_IMG_FORMATTING_ERROR = 0x3,
	INVALID_HEADER_FORMATTING_ERROR = 0x4,
	CORRUPED_IMG_ERROR = 0x5,
}AGIDL_IMG_ERROR;

const char* AGIDL_Error2Str(AGIDL_IMG_ERROR error);

#endif