/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_cc_chromascan.c
*   Date: 2/4/2024
*   Version: 0.4b
*   Updated: 6/10/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agidl_cc_chromascan.h>
#include <agidl_cc_manager.h>

void AGIDL_ChromaScan1555(u16* clr_data, u32 width, u32 height, COLOR16 chromakey, AGIDL_Bool on){
	u16 clr;
	int i;
	for(i = 0; i < width*height; i++){
		clr = clr_data[i];
		
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

void AGIDL_SetAlpha1555(u16* clr_data, u32 width, u32 height, AGIDL_Bool on){
	u16 clr;
	int i;
	for(i = 0; i < width*height; i++){
		clr = clr_data[i];
		
		if(on == TRUE){
			clr = 1 << 15 | clr;
		}
		else{
			clr = 0 << 15 | clr;
		}
		
		clr_data[i] = clr;
	}
}

void AGIDL_SetCoordAlpha1555(u16* clr_data, u32 x, u32 y, u32 width, u32 height, AGIDL_Bool on){
	u32 index;
	u16 clr;
	
	if(x < width && y < height){
		index = x + y * width;
		clr = clr_data[index];
		
		if(on == TRUE){
			clr = 1 << 15 | clr;
		}
		else{
			clr = 0 << 15 | clr;
		}
		
		clr_data[index] = clr;
	}
}

void AGIDL_SetClrAlpha1555(u16* clr_data, u32 width, u32 height, COLOR16 clr, AGIDL_Bool on){
	u16 clri;
	int i;
	for(i = 0; i < width*height; i++){
		clri = clr_data[i];
		
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

void AGIDL_ChromaScan8888(u32* clr_data, u32 width, u32 height, AGIDL_CLR_FMT fmt, COLOR chromakey, AGIDL_Bool on){
	u32 color, i;
	for(i = 0; i < width*height; i++){
		color = clr_data[i];
		
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

void AGIDL_SetAlpha8888(u32* clr_data, u32 width, u32 height, AGIDL_CLR_FMT fmt, AGIDL_Bool on){
	u32 color, i;
	for(i = 0; i < width*height; i++){
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

void AGIDL_SetCoordAlpha8888(u32* clr_data, u32 x, u32 y, u32 width, u32 height, AGIDL_CLR_FMT fmt, AGIDL_Bool on){
	u32 index, color;
	
	if(x < width && y < height){
		index = x + y * width;
		color = clr_data[index];
		
		if(on == TRUE){
			color = AGIDL_SetA(color,0xff,fmt);
		}
		else{
			color = AGIDL_SetA(color,0,fmt);
		}
		
		clr_data[index] = color;
	}
}

void AGIDL_SetClrAlpha8888(u32* clr_data, u32 width, u32 height, COLOR clr, AGIDL_CLR_FMT fmt, AGIDL_Bool on){
	u32 color, i;
	for(i = 0; i < width*height; i++){
		color = clr_data[i];
		
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