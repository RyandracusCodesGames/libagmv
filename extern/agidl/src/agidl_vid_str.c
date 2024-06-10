/********************************************
*   Adaptive Graphics Image Display Library
*
*   Copyright (c) 2023-2024 Ryandracus Chapman
*
*   Library: libagidl
*   File: agidl_vid_str.c
*   Date: 3/15/2024
*   Version: 0.4b
*   Updated: 3/16/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agidl_vid_str.h>

#include <stdlib.h>
#include <string.h>

#include <agidl_file_utils.h>
#include <agidl_img_error.h>
#include <agidl_math_utils.h>

const int PSX_QUANTIZATION_TABLE[64] = {	
	2, 16, 19, 22, 26, 27, 29, 34,
	16, 16, 22, 24, 27, 29, 34, 37,
	19, 22, 26, 27, 29, 34, 34, 38,
	22, 22, 26, 27, 29, 34, 37, 40,
	22, 26, 27, 29, 32, 35, 40, 48,
	26, 27, 29, 32, 35, 40, 48, 58,
	26, 27, 29, 34, 38, 46, 56, 69,
	27, 29, 35, 38, 46, 56, 69, 83
};

const int PSX_ALT_QUANTIZATION_TABLE[64] = {
	2, 16, 16, 19, 16, 19, 22, 22,
	22, 22, 22, 22, 26, 24, 26, 27,
	27, 27, 26, 26, 26, 26, 27, 27,
	27, 29, 29, 29, 34, 34, 34, 29,
	29, 29, 27, 27, 29, 29, 32, 32,
	34, 34, 37, 38, 37, 35, 35, 34,
	35, 38, 38, 40, 40, 40, 48, 48,
	46, 46, 56, 56, 58, 69, 69, 83
};

const int PSX_ZIGZAG_TABLE[64] = {
    0,   1,   5,  6,   14,  15,  27,  28,
    2,   4,   7,  13,  16,  26,  29,  42,
    3,   8,  12,  17,  25,  30,  41,  43,
    9,   11, 18,  24,  31,  40,  44,  53,
    10,  19, 23,  32,  39,  45,  52,  54,
    20,  22, 33,  38,  46,  51,  55,  60,
    21,  34, 37,  47,  50,  56,  59,  61,
    35,  36, 48,  49,  57,  58,  62,  63,
};

const int PSX_REVERSE_ZIGZAG_TABLE[64] = {
	 0, 1, 8, 16, 9, 2, 3, 10,
	 17, 24, 32, 25, 18, 11, 4, 5,
	 12, 19, 26, 33, 40, 48, 41, 34,
	 27, 20, 13, 6, 7, 14, 21, 28,
	 35, 42, 49, 56, 57, 50, 43, 36,
	 29, 22, 15, 23, 30, 37, 44, 51,
	 58, 59, 52, 45, 38, 31, 39, 46,
	 53, 60, 61, 54, 47, 55, 62, 63
};

const f32 PSX_IDCT_TABLE[8][8] = {
	{1.0000,0.9816,0.9272,0.8387,0.7071,0.5592,0.3907,0.2079},
	{1.0000,0.8387,0.3907,-0.1908,-0.7071,-0.9781,-0.9272,-0.5592},
	{1.0000,0.5592,-0.3746,-0.9781,-0.7071,0.1908,0.9205,0.8387},
	{1.0000,0.2079,-0.9205,-0.5592,0.7071,0.8387,-0.3746,-0.9816},
	{1.0000,-0.1908,-0.9272,0.5446,0.7071,-0.8290,-0.3907,0.9781},
	{1.0000,-0.5446,-0.3907,0.9816,-0.6947,-0.2079,0.9272,-0.8290},
	{1.0000,-0.8290,0.3746,0.2079,-0.7071,0.9816,-0.9205,0.5446},
	{1.0000,-0.9781,0.9205,-0.8290,0.7071,-0.5446,0.3746,-0.1908},
};

const AGIDL_AC_COEFF_LUT PSX_AC_COEFF_LUT[111] = {
	{"11",       0,1},
	{"011",      1,1},
	{"0100",     1,1},
	{"0101",     2,1},
	{"00101",    0,3},
	{"00110",    4,1},
	{"00111",    3,1},
	{"000100",   7,1},
	{"000101",   6,1},
	{"000110",   1,2},
	{"000111",   5,1},
	{"0000100",  2,2},
	{"0000101",  9,1},
	{"0000110",  0,4},
	{"0000111",  8,1},
	{"00100000",13,1},
	{"00100001", 0,6},
	{"00100010", 12,1},
	{"00100011",11,1},
	{"00100100", 3,2},
	{"00100101", 1,3},
	{"00100110", 0,5},
	{"00100111",10,1},
	{"0000001000",16,1},
	{"0000001001",5,2},
	{"0000001010",0 ,  7},
	{"0000001011",2 ,  3},
	{"0000001100",1 ,  4},
	{"0000001101",15,  1},
	{"0000001110",14,  1},
	{"0000001111",4 ,  2},
	{"000000010000",0 , 11},
	{"000000010001",8 ,  2},
	{"000000010010",4 ,  3},
	{"000000010011",0 , 10},
	{"000000010100",2 ,  4},
	{"000000010101",7 ,  2},
	{"000000010110",21,  1},
	{"000000010111",20,  1},
	{"000000011000",0 ,  9},
	{"000000011001",19,  1},
	{"000000011010",18,  1},
	{"000000011011",1 ,  5},
	{"000000011100",3 ,  3},
	{"000000011101",0 ,  8},
	{"000000011110",6 ,  2},
	{"000000011111",17,  1},
	{"0000000010000",10,  2},
	{"0000000010001",9 ,  2},
	{"0000000010010",5 ,  3},
	{"0000000010011",3 ,  4},
	{"0000000010100",2 ,  5},
	{"0000000010101",1 ,  7},
	{"0000000010110",1 ,  6},
	{"0000000010111",0 , 15},
	{"0000000011000",0 , 14},
	{"0000000011001",0 , 13},
	{"0000000011010",0 , 12},
	{"0000000011011",26,  1},
	{"0000000011100",25,  1},
	{"0000000011101",24,  1},
	{"0000000011110",23,  1},
	{"0000000011111",22,  1},
	{"00000000010000",0 , 31},
	{"00000000010001",0 , 30},
	{"00000000010010",0 , 29},
	{"00000000010011",0 , 28},
	{"00000000010100",0 , 27},
	{"00000000010101",0 , 26},
	{"00000000010110",0 , 25},
	{"00000000010111",0 , 24},
	{"00000000011000",0 , 23},
	{"00000000011001",0 , 22},
	{"00000000011010",0 , 21},
	{"00000000011011",0 , 20},
	{"00000000011100",0 , 19},
	{"00000000011101",0 , 18},
	{"00000000011110",0 , 17},
	{"00000000011111",0 , 16},
	{"000000000010000",0 , 40},
	{"000000000010001",0 , 39},
	{"000000000010010",0 , 38},
	{"000000000010011",0 , 37},
	{"000000000010100",0 , 36},
	{"000000000010101",0 , 35},
	{"000000000010110",0 , 34},
	{"000000000010111",0 , 33},
	{"000000000011000",0 , 32},
	{"000000000011001",1 , 14},
	{"000000000011010",1 , 13},
	{"000000000011011",1 , 12},
	{"000000000011100",1 , 11},
	{"000000000011101",1 , 10},
	{"000000000011110",1 ,  9},
	{"000000000011111",1 ,  8},
	{"0000000000010000",1 , 18},
	{"0000000000010001",1 , 17},
	{"0000000000010010",1 , 16},
	{"0000000000010011",1 , 15},
	{"0000000000010100",6 ,  3},
	{"0000000000010101",16,  2},
	{"0000000000010110",15,  2},
	{"0000000000010111",14,  2},
	{"0000000000011000",13,  2},
	{"0000000000011001",12,  2},
	{"0000000000011010",11,  2},
	{"0000000000011011",31,  1},
	{"0000000000011100",30,  1},
	{"0000000000011101",29,  1},
	{"0000000000011110",28,  1},
	{"0000000000011111",27,  1},
};

/* PARSING FUNCTIONS */
void AGIDL_ReadMDECVideoFrame(FILE* file, AGIDL_MDEC_FRAME* frame){
	frame->magic = AGIDL_ReadLong(file);
	frame->multi_chunk_num = AGIDL_ReadShort(file);
	frame->total_multi_chunk_num = AGIDL_ReadShort(file);
	frame->frame_num = AGIDL_ReadLong(file);
	frame->size = AGIDL_ReadLong(file);
	frame->width = AGIDL_ReadShort(file);
	frame->height = AGIDL_ReadShort(file);
	frame->num_blocks = AGIDL_ReadShort(file);
	frame->magic_const = AGIDL_ReadShort(file);
	frame->quant_scale = AGIDL_ReadShort(file);
	frame->version = AGIDL_ReadShort(file);
	frame->pad = AGIDL_ReadLong(file);
	
	frame->size = (MDEC_BLOCK_SIZE/2)*frame->total_multi_chunk_num;
	
	AGIDL_EnsureDimMul16(frame);
}

