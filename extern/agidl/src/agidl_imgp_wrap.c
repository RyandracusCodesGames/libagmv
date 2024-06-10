/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_wrap.c
*   Date: 12/13/2023
*   Version: 0.2b
*   Updated: 1/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_imgp_wrap.h>

#include <stdlib.h>

#include <agidl_img_types.h>

void * AGIDL_WrapImgData(void* data, u16* width, const u16 height, const AGIDL_CLR_FMT fmt, const u8 num_of_wraps){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		const u16 w = *width;
		
		COLOR* clrs = data;
		COLOR* wrap = malloc(sizeof(COLOR)*(w*num_of_wraps)*height);

		for(u32 i = 0; i < num_of_wraps; i++){
			for(u32 y = 0; y < height; y++){
				for(u32 x = w * i; x < (w*i)+w; x++){
					const COLOR clr = AGIDL_GetClr(clrs,x-(w*i),y,w,height);
					AGIDL_SetClr(wrap,clr,x,y,w*num_of_wraps,height);
				}
			}
		}
		
		free(clrs);
		
		*width = w*num_of_wraps;
		
		return wrap;
	}
	const u16 w = *width;
		
	COLOR16* clrs = data;
	COLOR16* wrap = malloc(sizeof(COLOR16)*(w*num_of_wraps)*height);

	for(u32 i = 0; i < num_of_wraps; i++){
		for(u32 y = 0; y < height; y++){
			for(u32 x = w * i; x < (w*i)+w; x++){
				const COLOR16 clr = AGIDL_GetClr16(clrs,x-(w*i),y,w,height);
				AGIDL_SetClr16(wrap,clr,x,y,w*num_of_wraps,height);
			}
		}
	}
		
	free(clrs);
		
	*width = w*num_of_wraps;
		
	return wrap;
}

void * AGIDL_WrapAndMirrorImgData(void* data, u16* width, u16* height, const AGIDL_CLR_FMT fmt, const u8 num_of_wraps, const AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* clrs = data;
		
		u16 glacew = *width;
		u16 glaceh = *height;
		
		COLOR* glace = AGIDL_MirrorImgData(clrs,&glacew,&glaceh,fmt,mirror);
		
		COLOR* wrap = malloc(sizeof(COLOR)*(glacew*num_of_wraps)*glaceh);

		for(u32 i = 0; i < num_of_wraps; i++){
			for(u32 y = 0; y < glaceh; y++){
				for(u32 x = glacew * i; x < (glacew*i)+glacew; x++){
					const COLOR clr = AGIDL_GetClr(glace,x-(glacew*i),y,glacew,glaceh);
					AGIDL_SetClr(wrap,clr,x,y,glacew*num_of_wraps,glaceh);
				}
			}
		}
		
		*width = glacew*num_of_wraps;
		*height = glaceh;
		
		free(glace);
		
		return wrap;
	}
	COLOR16* clrs = data;
		
	u16 glacew = *width;
	u16 glaceh = *height;
		
	COLOR16* glace = AGIDL_MirrorImgData(clrs,&glacew,&glaceh,fmt,mirror);
		
	COLOR16* wrap = malloc(sizeof(COLOR16)*(glacew*num_of_wraps)*glaceh);

	for(u32 i = 0; i < num_of_wraps; i++){
		for(u32 y = 0; y < glaceh; y++){
			for(u32 x = glacew * i; x < (glacew*i)+glacew; x++){
				const COLOR16 clr = AGIDL_GetClr16(glace,x-(glacew*i),y,glacew,glaceh);
				AGIDL_SetClr16(wrap,clr,x,y,glacew*num_of_wraps,glaceh);
			}
		}
	}
		
	*width = glacew*num_of_wraps;
	*height = glaceh;
		
	free(glace);
		
	return wrap;
}
