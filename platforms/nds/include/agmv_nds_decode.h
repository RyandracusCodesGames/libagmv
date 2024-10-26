#ifndef AGMV_NDS_DECODE_H
#define AGMV_NDS_DECODE_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_nds_decode.h
*   Date: 8/14/2024
*   Version: 2.0
*   Updated: 10/24/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv_nds_defines.h>
#include <agmv_nds_file_utils.h>

static inline void ITCM_CODE AGMV_Memcpy16(u16* src, u16* dest, u32 n){
	while(n >= 4){
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		n -= 4;
	}
	while(n--){
		*(dest++) = *(src++);
	}
}

static inline void ITCM_CODE AGMV_Memcpy8(u8* dest, u8* src, u32 n){
	while(n >= 4){
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		*(dest++) = *(src++);
		n -= 4;
	}
	while(n--){
		*(dest++) = *(src++);
	}
}


static inline void ITCM_CODE AGMV_Memset16(u16* dest, u16 num, u32 n){
	while(n--){
		*(dest++) = num;
	}
}

static inline void ITCM_CODE AGMV_BlockCopy(u16* dest, u16 *src, u32 doffset, u32 soffset, u8 block_w, u8 block_h, u16 width){
	u16 *dest_ptr, *src_ptr, i, block_w2;
	
	dest_ptr = dest + doffset;
	src_ptr  = src  + soffset;
	
	block_w2 = block_w << 1;
	
	for(i = 0; i < block_h; i += 4){
		dmaCopy(src_ptr,dest_ptr,block_w2);
		dest_ptr += width;   src_ptr += width;
		dmaCopy(src_ptr,dest_ptr,block_w2);
		dest_ptr += width;   src_ptr += width;
		dmaCopy(src_ptr,dest_ptr,block_w2);
		dest_ptr += width;   src_ptr += width;
		dmaCopy(src_ptr,dest_ptr,block_w2);
		dest_ptr += width;   src_ptr += width;
	}
}

static inline void ITCM_CODE AGMV_SBlockCopy(u16* dest, u16 *src, u32 doffset, u32 soffset, u8 block_w, u8 block_h, u16 width){
	u16 *dest_ptr, *src_ptr, i, block_w2;
	
	dest_ptr = dest + doffset;
	src_ptr  = src  + soffset;
	
	block_w2 = block_w << 1;
	
	for(i = 0; i < block_h; i++){
		dmaCopy(src_ptr,dest_ptr,block_w2);
		dest_ptr += width;   src_ptr += width;
	}
}

static inline void ITCM_CODE AGMV_BlockFill(u16 *dest, u16 color, u8 block_w, u8 block_h, u16 width){
	u16 i, block_w2;
	
	block_w2 = block_w << 1;
	
	for(i = 0; i < block_h; i += 4){
		dmaFillHalfWords(color,dest,block_w2);
		dest += width;
		dmaFillHalfWords(color,dest,block_w2);
		dest += width;
		dmaFillHalfWords(color,dest,block_w2);
		dest += width;
		dmaFillHalfWords(color,dest,block_w2);
		dest += width;
	}
}


static inline u16 ITCM_CODE AGMV_InterpColor(u32 color1, u32 color2){
	u8 r1, g1, b1, r2, g2, b2, r, g, b;
	
	b1 = (color1 >> 10) & 0x1f;
	g1 = (color1 >> 5) & 0x1f;
	r1 = (color1 & 0x1f);
	
	b2 = (color2 >> 10) & 0x1f;
	g2 = (color2 >> 5) & 0x1f;
	r2 = (color2 & 0x1f);
	
	r = (r1 + r2) >> 1;
	g = (g1 + g2) >> 1;
	b = (b1 + b2) >> 1;
	
	return 1 << 15 | b << 10 | g << 5 | r;
}

static inline u8 AGMV_GetBlockWidth(AGMV_BLOCK_SIZE block){
	if(block == AGMV_BLOCK_4x4){
		return 4;
	}
	else if(block == AGMV_BLOCK_2x4){
		return 2;
	}
	else return 8;
}

static inline u8 AGMV_GetBlockHeight(AGMV_BLOCK_SIZE block){
	if(block == AGMV_BLOCK_8x8){
		return 8;
	}
	else return 4;
}

void ITCM_CODE AGMV_DecompressLZSS(FILE* file, u8* bitstream_data, u32 usize, u32 csize);
int AGMV_DecodeHeader(FILE* file, AGMV* agmv);
int ITCM_CODE AGMV_DecodeFrameChunk(FILE* file, AGMV* agmv);
int ITCM_CODE AGMV_DecodeAudioChunk(FILE* file, AGMV* agmv);

#endif