void AGIDL_ReadMultiplexStream(FILE* file, AGIDL_MDEC_FRAME* frame){
	frame->point = 0;
	
	if(frame->no_audio){
		fread(frame->data,2,MDEC_BLOCK_SIZE/2,file);
		
		frame->point += MDEC_BLOCK_SIZE/2;

		for(int i = 1; i < frame->total_multi_chunk_num; i++){
			AGIDL_FindNextVideoFrame(file);
			fseek(file,32,SEEK_CUR);
			fread(frame->data+frame->point,2,MDEC_BLOCK_SIZE/2,file);
			frame->point += MDEC_BLOCK_SIZE/2;
		}
		
		frame->point = 0;
	}
	else{
		fread(frame->data,2,MDEC_BLOCK_SIZE/2,file);

		frame->point += MDEC_BLOCK_SIZE/2;

		for(int i = 1; i < frame->total_multi_chunk_num; i++){
			AGIDL_FindNextVideoFrame(file);
			fseek(file,32,SEEK_CUR);
			fread(frame->data+frame->point,2,MDEC_BLOCK_SIZE/2,file);
			frame->point += MDEC_BLOCK_SIZE/2; 
		}
		
		frame->point = 0;
	}
}


u16 AGIDL_GetNext16Bits(AGIDL_MDEC_FRAME* frame){
	return frame->data[frame->point++];
}

