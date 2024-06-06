/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_imgp_blend.h
*   Date: 12/13/2023
*   Version: 0.2b
*   Updated: 2/2/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include "agidl_imgp_blend.h"
#include "agidl_cc_manager.h"
#include "agidl_cc_mixer.h"

void AGIDL_BlendChromaImg(void* data, void* chromadata, u32 width, u32 height, u32 width2, u32 height2, AGIDL_CLR_FMT fmt1, AGIDL_CLR_FMT fmt2, u16 x_off, u16 y_off, COLOR chromaclr){
	if(AGIDL_GetBitCount(fmt1) == 24 || AGIDL_GetBitCount(fmt1) == 32){
		COLOR* clrs = (COLOR*)data;
		
		if(AGIDL_GetBitCount(fmt2) == 24 || AGIDL_GetBitCount(fmt2) == 32){
			COLOR* chroma = (COLOR*)chromadata;
			u32 x,y;
			for(y = 0; y < height2; y++){
				for(x = 0; x < width2; x++){
					COLOR clr = AGIDL_GetClr(chroma,x,y,width2,height2);
					
					if(AGIDL_IsInThreshold(clr,chromaclr,fmt2,fmt2,4)){
						AGIDL_SetClr(clrs,clr,x+x_off,y+y_off,width,height);
					}
				}
			}
		}
		else{
			COLOR16* chroma = (COLOR16*)chromadata;
			u32 x,y;
			for(y = 0; y < height2; y++){
				for(x = 0; x < width2; x++){
					COLOR16 clr16 = AGIDL_GetClr16(chroma,x,y,width2,height2);
					COLOR clr = AGIDL_CLR16_TO_CLR(clr16,fmt2,fmt1);
					if(AGIDL_IsInThreshold(clr16,chromaclr,fmt2,fmt2,6)){
						AGIDL_SetClr(clrs,clr,x+x_off,y+y_off,width,height);
					}
				}
			}
		}
	}
	else{
		COLOR16* clrs = (COLOR16*)data;
		
		if(AGIDL_GetBitCount(fmt2) == 24 || AGIDL_GetBitCount(fmt2) == 32){
			COLOR* chroma = (COLOR*)chromadata;
			u32 x,y;
			for(y = 0; y < height2; y++){
				for(x = 0; x < width2; x++){
					COLOR clr24 = AGIDL_GetClr(chroma,x,y,width2,height2);
					COLOR16 clr = AGIDL_CLR_TO_CLR16(clr24,fmt2,fmt1);
					if(AGIDL_IsInThreshold(clr,chromaclr,fmt2,fmt2,6)){
						AGIDL_SetClr16(clrs,clr,x+x_off,y+y_off,width,height);
					}
				}
			}
		}
		else{
			COLOR16* chroma = (COLOR16*)chromadata;
			u32 x,y;
			for(y = 0; y < height2; y++){
				for(x = 0; x < width2; x++){
					COLOR16 clr = AGIDL_GetClr16(chroma,x,y,width2,height2);
					if(AGIDL_IsInThreshold(clr,chromaclr,fmt2,fmt2,6)){
						AGIDL_SetClr16(clrs,clr,x+x_off,y+y_off,width,height);
					}
				}
			}
		}
	}
}

void AGIDL_BlendImgAlpha(void* spr1, u32 width, u32 height, int tx, int ty, void* spr2, u32 width2, u32 height2, AGIDL_CLR_FMT fmt){
	if(AGIDL_GetBitCount(fmt) == 32){
		COLOR* sprpix1 = (COLOR*)spr1;
		COLOR* sprpix2 = (COLOR*)spr2;
		
		u32 x,y;
		for(y = 0; y < height2; y++){
			for(x = 0; x < width2; x++){
				COLOR clr1 = AGIDL_GetClr(sprpix1,x,y,width,height);
				COLOR clr2 = AGIDL_GetClr(sprpix2,x,y,width2,height2);
				
				COLOR blend = AGIDL_BlendColor(clr1,clr2,0,CC_BLEND_CLR_DESTINV,fmt);
				
				AGIDL_SetClr(sprpix1,blend,x+tx,y+ty,width,height);
			}
		}
	}
}

