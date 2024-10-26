#ifndef AGMV_NDS_FILE_UTILS_H
#define AGMV_NDS_FILE_UTILS_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_nds_file_utils.h
*   Date: 8/14/2024
*   Version: 2.0
*   Updated: 8/26/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv_nds_defines.h>

static inline Bool ITCM_CODE AGMV_EOF(FILE* file){
	u32 pos = ftell(file), file_size;
	fseek(file,0,SEEK_END);
	file_size = ftell(file);
	fseek(file,pos,SEEK_SET);
	
	if(pos >= file_size){
		return TRUE;
	}
	else return FALSE;
}

static inline u8 ITCM_CODE AGMV_ReadByte(FILE* file){
	u8 byte;
	fread(&byte,1,1,file);
	return byte;
}

static inline u16 ITCM_CODE AGMV_ReadShort(FILE* file){
	u8 lsb = AGMV_ReadByte(file);
	u8 msb = AGMV_ReadByte(file);
	
	return msb << 8 | lsb;
}

static inline u32 ITCM_CODE AGMV_ReadLong(FILE* file){
	u8 lsb1, lsb2, msb1, msb2;
	
	lsb1 = AGMV_ReadByte(file);
	lsb2 = AGMV_ReadByte(file);
	msb1 = AGMV_ReadByte(file);
	msb2 = AGMV_ReadByte(file);
	
	return msb2 << 24 | msb1 << 16 | lsb2 << 8 | lsb1;
}

static inline void ITCM_CODE AGMV_ReadNBytes(FILE* file, u8* buf, u32 size){
	fread(buf,1,size,file);
}

static inline void ITCM_CODE AGMV_ReadFourCC(FILE* file, char fourcc[4]){
	fourcc[0] = AGMV_ReadByte(file);
	fourcc[1] = AGMV_ReadByte(file);
	fourcc[2] = AGMV_ReadByte(file);
	fourcc[3] = AGMV_ReadByte(file);
}

static inline Bool AGMV_IsCorrectFourCC(char fourcc[4], char f, char o, char u, char r){
	if(f != fourcc[0] || o != fourcc[1] || u != fourcc[2] || r != fourcc[3]){
		return FALSE;
	}
	else return TRUE;
}

static inline void ITCM_CODE AGMV_FindNextFrameChunk(FILE* file){
	u32 pos;
	Bool isFrame;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isFrame = AGMV_IsCorrectFourCC(fourcc,'A','G','F','C');

	if(isFrame){
		pos = ftell(file);
		fseek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isFrame && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isFrame = AGMV_IsCorrectFourCC(fourcc,'A','G','F','C');
		
		if(!isFrame){
			pos = ftell(file);
			fseek(file,pos-3,SEEK_SET);
		}
	}

	pos = ftell(file);
	fseek(file,pos-4,SEEK_SET);
}

static inline void ITCM_CODE AGMV_FindNextAudioChunk(FILE* file){
	u32 pos;
	Bool isAudio;
	char fourcc[4];

	AGMV_ReadFourCC(file,fourcc);
	isAudio = AGMV_IsCorrectFourCC(fourcc,'A','G','A','C');

	if(isAudio){
		pos = ftell(file);
		fseek(file,pos-4,SEEK_SET);
		return;
	}

	while(!isAudio && !AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isAudio = AGMV_IsCorrectFourCC(fourcc,'A','G','A','C');
		
		if(!isAudio){
			pos = ftell(file);
			fseek(file,pos-3,SEEK_SET);
		}
	}

	pos = ftell(file);
	fseek(file,pos-4,SEEK_SET);
}

static inline u16 ITCM_CODE AGMV_SkipFrameChunk(FILE* file){
	u32 fourcc, frame_num, frame_type, usize, csize;

	AGMV_FindNextFrameChunk(file);

	fourcc     = AGMV_ReadLong(file);
	frame_num  = AGMV_ReadLong(file);
	frame_type = AGMV_ReadShort(file);
	usize      = AGMV_ReadLong(file);
	csize      = AGMV_ReadLong(file);

	fseek(file,csize,SEEK_CUR);
	
	return frame_type;
}

static inline void ITCM_CODE AGMV_SkipAudioChunk(FILE* file){
	u32 fourcc, size, csize;

	AGMV_FindNextAudioChunk(file);

	fourcc = AGMV_ReadLong(file);
	size   = AGMV_ReadLong(file);
	csize  = AGMV_ReadLong(file);

	fseek(file,csize,SEEK_CUR);
}

#endif