u16 AGIDL_GetCur16Bits(const AGIDL_MDEC_FRAME* frame){
	return frame->data[frame->point];
}

u16 AGIDL_PeekNext16Bits(const AGIDL_MDEC_FRAME* frame){
	u16 point = frame->point;
	return frame->data[point++];
}

void AGIDL_SkipNWords(AGIDL_MDEC_FRAME* frame, const int n){
	frame->point += n;
}

void AGIDL_SeekMDEC(AGIDL_MDEC_FRAME* frame, const int n){
	frame->point = n;
}

AGIDL_Bool AGIDL_EOF(FILE* file){
	const u32 pos = ftell(file);
	fseek(file,0,SEEK_END);
	const u32 file_size = ftell(file);
	fseek(file,pos,SEEK_SET);
	
	if(pos >= file_size){
		return TRUE;
	}
	return FALSE;
}

u32 mdec_count = 0;

void AGIDL_PrintMDECBitstream(const AGIDL_MDEC_FRAME* frame){
	char filename[30]; mdec_count++;
	sprintf(filename,"macro_%ld.bs",mdec_count);
	
	FILE* file = fopen(filename,"wb");

	for(int i = 0; i < frame->size; i++){
		AGIDL_WriteShort(file,frame->data[i]);
	}
	
	fclose(file);
}

/* UTILITY FUNCTIONS */
s32 extend_sign(const u16 bit_size, const u64 n){
	const u32 mask = ((1LL << (bit_size - 1)) - 1);
	const AGIDL_Bool sign = (n & (1LL << (bit_size - 1))) != 0;

    s32 val = n & mask;
    if (sign) val |= ~mask;
    return val;
}

int AGIDL_IsMDECVideoFrame(const AGIDL_MDEC_FRAME* frame){
	if(frame->magic != MDEC_MAGIC || frame->magic_const != MDEC_CONST || frame->pad != 0 
	|| !(frame->version == 1 || frame->version == 2 || frame->version == 3) || frame->quant_scale > 63){
		return INVALID_HEADER_FORMATTING_ERROR;
	}
	return NO_IMG_ERROR;
}

