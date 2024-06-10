/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_wrap.c
*   Date: 12/13/2023
*   Version: 0.4b
*   Updated: 6/9/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <stdlib.h>
#include <agidl_imgp_wrap.h>

void * AGIDL_WrapImgData(void* data, u32* width, u32 height, AGIDL_CLR_FMT fmt, u8 num_of_wraps){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		u32 w = *width;
		
		COLOR* clrs = (COLOR*)data;
		COLOR* wrap = (COLOR*)malloc(sizeof(COLOR)*(w*num_of_wraps)*height);
		
		u32 i;
		for(i = 0; i < num_of_wraps; i++){
			u32 x,y;
			for(y = 0; y < height; y++){
				for(x = w*i; x < (w*i)+w; x++){
					COLOR clr = AGIDL_GetClr(clrs,x-(w*i),y,w,height);
					AGIDL_SetClr(wrap,clr,x,y,w*num_of_wraps,height);
				}
			}
		}
		
		free(clrs);
		
		*width = w*num_of_wraps;
		
		return wrap;
	}
	else{
		u32 w = *width;
		
		COLOR16* clrs = (COLOR16*)data;
		COLOR16* wrap = (COLOR16*)malloc(sizeof(COLOR16)*(w*num_of_wraps)*height);
		
		u32 i;
		for(i = 0; i < num_of_wraps; i++){
			u32 x,y;
			for(y = 0; y < height; y++){
				for(x = w*i; x < (w*i)+w; x++){
					COLOR16 clr = AGIDL_GetClr16(clrs,x-(w*i),y,w,height);
					AGIDL_SetClr16(wrap,clr,x,y,w*num_of_wraps,height);
				}
			}
		}
		
		free(clrs);
		
		*width = w*num_of_wraps;
		
		return wrap;
	}
}

void * AGIDL_WrapAndMirrorImgData(void* data, u32* width, u32* height, AGIDL_CLR_FMT fmt, u8 num_of_wraps, AGIDL_MIRROR mirror){
	if(AGIDL_GetBitCount(fmt) == 24 || AGIDL_GetBitCount(fmt) == 32){
		COLOR* clrs = (COLOR*)data;
		
		u32 glacew = *width;
		u32 glaceh = *height;
		
		COLOR* glace = (COLOR*)AGIDL_MirrorImgData(clrs,&glacew,&glaceh,fmt,mirror);
		
		COLOR* wrap = (COLOR*)malloc(sizeof(COLOR)*(glacew*num_of_wraps)*glaceh);
		
		u32 i;
		for(i = 0; i < num_of_wraps; i++){
			u32 x,y;
			for(y = 0; y < glaceh; y++){
				for(x = glacew*i; x < (glacew*i)+glacew; x++){
					COLOR clr = AGIDL_GetClr(glace,x-(glacew*i),y,glacew,glaceh);
					AGIDL_SetClr(wrap,clr,x,y,glacew*num_of_wraps,glaceh);
				}
			}
		}
		
		*width = glacew*num_of_wraps;
		*height = glaceh;
		
		free(glace);
		
		return wrap;
	}
	else{
		COLOR16* clrs = (COLOR16*)data;
		
		u32 glacew = *width;
		u32 glaceh = *height;
		
		COLOR16* glace = (COLOR16*)AGIDL_MirrorImgData(clrs,&glacew,&glaceh,fmt,mirror);
		
		COLOR16* wrap = (COLOR16*)malloc(sizeof(COLOR16)*(glacew*num_of_wraps)*glaceh);
		
		u32 i;
		for(i = 0; i < num_of_wraps; i++){
			u32 x,y;
			for(y = 0; y < glaceh; y++){
				for(x = glacew*i; x < (glacew*i)+glacew; x++){
					COLOR16 clr = AGIDL_GetClr16(glace,x-(glacew*i),y,glacew,glaceh);
					AGIDL_SetClr16(wrap,clr,x,y,glacew*num_of_wraps,glaceh);
				}
			}
		}
		
		*width = glacew*num_of_wraps;
		*height = glaceh;
		
		free(glace);
		
		return wrap;
	}
}