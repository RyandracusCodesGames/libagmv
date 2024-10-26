/********************************************
*   Adaptive Graphics Motion Video
*
*   Copyright (c) 2024 Ryandracus Chapman
*
*   Library: libagmv
*   File: agmv_psx_file_utils.c
*   Date: 10/1/2024
*   Version: 2.0
*   Updated: 10/25/2024
*   Author: Ryandracus Chapman
*
********************************************/
#include <agmv_psx_file_utils.h>
#include <agmv_psx_mem_utils.h>

File* AGMV_OpenFile(char* filename){
	File* file;
	u_char param;
	
	file = (File*)malloc3(sizeof(File));
	file->file = (CdlFILE*)malloc3(sizeof(CdlFILE));
	file->origin = (CdlLOC*)malloc3(sizeof(CdlLOC));
	file->buf = AGMV_NULL;
	file->file_pos = 0;
	file->buf_index = 0;
	file->error = NO_ERR;
	param = CdlModeSpeed;
	
	if(CdSearchFile(file->file,filename) == 0){
		AGMV_FreeMemory(file->file);
		AGMV_FreeMemory(file->origin);
		AGMV_FreeMemory(file);
		file->file = AGMV_NULL;
		file->origin = AGMV_NULL;
		file = AGMV_NULL;
		file->error = FILE_NOT_FOUND_ERR;
		return file;
	}
	
	file->origin->minute = file->file->pos.minute;
	file->origin->second = file->file->pos.second;
	file->origin->sector = file->file->pos.sector;
	file->origin->track  = file->file->pos.track;
	
	file->sector = CdPosToInt(&file->file->pos);
	
	file->buf = (u8*)malloc3(AGMV_CD_SEC_SIZE);
		
	CdControl(CdlSeekL,(u_char*)file->origin, 0);
	CdControl(CdlSetmode,&param,0);
	
	VSync(3);
	
	AGMV_CdRead(&file->file->pos,(u_long*)file->buf,AGMV_CD_SEC_COUNT);
	
	return file;
}

void AGMV_CloseFile(File* file){
	if(file != AGMV_NULL){
	
		if(file->file != AGMV_NULL){
			AGMV_FreeMemory(file->file);
			file->file = AGMV_NULL;
		}
		
		if(file->origin != AGMV_NULL){
			AGMV_FreeMemory(file->origin);
			file->origin = AGMV_NULL;
		}
	
		if(file->buf != AGMV_NULL){
			AGMV_FreeMemory(file->buf);
			file->buf = AGMV_NULL;
		}
		
		AGMV_FreeMemory(file);
		file = AGMV_NULL;
	}
}

void AGMV_ReadNBytes(File* file, u8* buf, u32 count){
	s32 buf_remnants, ccount;
	u32 file_pos, file_size, sector;
	u16  buf_index;
	u8* read_buf, remainder;
	s8 num_secs;
	CdlLOC* pos;
	
	buf_index = file->buf_index;
	buf_remnants = AGMV_CD_SEC_SIZE - buf_index;
	read_buf  = file->buf;
	
	if(buf_remnants >= count){
		AGMV_Memcpy8(buf, read_buf + buf_index, count);
		file->file_pos += count;
		file->buf_index += count;
		return;
	}
	
	pos = &file->file->pos;
	sector = file->sector;
	file_size = file->file->size;
	file_pos  = file->file_pos;
	ccount = count;

	AGMV_Memcpy8(buf, read_buf + buf_index, buf_remnants);
	buf += buf_remnants;
	file_pos += buf_remnants;
	ccount -= buf_remnants;
	buf_index = 0;
	
	num_secs = ccount / AGMV_CD_SEC_SIZE;
	
	sector += AGMV_CD_SEC_COUNT;
	CdIntToPos(sector,pos);
	CdControl(CdlReadS, (u_char *)pos, 0);
	
	while(num_secs-- && file_pos < file_size){
		AGMV_CdReadNoLOC((u_long*)read_buf,AGMV_CD_SEC_COUNT);
		AGMV_Memcpy8(buf, read_buf, AGMV_CD_SEC_SIZE);
		buf += AGMV_CD_SEC_SIZE; ccount -= AGMV_CD_SEC_SIZE;
		file_pos += AGMV_CD_SEC_SIZE; sector += AGMV_CD_SEC_COUNT;
	}
	
	AGMV_CdReadNoLOC((u_long*)read_buf,AGMV_CD_SEC_COUNT);
	AGMV_Memcpy8(buf, read_buf, ccount);
	
	buf_index += ccount; file_pos += ccount;
	
	CdIntToPos(sector,pos);
	CdControl(CdlSeekL,(u_char*)pos,0);

	file->sector = sector;
	file->file_pos = file_pos;
	file->buf_index = buf_index;
}