void AGIDL_EnsureDimMul16(AGIDL_MDEC_FRAME* frame){
	while(frame->width % 16 != 0){
		frame->width++;
	}
	while(frame->height % 16 != 0){
		frame->height++;
	}
}

u32 AGIDL_CalcMacroblockDim(const u32 size){
	return (u32)((size + 15) / 16.0f);
}

void AGIDL_FindNextVideoFrame(FILE* file){
	u32 magic = AGIDL_ReadLong(file);
	
	if(magic == MDEC_MAGIC){
		const u32 pos = ftell(file);
		fseek(file,pos-4,SEEK_SET);
		return;
	}
	
	while(magic != MDEC_MAGIC && AGIDL_EOF(file) == FALSE){
		magic = AGIDL_ReadLong(file);
	}

	const u32 pos = ftell(file);
	fseek(file,pos-4,SEEK_SET);
}

void AGIDL_MDECAllocResources(AGIDL_MDEC_FRAME* frame){
	frame->point = 0;
	frame->size = (MDEC_BLOCK_SIZE/2)*frame->total_multi_chunk_num;
	frame->data = (u16*)malloc(sizeof(u16)*MDEC_BLOCK_SIZE*frame->total_multi_chunk_num);
}

void AGIDL_FillBlock(s16 block[64]){
	for(int i = 0; i < 64; i++){
		block[i] = 0;
	}
}

void AGIDL_TransformBlockToMacroblock(s16 block[64], s16 macroblock[8][8]){
	for(int y = 0; y < 8; y++){
		for(int x = 0; x < 8; x++){
			const int index = x + y * 8;
			macroblock[x][y] = block[index];
		}
	}
}

void AGIDL_TransformMaroblockToBlock(s16 block[64], s16 macroblock[8][8]){
	for(int y = 0; y < 8; y++){
		for(int x = 0; x < 8; x++){
			const int index = x + y * 8;
			block[index] = macroblock[x][y];
		}
	}
}

void AGIDL_AddMDECCode(AGIDL_MDEC_FRAME* frame, const AGIDL_MDEC_CODE code){
	if(frame->len < frame->capacity - 1){
		frame->mdec[frame->len++] = code;
	}
	else{
		const u32 old_len = frame->len; frame->len++;
		
		u16* prev = malloc(sizeof(u16)*old_len);
		
		memcpy(prev,frame->mdec,old_len);
		
		free(frame->mdec);
		
		frame->mdec = (u16*)malloc(sizeof(u16)*frame->capacity<<1);
		memcpy(frame->mdec,prev,old_len);
		frame->mdec[old_len] = code;
		
		frame->capacity <<= 1;
		
		free(prev);
	}
}

void AGIDL_CreateMDECList(AGIDL_MDEC_FRAME* frame){
	frame->mdec = (u16*)malloc(sizeof(u16)*16);
	frame->capacity = 16;
	frame->len = 0;
}

AGIDL_Bool print = FALSE;

void AGIDL_InitMDECPrint(){
	print = TRUE;
}

void AGIDL_DisableMDECPrint(){
	print = FALSE;
}

/* PRIMARY STR DECODING FUNCTION CALLS */
float C(const int u){
	if(u == 0){
		return 0.3536f;
	}
	return 0.5f;
}

int AGIDL_SingleIDCT(const int x, const int y, s16 macroblock[8][8]){
	float summation = 0;

	for(int v = 0; v < 8; v++){
		for(int u = 0; u < 8; u++){
			summation += (C(u) * C(v)) * macroblock[u][v] * PSX_IDCT_TABLE[x][u] * PSX_IDCT_TABLE[y][v];
		}
	}
	
	return summation;
}

void AGIDL_IDCT(s16 macroblock[8][8]){
	s16 out[8][8];
	int x,y;
	for(y = 0; y < 8; y++){
		for(x = 0; x < 8; x++){
			out[x][y] = AGIDL_SingleIDCT(x,y,macroblock);
		}
	}
	
	for(y = 0; y < 8; y++){
		for(x = 0; x < 8; x++){
			macroblock[x][y] = out[x][y];
		}
	}
}

