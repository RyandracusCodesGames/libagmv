#include "agidl_img_compression.h"
#include "agidl_img_types.h"

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_img_compression.c
*   Date: 11/25/2023
*   Version: 0.1b
*   Updated: 3/7/2024
*   Author: Ryandracus Chapman
*
********************************************/

u32 AGIDL_EncodeRLE(void* data, u32 bit_count, u32 x, u32 y, u32 width, u32 height, u16 max_rle){

	u32 count = 1;

	if(bit_count == 24 || bit_count == 32){
		COLOR* clrdata = (COLOR*)data;
		
		u32 x_count = x + 1;
		while(AGIDL_GetClr(clrdata,x,y,width,height) == AGIDL_GetClr(clrdata,x_count,y,width,height) && count < max_rle){
			count++;
			x_count++;
		}
	}
	else{
		COLOR16* clrdata = (COLOR16*)data;
		u32 x_count = x + 1;
		while((AGIDL_GetClr16(clrdata,x,y,width,height) == AGIDL_GetClr16(clrdata,x_count,y,width,height)) && count < max_rle){
			count++;
			x_count++;
		}
	}

	return count;
}

u32 AGIDL_EncodeByteRLE(u8* data, u32 i, u32 max_rle){
	u32 count = 1, inext = i + 1;
	
	while(data[i] == data[inext] && count < max_rle){
		inext++;
		count++;
	}
	
	return count;
}

void AGIDL_PackBits(u8* src, u8 *dest, u32 max_rle){
	u32 count = 0;
	
	do{
		u8 byte = *src++;
		
		if(byte > 128){
			u8 nextbyte = *src++;
			
			int i;
			for(i = 0; i < 257 - byte; i++, count++){
				*dest++ = nextbyte;
			}
		}
		else if(byte < 128){
			int i;
			for(i = 0; i < byte + 1; i++, count++){
				*dest++ = *src++;
			}
		}
		else{
			count = max_rle;
		}
		
	}while(count < max_rle);
}