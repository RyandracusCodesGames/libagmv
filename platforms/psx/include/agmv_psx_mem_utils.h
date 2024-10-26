#ifndef AGMV_PSX_MEM_UTILS_H
#define AGMV_PSX_MEM_UTILS_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_psx_mem_utils.h
*   Date: 10/1/2024
*   Version: 2.0
*   Updated: 10/19/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_psx_defines.h>
#include <malloc.h>

extern void MCmemcpy(void* Dest, void* Src, int Length);

static inline void AGMV_Memcpy8(u8* dest, u8* src, u32 count){
	switch(count){
		case 1:{
			*(dest++) = *(src++);
		}break;
		case 2:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 3:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 4:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 5:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 6:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 7:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 8:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		default:{
			while(count >= 8){
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				count -= 8;
			}
			while(count--){
				*(dest++) = *(src++);
			}
		}break;
	}
}

static inline void AGMV_Memset8(u8* dest, u8 num, u32 count){
	switch(count){
		case 1:{
			*(dest++) = num;
		}break;
		case 2:{
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 3:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 4:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 5:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 6:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 7:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 8:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		default:{
			while(count >= 8){
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				count -= 8;
			}
			while(count--){
				*(dest++) = num;
			}
		}break;
	}
}

static inline void AGMV_Memcpy16(u16* dest, u16* src, u32 count){
	switch(count){
		case 1:{
			*(dest++) = *(src++);
		}break;
		case 2:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 3:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 4:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 5:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 6:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 7:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 8:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		default:{
			while(count >= 8){
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				count -= 8;
			}
			while(count--){
				*(dest++) = *(src++);
			}
		}break;
	}
}

static inline void AGMV_Memset16(u16* dest, u16 num, u32 count){
	switch(count){
		case 1:{
			*(dest++) = num;
		}break;
		case 2:{
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 3:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 4:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 5:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 6:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 7:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 8:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		default:{
			while(count >= 8){
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				count -= 8;
			}
			while(count--){
				*(dest++) = num;
			}
		}break;
	}
}

static inline void AGMV_Memcpy32(u32* dest, u32* src, u32 count){
	switch(count){
		case 1:{
			*(dest++) = *(src++);
		}break;
		case 3:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 4:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 5:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 6:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 7:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		case 8:{
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
			*(dest++) = *(src++);
		}break;
		default:{
			while(count >= 8){
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				*(dest++) = *(src++);
				count -= 8;
			}
			while(count--){
				*(dest++) = *(src++);
			}
		}break;
	}
}

static inline void AGMV_Memset32(u32* dest, u32 num, u32 count){
	switch(count){
		case 1:{
			*(dest++) = num;
		}break;
		case 2:{
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 3:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 4:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 5:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 6:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 7:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		case 8:{
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
			*(dest++) = num;
		}break;
		default:{
			while(count >= 8){
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				*(dest++) = num;
				count -= 8;
			}
			while(count--){
				*(dest++) = num;
			}
		}break;
	}
}


static inline void AGMV_SetRect(RECT* rect, u16 x, u16 y, u16 w, u16 h){
	rect->x = x;
	rect->y = y;
	rect->w = w;
	rect->h = h;
}

void AGMV_ClearImageData(u16* data, u32 size, u8 r, u8 g, u8 b);
void AGMV_InitializeMemory();
void AGMV_FreeMemory(void* p);

#endif