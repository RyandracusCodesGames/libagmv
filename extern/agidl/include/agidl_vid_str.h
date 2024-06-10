#ifndef AGIDL_VID_STR_H
#define AGIDL_VID_STR_H

/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_vid_str.h
*   Date: 3/15/2024
*   Version: 0.4b
*   Updated: 3/15/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_img_types.h>
#include <agidl_types.h>

#include <stdio.h>

#define MDEC_MAGIC 				0x80010160
#define MDEC_CONST 	 			0x3800
#define MDEC_BLOCK_SIZE 		2016
#define MDEC_AV_BLOCK_SIZE		2352
#define BOTTOM_10_BITS          0x3ff
#define MDEC_END_OF_DATA        0xFE00

typedef u16 AGIDL_MDEC_CODE;

typedef struct AGIDL_AC_COEFF_LUT{
	const char* bin;
	u8 zero_run;
	u8 ac;
}AGIDL_AC_COEFF_LUT;

typedef struct AGIDL_MDEC_YCbCr{
	int y, cb, cr;
}AGIDL_MDEC_YCbCr;

typedef struct AGIDL_MDEC_FRAME{
	u32 magic;
	u16 multi_chunk_num;
	u16 total_multi_chunk_num;
	u32 frame_num;
	u32 size;
	u16 width;
	u16 height;
	u16 num_blocks;
	u16 magic_const;
	u16 quant_scale;
	u16 version;
	u32 pad;
	AGIDL_Bool no_audio;
	u16 bit;
	AGIDL_Bool next;
	u32 point;
	u16* data;
	u32 capacity;
	u32 len;
	AGIDL_MDEC_CODE* mdec;
}AGIDL_MDEC_FRAME;	

/*PARSING FUNCTIONS*/
void AGIDL_ReadMDECVideoFrame(FILE* file, AGIDL_MDEC_FRAME* frame);
void AGIDL_ReadMultiplexStream(FILE* file, AGIDL_MDEC_FRAME* frame);
u16 AGIDL_GetNext16Bits(AGIDL_MDEC_FRAME* frame);
u16 AGIDL_PeekNext16Bits(const AGIDL_MDEC_FRAME* frame);
u16 AGIDL_GetCur16Bits(const AGIDL_MDEC_FRAME* frame);
void AGIDL_SkipNWords(AGIDL_MDEC_FRAME* frame, int n);
AGIDL_Bool AGIDL_EOF(FILE* file);
void AGIDL_PrintMDECBitstream(const AGIDL_MDEC_FRAME* frame);

/* UTILITY FUNCTIONS */
int AGIDL_IsMDECVideoFrame(const AGIDL_MDEC_FRAME* frame);
void AGIDL_EnsureDimMul16(AGIDL_MDEC_FRAME* frame);
u32 AGIDL_CalcMacroblockDim(u32 size);
void AGIDL_FindNextVideoFrame(FILE* file);
void AGIDL_MDECAllocResources(AGIDL_MDEC_FRAME* frame);
void AGIDL_FillBlock(s16 block[64]);
void AGIDL_TransformBlockToMacroblock(s16 block[64], s16 macroblock[8][8]);
void AGIDL_TransformMaroblockToBlock(s16 block[64], s16 macroblock[8][8]);
void AGIDL_AddMDECCode(AGIDL_MDEC_FRAME* frame, AGIDL_MDEC_CODE code);
void AGIDL_CreateMDECList(AGIDL_MDEC_FRAME* frame);
void AGIDL_InitMDECPrint();
void AGIDL_DisableMDECPrint();


/* PRIMARY STR DECODING FUNCTION CALLS */
float C(int u);
int AGIDL_SingleIDCT(int x, int y, s16 macroblock[8][8]);
void AGIDL_IDCT(s16 macroblock[8][8]);
void AGIDL_DecodeMacroblock(AGIDL_MDEC_FRAME* frame, s16 block[64], const int table[64]);
int AGIDL_MDEC(const char* filename);


#endif
