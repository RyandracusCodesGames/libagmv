/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_chromascan.c
*   Date: 2/4/2024
*   Version: 0.3b
*   Updated: 2/4/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_cc_chromascan.h>
#include <agidl_cc_manager.h>

void AGIDL_ChromaScan1555(u16* clr_data, const u32 width, const u32 height, const COLOR16 chromakey, const AGIDL_Bool on){
	for(int i = 0; i < width*height; i++){
		u16 clr = clr_data[i];
		
		if(clr == chromakey){
			if(on == TRUE){
				clr = 1 << 15 | clr;
			}
			else{
				clr = 0 << 15 | clr;
			}
		}
		else{
			if(on == TRUE){
				clr = 0 << 15 | clr;
			}
			else{
				clr = 1 << 15 | clr;
			}
		}
		
		clr_data[i] = clr;
	}
}

void AGIDL_SetAlpha1555(u16* clr_data, const u32 width, const u32 height, const AGIDL_Bool on){
	for(int i = 0; i < width*height; i++){
		u16 clr = clr_data[i];
		
		if(on == TRUE){
			clr = 1 << 15 | clr;
		}
		else{
			clr = 0 << 15 | clr;
		}
		
		clr_data[i] = clr;
	}
}

void AGIDL_SetCoordAlpha1555(u16* clr_data, const u32 x, const u32 y, const u32 width, const u32 height, const AGIDL_Bool on){
	if(x < width && y < height){
		const u32 index = x + y * width;
		
		u16 clr = clr_data[index];
		
		if(on == TRUE){
			clr = 1 << 15 | clr;
		}
		else{
			clr = 0 << 15 | clr;
		}
		
		clr_data[index] = clr;
	}
}

void AGIDL_SetClrAlpha1555(u16* clr_data, const u32 width, const u32 height, const COLOR16 clr, const AGIDL_Bool on){
	for(int i = 0; i < width*height; i++){
		u16 clri = clr_data[i];
		
		if(clri == clr){
			if(on == TRUE){
				clri = 1 << 15 | clr;
			}
			else{
				clri = 0 << 15 | clr;
			}
		}
		
		clr_data[i] = clri;
	}
}

void AGIDL_ChromaScan8888(u32* clr_data, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt, const COLOR chromakey, const AGIDL_Bool on){
	for(int i = 0; i < width*height; i++){
		u32 color = clr_data[i];
		
		if(color == chromakey){
			if(on == TRUE){
				color = AGIDL_SetA(color,0xff,fmt);
			}
			else{
				color = AGIDL_SetA(color,0,fmt);
			}
		}
		else{
			if(on == TRUE){
				color = AGIDL_SetA(color,0,fmt);
			}
			else{
				color = AGIDL_SetA(color,0xff,fmt);
			}
		}
		
		clr_data[i] = color;
	}
}

void AGIDL_SetAlpha8888(u32* clr_data, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt, const AGIDL_Bool on){
	for(int i = 0; i < width*height; i++){
		u32 color = clr_data[i];
		
		if(on == TRUE){
			color = AGIDL_SetA(color,0xff,fmt);
		}
		else{
			color = AGIDL_SetA(color,0,fmt);
		}
		
		clr_data[i] = color;
	}
}

void AGIDL_SetCoordAlpha8888(u32* clr_data, const u32 x, const u32 y, const u32 width, const u32 height, const AGIDL_CLR_FMT fmt, const AGIDL_Bool on){
	if(x < width && y < height){
		const u32 index = x + y * width;
		
		u32 color = clr_data[index];
		
		if(on == TRUE){
			color = AGIDL_SetA(color,0xff,fmt);
		}
		else{
			color = AGIDL_SetA(color,0,fmt);
		}
		
		clr_data[index] = color;
	}
}

void AGIDL_SetClrAlpha8888(u32* clr_data, const u32 width, const u32 height, const COLOR clr, const AGIDL_CLR_FMT fmt, const AGIDL_Bool on){
	for(int i = 0; i < width*height; i++){
		u32 color = clr_data[i];
		
		if(color == clr){
			if(on == TRUE){
				color = AGIDL_SetA(color,0xff,fmt);
			}
			else{
				color = AGIDL_SetA(color,0,fmt);
			}
		}
		
		clr_data[i] = color;
	}
}
