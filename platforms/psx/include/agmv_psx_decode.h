#ifndef AGMV_PSX_DECODE_H
#define AGMV_PSX_DECODE_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_psx_decode.h
*   Date: 10/4/2024
*   Version: 2.0
*   Updated: 10/26/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_psx_defines.h>
#include <agmv_psx_mem_utils.h>
#include <agmv_psx_file_utils.h>

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

static inline void AGMV_BlockCopy(u16* dest, u16 *src, u32 doffset, u32 soffset, u8 block_w, u8 block_h, u32 width){
	u16* dest_ptr, *src_ptr;
	
	dest_ptr = dest + doffset;
	src_ptr  = src  + soffset;
	
	while(block_h--){
		AGMV_Memcpy16(dest_ptr,src_ptr,block_w);
		dest_ptr += width;   src_ptr += width;
	}
}

static inline void AGMV_BlockFill(u16* dest, u16 color, u8 block_w, u8 block_h, u32 width){
	while(block_h--){
		AGMV_Memset16(dest,color,block_w);
		dest += width;
	}
}

static inline u16 AGMV_InterpColor(u16 color1, u16 color2){
	u8 r1, g1, b1, r2, g2, b2, r, g, b;
	
	r1 = color1 & 0x1f;
	g1 = (color1 >> 5) & 0x1f;
	b1 = (color1 >> 10) & 0x1f;
	
	r2 = color2 & 0x1f;
	g2 = (color2 >> 5) & 0x1f;
	b2 = (color2 >> 10) & 0x1f;
	
	r = (r1 + r2) >> 1;
	g = (g1 + g2) >> 1;
	b = (b1 + b2) >> 1;
	
	return b << 10 | g << 5 | r;
}

static inline void AGMV_DecompressLZSSAudio(AGMV* agmv, File* file, u8* bitstream_data, u32 usize, u32 csize){
	u32 bpos = 0;
	u8 byte = 0, flag_num = 0;
	u16 word, offset;
	u8 len, *src, *dest, msb, lsb, *buf;
	int window_start;

	do{
		if(flag_num == 0){
			byte = AGMV_ReadByte(file);
			flag_num = 8;
		}

		if(byte & 0x80){
			bitstream_data[bpos++] = AGMV_ReadByte(file);
		}
		else{
			lsb = AGMV_ReadByte(file);
			msb = AGMV_ReadByte(file);
			
			word = msb << 8 | lsb;
			
			offset = word >> 4;
			len = word & 0xF;

			window_start = bpos - offset;
			
			dest = &bitstream_data[bpos];
			src = &bitstream_data[window_start];
			
			AGMV_Memcpy8(dest,src,len);

			bpos += len;
		}

		byte <<= 1;
		flag_num--;
	}while(bpos < usize);
}

static inline void AGMV_DecompressLZSS(AGMV* agmv, File* file, u8* bitstream_data, u32 usize, u32 csize){
	u32 bpos = 0;
	u8 byte = 0, flag_num = 0;
	u16 word, offset;
	u8 len, *src, *dest, msb, lsb, *buf;
	int window_start;
	
	buf = agmv->bitstream->cbuffer;
	
	AGMV_ReadNBytes(file,buf,csize);

	do{
		if(flag_num == 0){
			byte = *(buf++);
			flag_num = 8;
		}

		if(byte & 0x80){
			bitstream_data[bpos++] = *(buf++);
		}
		else{
			lsb = *(buf++);
			msb = *(buf++);
			
			word = msb << 8 | lsb;
			
			offset = word >> 4;
			len = word & 0xF;

			window_start = bpos - offset;
			
			dest = &bitstream_data[bpos];
			src = &bitstream_data[window_start];
			
			AGMV_Memcpy8(dest,src,len);

			bpos += len;
		}

		byte <<= 1;
		flag_num--;
	}while(bpos < usize);
}

int AGMV_DecodeHeader(File* file, AGMV* agmv);
int AGMV_DecodeFrameChunk(File* file, AGMV* agmv);
int AGMV_DecodeAudioChunk(File* file, AGMV* agmv);

#endif