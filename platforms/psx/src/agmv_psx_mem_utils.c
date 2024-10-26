/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_psx_mem_utils.c
*   Date: 10/1/2024
*   Version: 2.0
*   Updated: 10/15/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv_psx_mem_utils.h>

u_long _ramsize   = 0x00200000; // force 2 megabytes of RAM
u_long _stacksize = 0x00004000; // force 16 kilobytes of stack

#define STACK (0x80000000 + _ramsize - _stacksize)

u_long heap_base[4];

void AGMV_ClearImageData(u16* data, u32 size, u8 r, u8 g, u8 b){
	AGMV_Memset16(data,(b&0x1f) << 10 | (g&0x1f) << 5 | (r&0x1f),size);
}

void AGMV_InitializeMemory(){
	InitHeap3((u_long*)heap_base,STACK-(u_long)heap_base);
}

void AGMV_FreeMemory(void* p){
	free3(p);
}