void AGIDL_DecodeMacroblock(AGIDL_MDEC_FRAME* frame, s16 block[64], const int table[64]){
	AGIDL_FillBlock(block);
	
	while(frame->point <= frame->size && AGIDL_GetCur16Bits(frame) == 0xfe00){
		frame->point++;
	}

	const u16 word = AGIDL_GetNext16Bits(frame);
	
	s16 dc = extend_sign(10,word & BOTTOM_10_BITS);
	const s16 quant = (word >> 10) & 0x3f;
	
	s16 val = dc * table[0];

	for (int n = 0; n < 64;){
        if(quant == 0){
            val = dc << 1;
        }

        val = AGIDL_Clamp(-0x400,val,0x3ff);

        if(quant > 0){
            block[PSX_ZIGZAG_TABLE[n]] = val;
        } 
		else if(quant == 0){
            block[n] = val;
        }

        const u16 rle = AGIDL_GetNext16Bits(frame);
        const u16 zeros = (word >> 10) & 0x3f;
		
        dc = extend_sign(10, rle & BOTTOM_10_BITS);
        n += zeros + 1;

        val = (dc * table[n] * quant + 4) >> 3;
    }


	s16 macroblock[8][8];
	AGIDL_TransformBlockToMacroblock(block,macroblock);
	AGIDL_IDCT(macroblock);
	AGIDL_TransformMaroblockToBlock(block,macroblock);
}

int AGIDL_MDEC(const char* filename){
	FILE* file = fopen(filename,"rb");
	
	if(file == NULL){
		return FILE_NOT_LOCATED_IMG_ERROR;
	}

	const u32 magic = AGIDL_ReadLong(file); fseek(file,0,SEEK_SET);
	
	if(magic == MDEC_MAGIC){
		AGIDL_MDEC_FRAME* frame = malloc(sizeof(AGIDL_MDEC_FRAME));
		
		frame->no_audio = TRUE;
		
		AGIDL_ReadMDECVideoFrame(file,frame);
		const int error = AGIDL_IsMDECVideoFrame(frame);
		
		if(error == NO_IMG_ERROR){
			AGIDL_MDECAllocResources(frame);
			AGIDL_ReadMultiplexStream(file,frame);
		  //AGIDL_GenerateMDECPipelineCodes(frame);
		  //AGIDL_DecodeMacroblock(frame);
		  //AGIDL_ConvertYCbCrToRGB(frame);	
			
			if(print == TRUE){
				AGIDL_PrintMDECBitstream(frame);
			}
			
			while(AGIDL_EOF(file) != TRUE){
				AGIDL_FindNextVideoFrame(file);
				AGIDL_ReadMDECVideoFrame(file,frame);
				AGIDL_ReadMultiplexStream(file,frame);
			  //AGIDL_GenerateMDECPipelineCodes(frame);
			  //AGIDL_DecodeMacroblock(frame);
			  //AGIDL_ConvertYCbCrToRGB(frame);	
			  
				if(print == TRUE){
					AGIDL_PrintMDECBitstream(frame);
				}
			}
		}
		
		free(frame->data);
		free(frame);
		
		return error;
	}
	AGIDL_FindNextVideoFrame(file);
	AGIDL_MDEC_FRAME* frame = malloc(sizeof(AGIDL_MDEC_FRAME));
	
	frame->no_audio = FALSE;
		
	AGIDL_ReadMDECVideoFrame(file,frame);
	const int error = AGIDL_IsMDECVideoFrame(frame);
	
	if(error == NO_IMG_ERROR){
		AGIDL_MDECAllocResources(frame);
		AGIDL_ReadMultiplexStream(file,frame);
			
		if(print == TRUE){
			AGIDL_PrintMDECBitstream(frame);
		}
			
		while(AGIDL_EOF(file) != TRUE){
			AGIDL_FindNextVideoFrame(file);
			AGIDL_ReadMDECVideoFrame(file,frame);
			AGIDL_ReadMultiplexStream(file,frame);
				
			if(print == TRUE){
				AGIDL_PrintMDECBitstream(frame);
			}
		}
	}

	free(frame->data);
	free(frame);
		
	return error;

	fclose(file);
	
	return NO_IMG_ERROR;
}
