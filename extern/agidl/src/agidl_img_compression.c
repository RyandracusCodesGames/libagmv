#include <agidl_img_compression.h>
#include <agidl_img_types.h>

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

u32 AGIDL_EncodeRLE(const void* data, const u32 bit_count, const u32 x, const u32 y, const u32 width, const u32 height, const u16 max_rle){

	u32 count = 1;

	if(bit_count == 24 || bit_count == 32){
		const COLOR* clrdata = data;
		
		u32 x_count = x + 1;
		while(AGIDL_GetClr(clrdata,x,y,width,height) == AGIDL_GetClr(clrdata,x_count,y,width,height) && count < max_rle){
			count++;
			x_count++;
		}
	}
	else{
		const COLOR16* clrdata = data;
		u32 x_count = x + 1;
		while((AGIDL_GetClr16(clrdata,x,y,width,height) == AGIDL_GetClr16(clrdata,x_count,y,width,height)) && count < max_rle){
			count++;
			x_count++;
		}
	}

	return count;
}

u32 AGIDL_EncodeByteRLE(const u8* data, const u32 i, const u32 max_rle){
	u32 count = 1, inext = i + 1;
	
	while(data[i] == data[inext] && count < max_rle){
		inext++;
		count++;
	}
	
	return count;
}

void AGIDL_PackBits(const u8* src, u8 *dest, const u32 max_rle){
	u32 count = 0;
	
	do{
		const u8 byte = *src++;
		
		if(byte > 128){
			const u8 nextbyte = *src++;

			for(int i = 0; i < 257 - byte; i++, count++){
				*dest++ = nextbyte;
			}
		}
		else if(byte < 128){
			for(int i = 0; i < byte + 1; i++, count++){
				*dest++ = *src++;
			}
		}
		else{
			count = max_rle;
		}
		
	}while(count < max_rle);
}
