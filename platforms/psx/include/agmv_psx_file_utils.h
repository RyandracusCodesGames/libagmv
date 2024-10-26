#ifndef AGMV_PSX_FILE_UTILS_H
#define AGMV_PSX_FILE_UTILS_H

/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_psx_file_utils.h
*   Date: 10/1/2024
*   Version: 2.0
*   Updated: 10/26/2024
*   Author: Ryandracus Chapman
*
********************************************/

#include <agmv_psx_defines.h>
#include <agmv_psx_mem_utils.h>

static inline void AGMV_CdRead(CdlLOC *loc, u32 *buf, int nsec){
	CdControl(CdlReadS, (u_char *)loc, 0);
	while(nsec--){
		/* transfer to main memory as soon data is ready */
		while(CdReady(1, 0) != CdlDataReady);
		CdGetSector(buf, 512);
		buf += 512;
	}
}

static inline void AGMV_CdReadNoLOC(u32 *buf, int nsec){
	while(nsec--){
		/* transfer to main memory as soon data is ready */
		while(CdReady(1, 0) != CdlDataReady);
		CdGetSector(buf, 512);
		buf += 512;
	}
}

static inline Bool AGMV_EOF(File* file){
	if(file->file_pos >= file->file->size){
		return AGMV_TRUE;
	}
	else return AGMV_FALSE;
}

static inline u32 AGMV_Ftell(File* file){
	return file->file_pos;
}

static inline Bool AGMV_IsCorrectFourCC(char fourcc[4], char f, char o, char u, char r){
	if(fourcc[0] == f && fourcc[1] == o && fourcc[2] == u && fourcc[3] == r){
		return AGMV_TRUE;
	}
	else return AGMV_FALSE;
}

File* AGMV_OpenFile(char* filename);
void AGMV_CloseFile(File* file);
void AGMV_SetCDMode(u32 mode);

void AGMV_ReadNBytes(File* file, u8* buf, u32 count);
void AGMV_SeekBackwards(File* file, u32 count);
void AGMV_SeekForwards(File* file, u32 count);
void AGMV_SeekSet(File* file);

static inline u8 AGMV_ReadByte(File* file){
	u8 byte;
	AGMV_ReadNBytes(file,(char*)&byte,1);
	return byte;
}

static inline u16 AGMV_ReadShort(File* file){
	u8 msb, lsb;
	
	msb = AGMV_ReadByte(file);
	lsb = AGMV_ReadByte(file);
	
	return lsb << 8 | msb;
}

static inline u32 AGMV_ReadLong(File* file){
	u8 msb2, msb1, lsb2, lsb1;
	
	msb1 = AGMV_ReadByte(file);
	msb2 = AGMV_ReadByte(file);
	lsb1 = AGMV_ReadByte(file);
	lsb2 = AGMV_ReadByte(file);
	
	return lsb2 << 24 | lsb1 << 16 | msb2 << 8 | msb1;
}

static inline void AGMV_ReadFourCC(File* file, char fourcc[4]){
	AGMV_ReadNBytes(file,fourcc,4);
}

static inline void AGMV_FindNextFourCC(File* file, char f, char o, char u, char r){
	Bool isFourcc;
	char fourcc[4];
	
	AGMV_ReadFourCC(file,fourcc);
	isFourcc = AGMV_IsCorrectFourCC(fourcc,f,o,u,r);
	
	if(isFourcc){
		AGMV_SeekBackwards(file,4);
		return;
	}
	
	while(!AGMV_EOF(file)){
		AGMV_ReadFourCC(file,fourcc);
		isFourcc = AGMV_IsCorrectFourCC(fourcc,f,o,u,r);
		
		if(isFourcc){
			AGMV_SeekBackwards(file,4);
			return;
		}
		else{
			AGMV_SeekBackwards(file,3);
		}
	}
	
	AGMV_SeekBackwards(file,4);
}

static inline void AGMV_FindNextFrameChunk(File* file){
	AGMV_FindNextFourCC(file,'A','G','F','C');
}

static inline void AGMV_FindNextAudioChunk(File* file){
	AGMV_FindNextFourCC(file,'A','G','A','C');
}

static inline u16 AGMV_SkipFrameChunk(File* file){
	u32 fourcc, frame_num, frame_type, usize, csize;
	
	AGMV_FindNextFrameChunk(file);
	
	fourcc     = AGMV_ReadLong(file);
	frame_num  = AGMV_ReadLong(file);
	frame_type = AGMV_ReadShort(file);
	usize      = AGMV_ReadLong(file);
	csize      = AGMV_ReadLong(file);
	
	AGMV_SeekForwards(file,csize);
	
	return frame_type;
}

static inline u16 AGMV_PeekFrameChunk(File* file){
	u32 fourcc, frame_num, frame_type, usize, csize;
	
	AGMV_FindNextFrameChunk(file);
	
	fourcc     = AGMV_ReadLong(file);
	frame_num  = AGMV_ReadLong(file);
	frame_type = AGMV_ReadShort(file);
	
	AGMV_SeekBackwards(file,10);
	
	return frame_type;
}

static inline void AGMV_SkipAudioChunk(File* file){
	u32 fourcc, size, csize;
	
	AGMV_FindNextAudioChunk(file);
	
	fourcc = AGMV_ReadLong(file);
	size   = AGMV_ReadLong(file);
	csize  = AGMV_ReadLong(file);
	
	AGMV_SeekForwards(file,csize);
}

#endif