void AGMV_SeekBackwards(File* file, u32 count){
	s32 ccount, buf_index, file_pos, buf_remnants;
	u32 sector;
	u8* buf;
	CdlLOC* pos;
	
	buf_remnants = AGMV_CD_SEC_SIZE - file->buf_index;
	
	if(buf_remnants >= count){
		file->file_pos -= count;
		file->buf_index -= count;
		return;
	}
	
	buf = file->buf;
	buf_index = file->buf_index;
	ccount = count;
	file_pos  = file->file_pos;
	sector = file->sector;
	pos = &file->file->pos;
	
	while(ccount-- && file_pos > 0){
		if(buf_index == 0){
			buf_index = AGMV_CD_SEC_SIZE;
			sector -= AGMV_CD_SEC_COUNT;
		}
		
		buf_index--;
		file_pos--;
	}
	
	CdIntToPos(sector,pos);
	CdControl(CdlSeekL,(u_char*)pos,0);
	AGMV_CdRead(pos,(u_long*)buf,AGMV_CD_SEC_COUNT);
	
	if(file_pos < 0){
		file_pos = 0;
	}
	
	file->file_pos = file_pos;
	file->buf_index = buf_index;
	file->sector = sector;
}

void AGMV_SeekForwards(File* file, u32 count){
	s32 ccount, buf_remnants;
	u32 file_pos, file_size, i, remainder, sector;
	u16 buf_index;
	u8* buf;
	CdlLOC* pos = &file->file->pos;
	
	buf_index = file->buf_index;
	
	buf_remnants = AGMV_CD_SEC_SIZE - buf_index;
	
	if(buf_remnants >= count){
		file->file_pos += count;
		file->buf_index += count;
		return;
	}
	
	ccount = count;
	sector = file->sector;
	file_size = file->file->size;
	file_pos  = file->file_pos;
	buf = file->buf;

	while(ccount-- && file_pos < file_size){
	
		if(buf_index >= AGMV_CD_SEC_SIZE){
			buf_index = 0;
			sector += AGMV_CD_SEC_COUNT;
		}
	
		buf_index++;
		file_pos++;
	}
	
	CdIntToPos(sector,pos);
	CdControlB(CdlSeekL,(u_char*)pos,0);
	AGMV_CdRead(pos,(u_long*)buf,AGMV_CD_SEC_COUNT);
	
	file->sector = sector;
	file->file_pos = file_pos;
	file->buf_index = buf_index;
}

void AGMV_SeekSet(File* file){
	file->file_pos = 0;
	file->buf_index = 0;
	
	CdControlB(CdlSeekL,(u_char*)file->origin,0);
	
	file->sector = CdPosToInt(file->origin);
	
	file->file->pos.minute = file->origin->minute;
	file->file->pos.second = file->origin->second;
	file->file->pos.sector = file->origin->sector;
	file->file->pos.track  = file->origin->track;

	AGMV_CdRead(&file->file->pos,(u_long*)file->buf,AGMV_CD_SEC_COUNT);
}