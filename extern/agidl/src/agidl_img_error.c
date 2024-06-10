/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_error.c
*   Date: 1/19/2023
*   Version: 0.2b
*   Updated: 1/19/2023
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_img_error.h>

const char* AGIDL_Error2Str(const AGIDL_IMG_ERROR error){
	switch(error){
		case NO_IMG_ERROR:{
			return "NO IMG ERROR";
		}break;
		case FILE_NOT_LOCATED_IMG_ERROR:{
			return "FILE NOT LOCATED IMAGE ERROR";
		}break;
		case MEMORY_IMG_ERROR:{
			return "MEMORY IMG ERROR";
		}break;
		case INVALID_IMG_FORMATTING_ERROR:{
			return "INVALID IMG FORMATTING ERROR";
		}break;
		case INVALID_HEADER_FORMATTING_ERROR :{
			return "INVALID HEADER FORMATTING ERROR";
		}break;
		case CORRUPED_IMG_ERROR :{
			return "CORRUPED IMG ERROR";
		}break;
		default: return "UNKNOWN IMAGE ERROR"; break;
	}
}